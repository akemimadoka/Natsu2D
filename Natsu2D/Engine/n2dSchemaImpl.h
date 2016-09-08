#pragma once
#include "../n2dVirtualFileSystem.h"
#include <experimental/filesystem>

class LocalFilesystemSchema
	: public natRefObjImpl<ISchema>
{
public:
	class LocalFilesystemStreamInfo
		: public natRefObjImpl<IStreamInfo>
	{
	public:
		LocalFilesystemStreamInfo(LocalFilesystemSchema* pSchema, ncTStr path);
		~LocalFilesystemStreamInfo();

		ncTStr GetPath() const override;
		nResult GetEditTime(std::chrono::system_clock::time_point& time) const override;
		nResult GetSize(nLen& size) const override;
		natRefPointer<natStream> OpenStream(nBool Readable, nBool Writable) override;

	private:
		LocalFilesystemSchema* m_pSchema;
		nTString m_Path;
		std::chrono::system_clock::time_point m_EditTime;
		nLen m_Size;
	};

	static constexpr nTChar DefaultRootPath[] = { _T('.'), std::experimental::filesystem::path::preferred_separator, 0 };

	explicit LocalFilesystemSchema(ncTStr schemaName, ncTStr rootPath = DefaultRootPath);
	~LocalFilesystemSchema();

	ncTStr GetName() const override;
	natRefPointer<IStreamInfo> GetStreamInfoFromPath(ncTStr path) override;

	ncTStr GetRootPath() const noexcept;

private:
	nTString m_SchemaName;
	nTString m_RootPath;
};

class n2dSchemaFactoryImpl
	: public natRefObjImpl<n2dSchemaFactory>
{
public:
	natRefPointer<ISchema> CreateLocalFilesystemSchema(ncTStr schemaName) const override;
	natRefPointer<ISchema> CreateLocalFilesystemSchema(ncTStr schemaName, ncTStr rootPath) const override;
};
