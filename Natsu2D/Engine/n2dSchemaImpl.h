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
		LocalFilesystemStreamInfo(LocalFilesystemSchema* pSchema, nStrView path);
		~LocalFilesystemStreamInfo();

		nStrView GetPath() const override;
		nResult GetEditTime(std::chrono::system_clock::time_point& time) const override;
		nResult GetSize(nLen& size) const override;
		natRefPointer<natStream> OpenStream(nBool Readable, nBool Writable) override;

	private:
		LocalFilesystemSchema* m_pSchema;
		nString m_Path;
	};

	static const nString DefaultRootPath;

	explicit LocalFilesystemSchema(nStrView schemaName, nStrView rootPath = DefaultRootPath);
	~LocalFilesystemSchema();

	nStrView GetName() const override;
	natRefPointer<IStreamInfo> GetStreamInfoFromPath(nStrView path) override;
	nBool PathExist(nStrView path) const override;
	nResult RemoveFromPath(nStrView path) override;
	nResult EnumPath(nStrView path, nBool recursive, nBool includeFolder, std::function<nBool(IStreamInfo*)> enumCallback) override;

	nStrView GetRootPath() const noexcept;

private:
	nString getRealPath(nStrView path) const;
	nResult enumPathImpl(std::experimental::filesystem::path const& path, nBool recursive, nBool includeFolder, std::function<nBool(IStreamInfo*)> enumCallback);

	nString m_SchemaName;
	nString m_RootPath;
};

class n2dSchemaFactoryImpl
	: public natRefObjImpl<n2dSchemaFactory>
{
public:
	natRefPointer<ISchema> CreateLocalFilesystemSchema(nStrView schemaName) const override;
	natRefPointer<ISchema> CreateLocalFilesystemSchema(nStrView schemaName, nStrView rootPath) const override;
};
