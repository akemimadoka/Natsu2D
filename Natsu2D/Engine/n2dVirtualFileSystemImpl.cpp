#include "n2dVirtualFileSystemImpl.h"

namespace
{
	nBool ParseUri(nTString const& Uri, nTString& SchemaName, nTString& Path)
	{
		auto division = Uri.find(_T("://"));
		if (division == nTString::npos)
		{
			return false;
		}

		SchemaName.assign(Uri, 0, division);
		Path.assign(Uri, division + 3);
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

nBool n2dVirtualFileSystemImpl::SchemaExist(ncTStr schemaName) const
{
	return m_SchemaMap.find(schemaName) != m_SchemaMap.end();
}

natRefPointer<ISchema> n2dVirtualFileSystemImpl::GetSchemaFromName(ncTStr name)
{
	auto iter = m_SchemaMap.find(name);
	if (iter != m_SchemaMap.end())
	{
		return iter->second;
	}

	nat_Throw(natException, _T("No schema named \"{0}\"."), name);
}

natRefPointer<IStreamInfo> n2dVirtualFileSystemImpl::GetStreamInfoFromUri(ncTStr uri)
{
	nTString schemaName, path;
	if (!ParseUri(uri, schemaName, path))
	{
		nat_Throw(natException, _T("Bad uri."));
	}

	auto iter = m_SchemaMap.find(schemaName);
	if (iter != m_SchemaMap.end())
	{
		return iter->second->GetStreamInfoFromPath(path.c_str());
	}

	nat_Throw(natException, _T("No schema named \"{0}\"."), schemaName);
}
