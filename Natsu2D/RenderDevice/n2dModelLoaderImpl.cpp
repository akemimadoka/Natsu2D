#include "n2dModelLoaderImpl.h"
#include <natStream.h>
#include <natUtil.h>
#include "natLog.h"
#include "n2dMeshDataImpl.h"
#include "n2dTextureImpl.h"
#include "n2dModelImpl.h"
#include "..\n2dCommon.h"
#include "OpenGL.h"
#include "..\include\assimp\Importer.hpp"
#include "..\include\assimp\scene.h"
#include "..\include\assimp\postprocess.h"
#include <natMath.h>

n2dModelLoaderImpl::n2dModelLoaderImpl()
	: m_VertexBuffer(0u),
	m_UVBuffer(0u),
	m_NormalBuffer(0u),
	m_ElementBuffer(0u),
	m_Model(nullptr),
	m_DefaultTexture(nullptr)
{
	//m_Model = new n2dModelDataImpl;
}

nResult n2dModelLoaderImpl::LoadFromStream(natStream* pStream)
{
	SafeRelease(m_Model);
	m_Model = new n2dModelDataImpl;

	Assimp::Importer tImporter;
	nLen tLen = pStream->GetSize() - pStream->GetPosition();
	nData tBuf = new nByte[static_cast<nuInt>(tLen)];
	
	if (pStream->ReadBytes(tBuf, tLen) != tLen || pStream->GetLastErr() != NatErr_OK)
	{
		SafeDelArr(tBuf);
		return NatErr_InternalErr;
	}

	const aiScene* tpScene = tImporter.ReadFileFromMemory(tBuf, static_cast<size_t>(tLen), aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	SafeDelArr(tBuf);

	if (!tpScene)
	{
		natLog::GetInstance().LogErr(natUtil::C2Wstr(tImporter.GetErrorString()));
		return NatErr_InternalErr;
	}

	loadMeshData(tpScene, tpScene->mRootNode, 1.0f);

	/*for (nuInt i = 0u; i < tpScene->mNumMeshes; ++i)
	{
		std::vector<natVec3<>> tVert;
		std::vector<natVec2<>> tUV;
		std::vector<natVec3<>> tNormal;
		std::vector<nShort> tIndex;

		for (nuInt iv = 0; iv < tpScene->mMeshes[i]->mNumVertices; ++iv)
		{
			tVert.emplace_back(natVec3<>(&tpScene->mMeshes[i]->mVertices[iv][0]));
			tUV.emplace_back(natVec2<>(&tpScene->mMeshes[i]->mTextureCoords[0][iv][0]));
			tNormal.emplace_back(natVec3<>(&tpScene->mMeshes[i]->mNormals[iv][0]));
		}

		//pMesh->AddVert(&tVert[0], &tUV[0], &tNormal[0], tVert.size());

		//loadMeshData(pMesh, tpScene, tpScene->mRootNode, 1.0f);

		// unused
		for (nuInt ii = 0u; ii < tpScene->mMeshes[i]->mNumFaces; ++ii)
		{
			m_ElementIndexes.push_back(tpScene->mMeshes[i]->mFaces[ii].mIndices[0]);
			m_ElementIndexes.push_back(tpScene->mMeshes[i]->mFaces[ii].mIndices[1]);
			m_ElementIndexes.push_back(tpScene->mMeshes[i]->mFaces[ii].mIndices[2]);
		}
	}*/

	//m_MeshData->AddVert(&m_Vertices[0], &m_UVs[0], &m_Normals[0], m_Vertices.size());

	return NatErr_OK;
}

nResult n2dModelLoaderImpl::LoadFromFile(ncTStr lpPath)
{
	natStream* tpStream = new natFileStream(lpPath, true, false);
	nResult tRet = LoadFromStream(tpStream);
	SafeRelease(tpStream);
	return tRet;
}

void n2dModelLoaderImpl::SetDefaultTexture(n2dTexture2D* Texture)
{
	m_DefaultTexture = Texture;
}

n2dModelData* n2dModelLoaderImpl::GetModel()
{
	return m_Model;
}

nuInt n2dModelLoaderImpl::GetVertexBuffer()
{
	if (m_VertexBuffer == 0u)
	{
		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(natVec3<>), m_Vertices.data(), GL_STATIC_DRAW);
	}

	return m_VertexBuffer;
}

nuInt n2dModelLoaderImpl::GetUVBuffer()
{
	if (m_UVBuffer == 0u)
	{
		glGenBuffers(1, &m_UVBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_UVBuffer);
		glBufferData(GL_ARRAY_BUFFER, m_UVs.size() * sizeof(natVec2<>), m_UVs.data(), GL_STATIC_DRAW);
	}

	return m_UVBuffer;
}

nuInt n2dModelLoaderImpl::GetNormalBuffer()
{
	if (m_NormalBuffer == 0u)
	{
		glGenBuffers(1, &m_NormalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, m_Normals.size() * sizeof(natVec3<>), m_Normals.data(), GL_STATIC_DRAW);
	}

	return m_NormalBuffer;
}

nuInt n2dModelLoaderImpl::GetIndexBuffer()
{
	if (m_ElementBuffer == 0u)
	{
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

nuInt n2dModelLoaderImpl::GetIndexCount() const
{
	return m_ElementIndexes.size();
}

void n2dModelLoaderImpl::loadMeshData(const aiScene* pScene, const aiNode* pNode, nFloat fScale)
{
	natMat4<> m(pNode->mTransformation[0]);
	m = natTransform::scale(m, natVec3<>(fScale));

	for (nuInt i = 0u; i < pNode->mNumMeshes; ++i)
	{
		const aiMesh* tMesh = pScene->mMeshes[pNode->mMeshes[i]];

		aiMaterial* pMaterial = pScene->mMaterials[tMesh->mMaterialIndex];
		
		n2dMeshDataImpl::Material tMaterial;
		memset(&tMaterial, 0, sizeof(tMaterial));

		aiString tTexPath;
		tMaterial.Texture = new n2dTexture2DImpl;
		if (AI_SUCCESS != pMaterial->GetTexture(aiTextureType_DIFFUSE, 0u, &tTexPath) || !tMaterial.Texture->LoadTexture(natUtil::C2Wstr(tTexPath.C_Str())))
		{
			tMaterial.Texture = m_DefaultTexture;
		}

		aiColor4D tColor;
		memset(&tColor, 0, sizeof(aiColor4D));
		if (AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &tColor))
		{
			tMaterial.Diffuse = natVec4<>(&tColor[0]);
		}

		memset(&tColor, 0, sizeof(aiColor4D));
		if (AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_SPECULAR, &tColor))
		{
			tMaterial.Specular = natVec4<>(&tColor[0]);
		}

		memset(&tColor, 0, sizeof(aiColor4D));
		if (AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_AMBIENT, &tColor))
		{
			tMaterial.Ambient = natVec4<>(&tColor[0]);
		}

		memset(&tColor, 0, sizeof(aiColor4D));
		if (AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_EMISSIVE, &tColor))
		{
			tMaterial.Emission = natVec4<>(&tColor[0]);
		}

		nuInt max = 1u;
		aiGetMaterialFloatArray(pMaterial, AI_MATKEY_SHININESS, &tMaterial.Shininess, &max);
		max = 1u;
		aiGetMaterialFloatArray(pMaterial, AI_MATKEY_SHININESS_STRENGTH, &tMaterial.Strength, &max);

		max = 1u;
		nInt tmp = 0;
		if (AI_SUCCESS == aiGetMaterialIntegerArray(pMaterial, AI_MATKEY_ENABLE_WIREFRAME, &tmp, &max) && tmp != 0)
		{
			tMaterial.WireFrame = true;
		}
		else
		{
			tMaterial.WireFrame = false;
		}

		max = 1u;
		if (AI_SUCCESS == aiGetMaterialIntegerArray(pMaterial, AI_MATKEY_TWOSIDED, &tmp, &max) && tmp != 0)
		{
			tMaterial.Both_sided = true;
		}
		else
		{
			tMaterial.Both_sided = false;
		}

		const aiFace* pFace = tMesh->mFaces;
		if (pFace->mNumIndices != 3u)
		{
			throw natException(_T("n2dModelLoaderImpl::loadMeshData"), _T("Not Supported model"));
		}

		n2dMeshDataImpl* pMesh = new n2dMeshDataImpl;
		m_Model->m_Meshes.push_back(pMesh);

		pMesh->m_VertCount = tMesh->mNumVertices;
		pMesh->m_Vert.resize(tMesh->mNumVertices);
		/*pMesh->m_Vertex.reserve(tMesh->mNumVertices);
		pMesh->m_UV.reserve(tMesh->mNumVertices);
		pMesh->m_Normal.reserve(tMesh->mNumVertices);*/
		for (nuInt iv = 0u; iv < tMesh->mNumVertices; ++iv)
		{
			n2dGraphics3DVertex& tVert = pMesh->m_Vert[iv];
			tVert.vert = natVec3<>(&tMesh->mVertices[iv][0]);
			tVert.uv = natVec2<>(&tMesh->mTextureCoords[0][iv][0]);
			tVert.normal = natVec3<>(&tMesh->mNormals[iv][0]);
			
			/*pMesh->m_Vertex.emplace_back(natVec3<>(&tMesh->mVertices[iv][0]));
			pMesh->m_UV.emplace_back(natVec2<>(&tMesh->mTextureCoords[0][iv][0]));
			pMesh->m_Normal.emplace_back(natVec3<>(&tMesh->mNormals[iv][0]));*/
		}

		pMesh->ApplyMaterial(tMaterial);
		pMesh->m_IndexCount = tMesh->mNumFaces * 3u;
		pMesh->m_Index.clear();
		pMesh->m_Index.reserve(tMesh->mNumFaces * 3u);

		std::vector<nuShort>& tmpIndex = pMesh->m_Index;

		SafeRelease(pMesh);

		std::for_each(pFace, pFace + tMesh->mNumFaces, [&tmpIndex] (const aiFace& tFace)
		{
			tmpIndex.insert(tmpIndex.end(), tFace.mIndices, tFace.mIndices + 3);
		});
	}

	for (nuInt i = 0u; i < pNode->mNumChildren; ++i)
	{
		loadMeshData(pScene, pNode->mChildren[i], fScale);
	}
}