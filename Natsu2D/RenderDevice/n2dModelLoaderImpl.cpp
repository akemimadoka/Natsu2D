#include "n2dModelLoaderImpl.h"
#include <natStream.h>
#include <natUtil.h>
#include "natLog.h"
#include "n2dVBOHelper.h"
#include "..\n2dCommon.h"
#include "..\include\assimp\Importer.hpp"
#include "..\include\assimp\scene.h"
#include "..\include\assimp\postprocess.h"

n2dModelLoaderImpl::n2dModelLoaderImpl()
	: m_VertexBuffer(0u),
	m_UVBuffer(0u),
	m_NormalBuffer(0u),
	m_ElementBuffer(0u)
{
}

nResult n2dModelLoaderImpl::LoadFromStream(natStream* pStream)
{
	Assimp::Importer tImporter;
	nLen tLen = pStream->GetSize() - pStream->GetPosition();
	nData tBuf = new nByte[static_cast<nuInt>(tLen)];
	
	if (pStream->ReadBytes(tBuf, tLen) != tLen || pStream->GetLastErr() != NatErr_OK)
	{
		SafeDelArr(tBuf);
		return NatErr_InternalErr;
	}

	const aiScene* tpScene = tImporter.ReadFileFromMemory(tBuf, static_cast<size_t>(tLen), aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	if (!tpScene)
	{
		natLog::GetInstance().LogErr(natUtil::C2Wstr(tImporter.GetErrorString()));
		return NatErr_InternalErr;
	}

	for (nuInt i = 0u; i < tpScene->mNumMeshes; ++i)
	{
		for (nuInt iv = 0; iv < tpScene->mMeshes[i]->mNumVertices; ++iv)
		{
			m_Vertices.push_back(natVec3<>(tpScene->mMeshes[i]->mVertices[iv].x, tpScene->mMeshes[i]->mVertices[iv].y, tpScene->mMeshes[i]->mVertices[iv].z));
			m_UVs.push_back(natVec2<>(tpScene->mMeshes[i]->mTextureCoords[0][iv].x, tpScene->mMeshes[i]->mTextureCoords[0][iv].y));
			m_Normals.push_back(natVec3<>(tpScene->mMeshes[i]->mNormals[iv].x, tpScene->mMeshes[i]->mNormals[iv].y, tpScene->mMeshes[i]->mNormals[iv].z));
		}
	}

	n2dVBOHelper::indexVBO(m_Vertices, m_UVs, m_Normals, m_ElementIndexes);

	return NatErr_OK;
}

nResult n2dModelLoaderImpl::LoadFromFile(ncTStr lpPath)
{
	natStream* tpStream = new natFileStream(lpPath, false);
	nResult tRet = LoadFromStream(tpStream);
	SafeRelease(tpStream);
	return tRet;
}

nuInt n2dModelLoaderImpl::GetVertexBuffer()
{
	if (m_VertexBuffer == 0u)
	{
		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(natVec3<>), &m_Vertices[0], GL_STATIC_DRAW);
	}

	return m_VertexBuffer;
}

nuInt n2dModelLoaderImpl::GetUVBuffer()
{
	if (m_UVBuffer == 0u)
	{
		glGenBuffers(1, &m_UVBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_UVBuffer);
		glBufferData(GL_ARRAY_BUFFER, m_UVs.size() * sizeof(natVec2<>), &m_UVs[0], GL_STATIC_DRAW);
	}

	return m_UVBuffer;
}

nuInt n2dModelLoaderImpl::GetNormalBuffer()
{
	if (m_NormalBuffer == 0u)
	{
		glGenBuffers(1, &m_NormalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, m_Normals.size() * sizeof(natVec3<>), &m_Normals[0], GL_STATIC_DRAW);
	}

	return m_NormalBuffer;
}

nuInt n2dModelLoaderImpl::GetIndexBuffer()
{
	if (m_ElementBuffer == 0u)
	{
		if (m_VertexBuffer != 0u)
		{
			glDeleteBuffers(1, &m_VertexBuffer);
			m_VertexBuffer = 0u;
		}

		if (m_UVBuffer != 0u)
		{
			glDeleteBuffers(1, &m_UVBuffer);
			m_UVBuffer = 0u;
		}

		if (m_NormalBuffer != 0u)
		{
			glDeleteBuffers(1, &m_NormalBuffer);
			m_NormalBuffer = 0u;
		}

		n2dVBOHelper::indexVBO(m_Vertices, m_UVs, m_Normals, m_ElementIndexes);

		GetVertexBuffer();
		GetUVBuffer();
		GetNormalBuffer();

		glGenBuffers(1, &m_ElementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_ElementIndexes.size() * sizeof(nuShort), &m_ElementIndexes[0], GL_STATIC_DRAW);
	}

	return m_ElementBuffer;
}

nuInt n2dModelLoaderImpl::GetVertexCount() const
{
	return m_Vertices.size();
}

nuInt n2dModelLoaderImpl::GetUVCount() const
{
	return m_UVs.size();
}

nuInt n2dModelLoaderImpl::GetNormalCount() const
{
	return m_Normals.size();
}

nuInt n2dModelLoaderImpl::GetIndexCount() const
{
	return m_ElementIndexes.size();
}