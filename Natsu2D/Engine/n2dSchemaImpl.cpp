#include "n2dSchemaImpl.h"
#include <iterator>

namespace
{
	///	@brief	判断路径是否安全
	///	@note	若指定的路径可能访问到根路径以上的路径则判断为不安全
	nBool IsPathSafe(const nChar* path)
	{
		nInt depth = 0;
		while (*path)
		{
			if (*path == '.' && *++path == '.' && --depth < 0)
			{
				return false;
			}

			if (*path == std::experimental::filesystem::path::preferred_separator && *++path != std::experimental::filesystem::path::preferred_separator)
			{
				++depth;
			}

			++path;
		}

		return true;
	}

	time_t FileTimeToTime_T(FILETIME const& FileTime)
	{
		ULARGE_INTEGER uli;
		uli.HighPart = FileTime.dwHighDateTime;
		uli.LowPart = FileTime.dwLowDateTime;

		return uli.QuadPart / 10000000ull - 11644473600ull;
	}

	template <nBool test>
	struct PathTransformer
	{
		static nString Impl(const nChar* path)
		{
			nString tPath;
			for (auto pRead = path; *pRead; ++pRead)
			{
				tPath.Append(*pRead == '/' ? static_cast<nChar>(std::experimental::filesystem::path::preferred_separator) : *pRead);
			}
			
			return std::move(tPath);
		}
	};

	template <>
	struct PathTransformer<true>
	{
		static nString Impl(nStrView path)
		{
			return path;
		}
	};
}

const nString LocalFilesystemSchema::DefaultRootPath = WideStringView{ L'.', std::experimental::filesystem::path::preferred_separator, 0 };

LocalFilesystemSchema::LocalFilesystemStreamInfo::LocalFilesystemStreamInfo(LocalFilesystemSchema* pSchema, nStrView path)
	: m_pSchema(pSchema), m_Path(path)
{
	natFileStream{ path, false, false };
}

LocalFilesystemSchema::LocalFilesystemStreamInfo::~LocalFilesystemStreamInfo()
{
}

nStrView LocalFilesystemSchema::LocalFilesystemStreamInfo::GetPath() const
{
	return m_Path;
}

nResult LocalFilesystemSchema::LocalFilesystemStreamInfo::GetEditTime(std::chrono::system_clock::time_point& time) const
{
	natFileStream tFile{ m_Path, false, false };

	FILETIME editTime{ 0 };
	if (!GetFileTime(tFile.GetUnsafeHandle(), nullptr, nullptr, &editTime))
	{
		return NatErr_InternalErr;
	}

	time = std::chrono::system_clock::from_time_t(FileTimeToTime_T(editTime));
	return NatErr_OK;
}

nResult LocalFilesystemSchema::LocalFilesystemStreamInfo::GetSize(nLen& size) const
{
	natFileStream tFile{ m_Path, false, false };

	size = tFile.GetSize();
	return NatErr_OK;
}

natRefPointer<natStream> LocalFilesystemSchema::LocalFilesystemStreamInfo::OpenStream(nBool Readable, nBool Writable)
{
	return make_ref<natFileStream>(m_Path, Readable, Writable);
}

LocalFilesystemSchema::LocalFilesystemSchema(nStrView schemaName, nStrView rootPath)
	: m_SchemaName(schemaName), m_RootPath(rootPath)
{
	if (m_RootPath.empty())
	{
		m_RootPath = DefaultRootPath;
	}
	else if (m_RootPath[m_RootPath.size() - 1] != std::experimental::filesystem::path::preferred_separator)
	{
		m_RootPath += WideStringView{ std::experimental::filesystem::path::preferred_separator };
	}

	if (!std::experimental::filesystem::exists(static_cast<std::wstring>(m_RootPath)))
	{
		nat_Throw(natException, "rootPath \"{0}\" doesn't exist."_nv, m_RootPath);
	}
}

LocalFilesystemSchema::~LocalFilesystemSchema()
{
}

nStrView LocalFilesystemSchema::GetName() const
{
	return m_SchemaName;
}

natRefPointer<IStreamInfo> LocalFilesystemSchema::GetStreamInfoFromPath(nStrView path)
{
	auto tPath{ PathTransformer<std::experimental::filesystem::path::preferred_separator == '/'>::Impl(path.data()) };
	if (!IsPathSafe(tPath.data()))
	{
		nat_Throw(natException, "path \"{0}\" is not safe."_nv, tPath);
	}

	return make_ref<LocalFilesystemStreamInfo>(this, tPath);
}

nBool LocalFilesystemSchema::PathExist(nStrView path) const
{
	return std::experimental::filesystem::exists(WideString{ getRealPath(path) }.data());
}

nResult LocalFilesystemSchema::RemoveFromPath(nStrView path)
{
	auto tPath{ PathTransformer<std::experimental::filesystem::path::preferred_separator == '/'>::Impl(path.data()) };
	if (!IsPathSafe(tPath.data()))
	{
		nat_Throw(natException, "path \"{0}\" is not safe."_nv, tPath);
	}

	std::experimental::filesystem::path realPath{ static_cast<std::wstring>(GetRootPath() + tPath) };
	return remove_all(realPath) > 0 ? NatErr_OK : NatErr_NotFound;
}

nResult LocalFilesystemSchema::EnumPath(nStrView path, nBool recursive, nBool includeFolder, std::function<nBool(IStreamInfo*)> enumCallback)
{
	auto realPath = getRealPath(path);
	if (!std::experimental::filesystem::exists(static_cast<std::wstring>(realPath)))
	{
		return NatErr_NotFound;
	}

	return enumPathImpl(static_cast<std::wstring>(nString{ path }), recursive, includeFolder, enumCallback);
}

nStrView LocalFilesystemSchema::GetRootPath() const noexcept
{
	return m_RootPath;
}

nString LocalFilesystemSchema::getRealPath(nStrView path) const
{
	auto tPath{ PathTransformer<std::experimental::filesystem::path::preferred_separator == '/'>::Impl(path.data()) };
	if (!IsPathSafe(tPath.data()))
	{
		nat_Throw(natException, "path \"{0}\" is not safe."_nv, tPath);
	}

	return GetRootPath() + tPath;
}

nResult LocalFilesystemSchema::enumPathImpl(std::experimental::filesystem::path const& path, nBool recursive, nBool includeFolder, std::function<nBool(IStreamInfo*)> enumCallback)
{
	for (auto&& item : path)
	{
		auto isdir = is_directory(item);
		if (includeFolder || !isdir)
		{
			if (!enumCallback(GetStreamInfoFromPath(nString{ WideString{ item.c_str() } })))
			{
				return NatErr_Interrupted;
			}
		}
		if (recursive && isdir)
		{
			if (enumPathImpl(item, recursive, includeFolder, enumCallback) == NatErr_Interrupted)
			{
				return NatErr_Interrupted;
			}
		}
	}

	return NatErr_OK;
}

natRefPointer<ISchema> n2dSchemaFactoryImpl::CreateLocalFilesystemSchema(nStrView schemaName) const
{
	return make_ref<LocalFilesystemSchema>(schemaName);
}

natRefPointer<ISchema> n2dSchemaFactoryImpl::CreateLocalFilesystemSchema(nStrView schemaName, nStrView rootPath) const
{
	return make_ref<LocalFilesystemSchema>(schemaName, rootPath);
}
