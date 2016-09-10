#include "n2dSchemaImpl.h"
#include <iterator>

namespace
{
	///	@brief	判断路径是否安全
	///	@note	若指定的路径可能访问到根路径以上的路径则判断为不安全
	nBool IsPathSafe(ncTStr path)
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
		static nTString Impl(ncTStr path)
		{
			nTString tPath;
			replace_copy(path, path + std::char_traits<nTChar>::length(path), back_inserter(tPath), _T('/'), std::experimental::filesystem::path::preferred_separator);
			return move(tPath);
		}
	};

	template <>
	struct PathTransformer<true>
	{
		static nTString Impl(ncTStr path)
		{
			return path;
		}
	};
}

LocalFilesystemSchema::LocalFilesystemStreamInfo::LocalFilesystemStreamInfo(LocalFilesystemSchema* pSchema, ncTStr path)
	: m_pSchema(pSchema), m_Path(path)
{
	natFileStream{ path, false, false };
}

LocalFilesystemSchema::LocalFilesystemStreamInfo::~LocalFilesystemStreamInfo()
{
}

ncTStr LocalFilesystemSchema::LocalFilesystemStreamInfo::GetPath() const
{
	return m_Path.c_str();
}

nResult LocalFilesystemSchema::LocalFilesystemStreamInfo::GetEditTime(std::chrono::system_clock::time_point& time) const
{
	natFileStream tFile{ m_Path.c_str(), false, false };

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
	natFileStream tFile{ m_Path.c_str(), false, false };

	size = tFile.GetSize();
	return NatErr_OK;
}

natRefPointer<natStream> LocalFilesystemSchema::LocalFilesystemStreamInfo::OpenStream(nBool Readable, nBool Writable)
{
	return make_ref<natFileStream>(m_Path.c_str(), Readable, Writable);
}

LocalFilesystemSchema::LocalFilesystemSchema(ncTStr schemaName, ncTStr rootPath)
	: m_SchemaName(schemaName), m_RootPath(rootPath)
{
	if (m_RootPath.empty())
	{
		m_RootPath = DefaultRootPath;
	}
	else if (m_RootPath.back() != std::experimental::filesystem::path::preferred_separator)
	{
		m_RootPath += std::experimental::filesystem::path::preferred_separator;
	}

	if (!std::experimental::filesystem::exists(m_RootPath))
	{
		nat_Throw(natException, _T("rootPath \"{0}\" doesn't exist."), m_RootPath);
	}
}

LocalFilesystemSchema::~LocalFilesystemSchema()
{
}

ncTStr LocalFilesystemSchema::GetName() const
{
	return m_SchemaName.c_str();
}

natRefPointer<IStreamInfo> LocalFilesystemSchema::GetStreamInfoFromPath(ncTStr path)
{
	auto tPath{ PathTransformer<std::experimental::filesystem::path::preferred_separator == _T('/')>::Impl(path) };
	if (!IsPathSafe(tPath.c_str()))
	{
		nat_Throw(natException, _T("path \"{0}\" is not safe."), tPath);
	}

	return make_ref<LocalFilesystemStreamInfo>(this, tPath.c_str());
}

nBool LocalFilesystemSchema::PathExist(ncTStr path) const
{
	return std::experimental::filesystem::exists(getRealPath(path));
}

nResult LocalFilesystemSchema::RemoveFromPath(ncTStr path)
{
	auto tPath{ PathTransformer<std::experimental::filesystem::path::preferred_separator == _T('/')>::Impl(path) };
	if (!IsPathSafe(tPath.c_str()))
	{
		nat_Throw(natException, _T("path \"{0}\" is not safe."), tPath);
	}

	std::experimental::filesystem::path realPath{ GetRootPath() + tPath };
	return remove_all(realPath) > 0 ? NatErr_OK : NatErr_NotFound;
}

nResult LocalFilesystemSchema::EnumPath(ncTStr path, nBool recursive, nBool includeFolder, std::function<nBool(IStreamInfo*)> enumCallback)
{
	auto realPath = getRealPath(path);
	if (!std::experimental::filesystem::exists(realPath))
	{
		return NatErr_NotFound;
	}

	return enumPathImpl(path, recursive, includeFolder, enumCallback);
}

ncTStr LocalFilesystemSchema::GetRootPath() const noexcept
{
	return m_RootPath.c_str();
}

nTString LocalFilesystemSchema::getRealPath(ncTStr path) const
{
	auto tPath{ PathTransformer<std::experimental::filesystem::path::preferred_separator == _T('/')>::Impl(path) };
	if (!IsPathSafe(tPath.c_str()))
	{
		nat_Throw(natException, _T("path \"{0}\" is not safe."), tPath);
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
			if (!enumCallback(GetStreamInfoFromPath(item.c_str())))
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

natRefPointer<ISchema> n2dSchemaFactoryImpl::CreateLocalFilesystemSchema(ncTStr schemaName) const
{
	return make_ref<LocalFilesystemSchema>(schemaName);
}

natRefPointer<ISchema> n2dSchemaFactoryImpl::CreateLocalFilesystemSchema(ncTStr schemaName, ncTStr rootPath) const
{
	return make_ref<LocalFilesystemSchema>(schemaName, rootPath);
}
