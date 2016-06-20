#pragma once
#include "..\n2dFileSystem.h"
#include <unordered_map>

class n2dFileNodeImpl
	: public natRefObjImpl<n2dFileNode>
{
	friend class n2dFileSystemImpl;
public:
	n2dFileNodeImpl();
	n2dFileNodeImpl(n2dFileNodeImpl* pParentNode, natStream* pStream, nTString const& FileName, nBool bArchive, nBool bReadOnly, time_t ModifiedTime, size_t CompressedSize, size_t UncompressedSize);
	~n2dFileNodeImpl();

	n2dFileNodeImpl(n2dFileNodeImpl const&) = delete;

	n2dFileNodeImpl& operator=(n2dFileNodeImpl const&) = delete;

	ncTStr GetName() const override;
	nBool IsArchive() const override;
	nBool IsReadOnly() const override;
	time_t GetModifiedTime() const override;
	size_t GetCompressedSize() const override;
	size_t GetUncompressedSize() const override;

	n2dFileNode* GetParentNode() const override;

	nResult Create(nTString const& Filename, nBool bArchive, n2dFileNode** pFileNode) override;
	nResult Open(nTString const& Filename, n2dFileNode** pFileNode) override;
	nResult Remove(nTString const& Filename) override;
	nBool Exist(nTString const& Filename) const override;

	nResult Associate(natStream* pStream) override;

	nuInt GetChildNodeCount() const override;
	nResult List(nBool Recursive, nBool Dirs, std::function<nBool(n2dFileNode*)> Callback) const override;

	natRefPointer<natStream> OpenStream() override;
	nResult Close() override;

	nResult rawCreate(nTString const& Filename, n2dFileNodeImpl** pFileNode);
	nResult doOpen(nTString const& Filename, n2dFileNodeImpl** pFileNode);
private:
	nTString m_FileName;
	nBool m_IsArchive;
	nBool m_ReadOnly;
	time_t m_ModifiedTime;
	size_t m_CompressedSize;
	size_t m_UncompressedSize;

	n2dFileNodeImpl* m_ParentNode;
	std::unordered_map<nTString, natRefPointer<n2dFileNodeImpl>> m_ChildNodes;
	natRefPointer<natStream> m_pStream;
};

class n2dFileSystemImpl
	: public natRefObjImpl<n2dFileSystem>
{
public:
	n2dFileSystemImpl();
	~n2dFileSystemImpl();

	n2dFileNode* GetRootNode() const override;
	n2dFileNode* GetNode(nTString const& Path) override;
	nResult RemoveNode(nTString const& Path) override;

	nResult AddNode(nTString const& Path, n2dFileNode** pNode) override;
	nResult AddNode(nTString const& Path, natStream* pStream, n2dFileNode** pNode) override;
private:
	natRefPointer<n2dFileNodeImpl> m_pRootNode;
};