#pragma once
#include "n2dInterface.h"
#include "n2dCommon.h"

namespace NatsuLib
{
	struct natStream;
}

struct n2dFileNode
	: n2dInterface
{
	virtual ncTStr GetName() const = 0;
	virtual nBool IsArchive() const = 0;
	virtual nBool IsReadOnly() const = 0;

	virtual n2dFileNode* GetParentNode() const = 0;

	virtual nResult Create(nTString const& Filename, nBool bArchive, n2dFileNode** pFileNode) = 0;
	virtual nResult Open(nTString const& Filename, n2dFileNode** pFileNode) = 0;
	virtual nResult Remove(nTString const& Filename) = 0;
	virtual nBool Exist(nTString const& Filename) const = 0;

	virtual nResult Associate(natStream* pStream) = 0;

	virtual nuInt GetChildNodeCount() const = 0;
	virtual nResult List(nBool Recursive, nBool Dirs, std::function<bool(n2dFileNode*)> Callback) const = 0;

	virtual natRefPointer<natStream> OpenStream() = 0;
	virtual nResult Close() = 0;

	virtual time_t GetModifiedTime() const = 0;
	virtual size_t GetCompressedSize() const = 0;
	virtual size_t GetUncompressedSize() const = 0;
};

struct n2dFileSystem
	: n2dInterface
{
	virtual n2dFileNode* GetRootNode() const = 0;
	virtual n2dFileNode* GetNode(nTString const& Path) = 0;
	virtual nResult RemoveNode(nTString const& Path) = 0;

	virtual nResult AddNode(nTString const& Path, n2dFileNode** pNode) = 0;
	virtual nResult AddNode(nTString const& Path, natStream* pStream, n2dFileNode** pNode) = 0;
};