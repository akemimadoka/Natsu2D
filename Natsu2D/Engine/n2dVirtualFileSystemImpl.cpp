#include "n2dVirtualFileSystemImpl.h"

namespace
{
	nBool ParseUri(nStrView const& Uri, nString& SchemaName, nString& Path)
	{
		auto division = Uri.Find("://"_nv);
		if (division == nString::npos)
		{
			return false;
		}

		SchemaName.Assign(Uri.Slice(0, division));
		Path.Assign(Uri.Slice(division + 3, -1));
		return true;
	}
}

n2dVirtualFileSystemImpl::n2dVirtualFileSystemImpl()
{
}

n2dVirtualFileSystemImpl::~n2dVirtualFileSystemImpl()
{
}

nResult n2dVirtualFileSystemImpl::RegisterSchema(ISchema* pSchema)
{
	auto SchemaName = pSchema->GetName();
	if (m_SchemaMap.find(SchemaName) != m_SchemaMap.end())
	{
		return NatErr_Duplicated;
	}

	m_SchemaMap.emplace(SchemaName, pSchema);
	return NatErr_OK;
}

nBool n2dVirtualFileSystemImpl::SchemaExist(nStrView schemaName) const
{
	return m_SchemaMap.find(schemaName) != m_SchemaMap.end();
}

natRefPointer<ISchema> n2dVirtualFileSystemImpl::GetSchemaFromName(nStrView name)
{
	auto iter = m_SchemaMap.find(name);
	if (iter != m_SchemaMap.end())
	{
		return iter->second;
	}

	nat_Throw(natException, "No schema named \"{0}\"."_nv, name);
}

natRefPointer<IStreamInfo> n2dVirtualFileSystemImpl::GetStreamInfoFromUri(nStrView uri)
{
	nString schemaName, path;
	if (!ParseUri(uri, schemaName, path))
	{
		nat_Throw(natException, "Bad uri."_nv);
	}

	auto iter = m_SchemaMap.find(schemaName);
	if (iter != m_SchemaMap.end())
	{
		return iter->second->GetStreamInfoFromPath(path);
	}

	nat_Throw(natException, "No schema named \"{0}\"."_nv, schemaName);
}
