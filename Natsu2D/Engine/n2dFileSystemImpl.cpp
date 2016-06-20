#include "n2dFileSystemImpl.h"
#include <natStream.h>
#include <natUtil.h>
#include <ctime>

n2dFileNodeImpl::n2dFileNodeImpl()
	: m_IsArchive(false),
	m_ReadOnly(false),
	m_ModifiedTime(0),
	m_CompressedSize(0),
	m_UncompressedSize(0),
	m_ParentNode(nullptr)
{
}

n2dFileNodeImpl::n2dFileNodeImpl(n2dFileNodeImpl* pParentNode, natStream* pStream, nTString const& FileName, nBool bArchive, nBool bReadOnly, time_t ModifiedTime, size_t CompressedSize, size_t UncompressedSize)
	: m_FileName(FileName),
	m_IsArchive(bArchive),
	m_ReadOnly(bReadOnly),
	m_ModifiedTime(ModifiedTime),
	m_CompressedSize(CompressedSize),
	m_UncompressedSize(UncompressedSize),
	m_ParentNode(pParentNode),
	m_pStream(pStream)
{
}

n2dFileNodeImpl::~n2dFileNodeImpl()
{
}

ncTStr n2dFileNodeImpl::GetName() const
{
	return m_FileName.c_str();
}

nBool n2dFileNodeImpl::IsArchive() const
{
	return m_IsArchive;
}

nBool n2dFileNodeImpl::IsReadOnly() const
{
	return m_ReadOnly;
}

time_t n2dFileNodeImpl::GetModifiedTime() const
{
	return m_ModifiedTime;
}

size_t n2dFileNodeImpl::GetCompressedSize() const
{
	return m_CompressedSize;
}

size_t n2dFileNodeImpl::GetUncompressedSize() const
{
	return m_UncompressedSize;
}

n2dFileNode* n2dFileNodeImpl::GetParentNode() const
{
	return m_ParentNode;
}

nResult n2dFileNodeImpl::Create(nTString const& Filename, nBool bArchive, n2dFileNode** pFileNode)
{
	n2dFileNodeImpl* pNode;
	auto RetCode = rawCreate(Filename, &pNode);
	pNode->m_IsArchive = bArchive;
	*pFileNode = pNode;

	return RetCode;
}

nResult n2dFileNodeImpl::Open(nTString const& Filename, n2dFileNode** pFileNode)
{
	n2dFileNodeImpl* pNode;
	auto RetCode = doOpen(Filename, &pNode);
	*pFileNode = pNode;
	return RetCode;
}

nResult n2dFileNodeImpl::Remove(nTString const& Filename)
{
	return m_ChildNodes.erase(Filename) == 0u ? NatErr_InternalErr : NatErr_OK;
}

nBool n2dFileNodeImpl::Exist(nTString const & Filename) const
{
	return m_ChildNodes.find(Filename) != m_ChildNodes.end();
}

nResult n2dFileNodeImpl::Associate(natStream* pStream)
{
	if (m_IsArchive)
	{
		return NatErr_IllegalState;
	}

	m_pStream = natRefPointer<natStream>(pStream);
	m_CompressedSize = m_UncompressedSize = static_cast<size_t>(pStream->GetSize());
	m_ReadOnly = pStream->CanWrite();

	return NatErr_OK;
}

nuInt n2dFileNodeImpl::GetChildNodeCount() const
{
	return m_ChildNodes.size();
}

nResult n2dFileNodeImpl::List(nBool Recursive, nBool Dirs, std::function<nBool(n2dFileNode*)> Callback) const
{
	for (auto const& i : m_ChildNodes)
	{
		if (!(Dirs ^ i.second->m_IsArchive))
		{
			if (Callback(i.second))
			{
				return NatErr_Interrupted;
			}
		}

		if (Recursive && i.second->m_IsArchive)
		{
			nResult ret;
			if ((ret = i.second->List(true, Dirs, Callback)) != NatErr_OK)
			{
				return ret;
			}
		}
	}

	return NatErr_OK;
}

natRefPointer<natStream> n2dFileNodeImpl::OpenStream()
{
	return m_pStream;
}

nResult n2dFileNodeImpl::Close()
{
	m_pStream = nullptr;
	return NatErr_OK;
}

nResult n2dFileNodeImpl::rawCreate(nTString const& Filename, n2dFileNodeImpl** pFileNode)
{
	natRefPointer<n2dFileNodeImpl> pNode;

	try
	{
		pNode = make_ref<n2dFileNodeImpl>();
	}
	catch (...)
	{
		return NatErr_InternalErr;
	}
	
	pNode->m_ParentNode = this;
	pNode->m_ModifiedTime = time(nullptr);

	m_ChildNodes[Filename] = pNode;
	*pFileNode = pNode;

	return NatErr_OK;
}

nResult n2dFileNodeImpl::doOpen(nTString const& Filename, n2dFileNodeImpl** pFileNode)
{
	auto Itea = m_ChildNodes.find(Filename);

	if (Itea == m_ChildNodes.end())
	{
		return NatErr_InternalErr;
	}

	*pFileNode = Itea->second;

	return NatErr_OK;
}

n2dFileSystemImpl::n2dFileSystemImpl()
{
	m_pRootNode = make_ref<n2dFileNodeImpl>();
	m_pRootNode->m_IsArchive = true;
}

n2dFileSystemImpl::~n2dFileSystemImpl()
{
}

n2dFileNode* n2dFileSystemImpl::GetRootNode() const
{
	return m_pRootNode;
}

n2dFileNode* n2dFileSystemImpl::GetNode(nTString const& Path)
{
	std::vector<nTString> SplitResult;
	natUtil::split(Path, nTString(_T("/")), SplitResult);

	auto Itea = SplitResult.begin();
	if (!Itea->empty())
	{
		return nullptr;
	}
	++Itea;

	n2dFileNodeImpl* pNode = m_pRootNode;
	for (; Itea != SplitResult.end(); ++Itea)
	{
		if (NATFAIL(pNode->doOpen(*Itea, &pNode)))
		{
			return nullptr;
		}
	}

	return pNode;
}

nResult n2dFileSystemImpl::RemoveNode(nTString const& Path)
{
	n2dFileNode *pNode, *pParentNode;

	if ((pNode = GetNode(Path)) == nullptr)
	{
		return NatErr_InternalErr;
	}

	pParentNode = pNode->GetParentNode();
	if (pParentNode == nullptr)
	{
		m_pRootNode = make_ref<n2dFileNodeImpl>();
		m_pRootNode->m_IsArchive = true;

		return NatErr_OK;
	}

	return pParentNode->Remove(pNode->GetName());
}

nResult n2dFileSystemImpl::AddNode(nTString const& Path, n2dFileNode** pNodeOutput)
{
	std::vector<nTString> SplitResult;
	natUtil::split(Path, nTString(_T("/")), SplitResult);

	auto Itea = SplitResult.begin();
	if (!Itea->empty())
	{
		return NatErr_InvalidArg;
	}
	++Itea;

	n2dFileNodeImpl* pNode = m_pRootNode;
	for (; Itea != SplitResult.end(); ++Itea)
	{
		if (NATFAIL(pNode->doOpen(*Itea, &pNode)))
		{
			pNode->rawCreate(*Itea, &pNode);
			pNode->m_IsArchive = true;
		}
	}

	*pNodeOutput = pNode;
	return NatErr_OK;
}

nResult n2dFileSystemImpl::AddNode(nTString const& Path, natStream* pStream, n2dFileNode** pNodeOutput)
{
	std::vector<nTString> SplitResult;
	natUtil::split(Path, nTString(_T("/")), SplitResult);

	auto Itea = SplitResult.begin();
	if (!Itea->empty())
	{
		return NatErr_InvalidArg;
	}
	++Itea;

	n2dFileNodeImpl* pNode = m_pRootNode;
	for (; Itea != SplitResult.end(); ++Itea)
	{
		if (NATFAIL(pNode->doOpen(*Itea, &pNode)))
		{
			if (Itea == SplitResult.end() - 1)
			{
				pNode->rawCreate(*Itea, &pNode);
				pNode->m_IsArchive = false;

				pNode->Associate(pStream);
			}
			else
			{
				pNode->rawCreate(*Itea, &pNode);
				pNode->m_IsArchive = true;
			}
		}
	}

	*pNodeOutput = pNode;
	return NatErr_OK;
}
