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
	nBool SchemaExist(nStrView schemaName) const override;
	natRefPointer<ISchema> GetSchemaFromName(nStrView name) override;
	natRefPointer<IStreamInfo> GetStreamInfoFromUri(nStrView uri) override;

private:
	std::unordered_map<nString, natRefPointer<ISchema>> m_SchemaMap;
};
