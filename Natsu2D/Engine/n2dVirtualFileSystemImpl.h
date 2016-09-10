#pragma once
#include "../n2dVirtualFileSystem.h"
#include <unordered_map>

class n2dVirtualFileSystemImpl
	: public natRefObjImpl<n2dVirtualFileSystem>
{
public:
	n2dVirtualFileSystemImpl();
	~n2dVirtualFileSystemImpl();

	nResult RegisterSchema(ISchema* pSchema) override;
	nBool SchemaExist(ncTStr schemaName) const override;
	natRefPointer<ISchema> GetSchemaFromName(ncTStr name) override;
	natRefPointer<IStreamInfo> GetStreamInfoFromUri(ncTStr uri) override;

private:
	std::unordered_map<nTString, natRefPointer<ISchema>> m_SchemaMap;
};
