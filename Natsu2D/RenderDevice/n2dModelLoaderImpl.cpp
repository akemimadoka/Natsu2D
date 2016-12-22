#include "n2dModelLoaderImpl.h"
#include "../n2dEngine.h"
#include <natStream.h>
#include <natUtil.h>
#include "natLog.h"
#include "n2dMeshDataImpl.h"
#include "n2dTextureImpl.h"
#include "n2dModelImpl.h"
#include "n2dMotionManagerImpl.h"
#include "../include/assimp/Importer.hpp"
#include "../include/assimp/scene.h"
#include "../include/assimp/postprocess.h"

n2dModelLoaderImpl::n2dModelLoaderImpl(n2dRenderDeviceImpl* pRenderDevice)
	: m_DefaultTexture(nullptr),
	m_pRenderDevice(pRenderDevice)
{
	m_DefaultTexture = make_ref<n2dTexture2DImpl>();
}

nResult n2dModelLoaderImpl::CreateStaticModelFromStream(natStream* pStream, n2dModelData** pOut)
{
	if (!pStream || !pOut)
	{
		return NatErr_InvalidArg;
	}

	n2dStaticModelDataImpl* pModel = nullptr;

	try
	{
		pModel = new n2dStaticModelDataImpl;
	}
	catch (...)
	{
		return NatErr_InternalErr;
	}

	Assimp::Importer tImporter;
	nLen tLen = pStream->GetSize() - pStream->GetPosition();
	nData tBuf = new nByte[static_cast<nuInt>(tLen)];
	
	if (pStream->ReadBytes(tBuf, tLen) != tLen)
	{
		SafeDelArr(tBuf);
		return NatErr_InternalErr;
	}

	const aiScene* tpScene = tImporter.ReadFileFromMemory(tBuf, static_cast<size_t>(tLen), aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	SafeDelArr(tBuf);

	if (!tpScene)
	{
		m_pRenderDevice->GetEngine()->GetLogger().LogErr(nString{ AnsiStringView(tImporter.GetErrorString()) });
		return NatErr_InternalErr;
	}

	loadMeshData(pModel, tpScene, tpScene->mRootNode, 1.0f);

	for (auto& mesh : pModel->m_Meshes)
	{
		mesh->m_pRenderDevice = m_pRenderDevice;
	}

	*pOut = pModel;
	return NatErr_OK;
}

nResult n2dModelLoaderImpl::CreateStaticModelFromFile(nStrView lpPath, n2dModelData** pOut)
{
	natStream* tpStream = new natFileStream(lpPath, true, false);
	nResult tRet = CreateStaticModelFromStream(tpStream, pOut);
	SafeRelease(tpStream);
	return tRet;
}

nResult n2dModelLoaderImpl::CreateDynamicModelFromStream(natStream * pStream, n2dModelData ** pOut)
{
	if (!pStream || !pOut)
	{
		return NatErr_InvalidArg;
	}

	n2dDynamicModelDataImpl* pModel;

	try
	{
		pModel = new n2dDynamicModelDataImpl;
	}
	catch (...)
	{
		return NatErr_InternalErr;
	}

	pModel->m_Mesh.m_pRenderDevice = m_pRenderDevice;

	nByte tBuf[257] = { 0 };

	pStream->ReadBytes(tBuf, 3ull);
	if (0 == strcmp("Pmd", reinterpret_cast<ncStr>(tBuf)))
	{
		pStream->ReadBytes(tBuf, 4ull);
		if (1.0f != *reinterpret_cast<nFloat*>(tBuf))
		{
			return NatErr_InternalErr;
		}

		// Model info start
		pStream->ReadBytes(tBuf, 20ull);
		pModel->m_Name = AnsiStringView{ reinterpret_cast<ncStr>(tBuf) };
		pStream->ReadBytes(tBuf, 256ull);
		pModel->m_Comment = AnsiStringView{ reinterpret_cast<ncStr>(tBuf) };

		// Model info end

		// Vertex start
		pStream->ReadBytes(tBuf, 4ull);
		pModel->m_Mesh.m_Vert.resize(static_cast<size_t>(*reinterpret_cast<nuInt*>(tBuf)));
		pModel->m_Mesh.m_VertAdd.resize(static_cast<size_t>(*reinterpret_cast<nuInt*>(tBuf)));

		for (nuInt i = 0u; i < static_cast<size_t>(*reinterpret_cast<nuInt*>(tBuf)); ++i)
		{
			auto& vert = pModel->m_Mesh.m_Vert[i];
			auto& vertadd = pModel->m_Mesh.m_VertAdd[i];
			pStream->ReadBytes(reinterpret_cast<nData>(&vert.vert), 12ull);
			vert.vert.z = -vert.vert.z;
			pStream->ReadBytes(reinterpret_cast<nData>(&vert.normal), 12ull);
			vert.normal.z = -vert.normal.z;
			pStream->ReadBytes(reinterpret_cast<nData>(&vert.uv), 8ull);
			pStream->ReadBytes(reinterpret_cast<nData>(&vertadd.TargetBone), 4ull);
			pStream->ReadBytes(reinterpret_cast<nData>(&vertadd.Weight), 1ull);
			pStream->ReadBytes(reinterpret_cast<nData>(&vertadd.EdgeFlag), 1ull);
		}

		pStream->ReadBytes(tBuf, 4ull);
		pModel->m_Mesh.m_Index.resize(static_cast<size_t>(*reinterpret_cast<nuInt*>(tBuf)));
		pStream->ReadBytes(reinterpret_cast<nData>(pModel->m_Mesh.m_Index.data()), static_cast<size_t>(*reinterpret_cast<nuInt*>(tBuf)) * 2ull);
		// Vertex end

		// Material start
		pStream->ReadBytes(tBuf, 4ull);
		pModel->m_Mesh.m_Materials.resize(static_cast<size_t>(*reinterpret_cast<nuInt*>(tBuf)));

		nuInt start = 0u;
		tBuf[20] = 0u;
		for (auto& mat : pModel->m_Mesh.m_Materials)
		{
			nFloat alpha = 0.0f;
			pStream->ReadBytes(reinterpret_cast<nData>(&mat.BaseMaterial.Diffuse), 12ull);
			pStream->ReadBytes(reinterpret_cast<nData>(&alpha), 4ull);
			pStream->ReadBytes(reinterpret_cast<nData>(&mat.BaseMaterial.Shininess), 4ull);
			pStream->ReadBytes(reinterpret_cast<nData>(&mat.BaseMaterial.Specular), 12ull);
			pStream->ReadBytes(reinterpret_cast<nData>(&mat.BaseMaterial.Ambient), 12ull);
			pStream->ReadBytes(reinterpret_cast<nData>(&mat.Toon), 2ull);

			mat.BaseMaterial.Ambient[3] = alpha;
			mat.BaseMaterial.Diffuse[3] = alpha;
			mat.BaseMaterial.Specular[3] = 1.0f;

			mat.BaseMaterial.Both_sided = false;
			mat.BaseMaterial.Strength = 1.0f;
			mat.BaseMaterial.WireFrame = false;
			mat.BaseMaterial.Emission = natVec4<>(0.0f, 0.0f, 0.0f, 1.0f);

			mat.Start = start;
			pStream->ReadBytes(reinterpret_cast<nData>(&mat.Length), 4ull);
			//mat.Length /= 3u;
			start += mat.Length;
			mat.End = start;

			pStream->ReadBytes(tBuf, 20ull);

			nString tStr = AnsiStringView{ reinterpret_cast<ncStr>(tBuf) };
			std::replace(tStr.begin(), tStr.end(), '/', '\\');

			std::vector<nString> SplitResult;
			tStr.GetView().Split("*"_nv, [&SplitResult](nStrView str)
			{
				SplitResult.emplace_back(std::move(str));
			});

#ifndef NDEBUG
			if (SplitResult.size() > 2)
			{
				m_pRenderDevice->GetEngine()->GetLogger().LogWarn("Texture and spa filename count dismatch, continue anyway"_nv);
			}
#endif

			mat.BaseMaterial.Texture = make_ref<n2dTexture2DImpl>();
			if (!SplitResult.empty() && SplitResult[0] != ""_nv && !mat.BaseMaterial.Texture->LoadTexture(SplitResult[0]))
			{
				m_pRenderDevice->GetEngine()->GetLogger().LogWarn("Unable to load texture file \"%s\""_nv, SplitResult[0]);
				mat.BaseMaterial.Texture = m_DefaultTexture;
			}

			if (SplitResult.size() >= 2 && SplitResult[1] != ""_nv)
			{
				mat.Spa = make_ref<n2dTexture2DImpl>();
				if (!mat.Spa->LoadTexture(SplitResult[1]))
				{
					m_pRenderDevice->GetEngine()->GetLogger().LogWarn("Unable to load spa file \"%s\""_nv, SplitResult[1]);
					mat.Spa = m_DefaultTexture;
				}
			}
		}
		// Material end

		// Bones start
		pStream->ReadBytes(tBuf, 2ull);
		pModel->m_Mesh.m_Bones.resize(*reinterpret_cast<nuShort*>(tBuf));
		pModel->m_Mesh.m_BoneMap.resize(*reinterpret_cast<nuShort*>(tBuf));

		tBuf[20] = 0u;
		for (auto& tBone : pModel->m_Mesh.m_Bones)
		{
			pStream->ReadBytes(tBuf, 20ull);
			tBone.Name = AnsiStringView{ reinterpret_cast<ncStr>(tBuf) };
			pStream->ReadBytes(reinterpret_cast<nData>(&tBone.Parent), 2ull);
			pStream->ReadBytes(reinterpret_cast<nData>(&tBone.Child), 2ull);
			pStream->ReadBytes(reinterpret_cast<nData>(&tBone.Type), 1ull);
			pStream->ReadBytes(reinterpret_cast<nData>(&tBone.Target), 2ull);
			pStream->ReadBytes(reinterpret_cast<nData>(&tBone.Pos), 12ull);
			tBone.Pos[2] = -tBone.Pos[2];
		}
		// Bones end

		// IKs start
		pStream->ReadBytes(tBuf, 2ull);
		pModel->m_Mesh.m_IKs.resize(*reinterpret_cast<nuShort*>(tBuf));
		pModel->m_Mesh.m_IK.resize(*reinterpret_cast<nuShort*>(tBuf));

		for (auto& tIK : pModel->m_Mesh.m_IKs)
		{
			pStream->ReadBytes(reinterpret_cast<nData>(&tIK.Target), 2ull);
			pStream->ReadBytes(reinterpret_cast<nData>(&tIK.Effector), 2ull);

			pStream->ReadBytes(tBuf, 1ull);
			tIK.LinkList.resize(*tBuf);

			pStream->ReadBytes(reinterpret_cast<nData>(&tIK.MaxIteration), 2ull);
			pStream->ReadBytes(reinterpret_cast<nData>(&tIK.MaxRotation), 4ull);

			pStream->ReadBytes(reinterpret_cast<nData>(tIK.LinkList.data()), 2ull * tIK.LinkList.size());
		}
		// IKs end

		// Morphes start
		pStream->ReadBytes(tBuf, 2ull);
		pModel->m_Mesh.m_Morphes.resize(*reinterpret_cast<nuShort*>(tBuf));
		pModel->m_Mesh.m_MorphMap.resize(*reinterpret_cast<nuShort*>(tBuf));

		tBuf[20] = 0u;
		for (auto& tMorph : pModel->m_Mesh.m_Morphes)
		{
			pStream->ReadBytes(tBuf, 20ull);
			tMorph.Name = AnsiStringView{ reinterpret_cast<ncStr>(tBuf) };
			pStream->ReadBytes(tBuf, 4ull);
			tMorph.Vertexes.resize(static_cast<size_t>(*reinterpret_cast<nuInt*>(tBuf)));
			tMorph.Translation.resize(static_cast<size_t>(*reinterpret_cast<nuInt*>(tBuf)));
			pStream->ReadBytes(reinterpret_cast<nData>(&tMorph.Type), 1ull);
			for (nuInt i = 0u; i < static_cast<size_t>(*reinterpret_cast<nuInt*>(tBuf)); ++i)
			{
				pStream->ReadBytes(reinterpret_cast<nData>(&tMorph.Vertexes[i]), 4ull);
				pStream->ReadBytes(reinterpret_cast<nData>(&tMorph.Translation[i]), 12ull);
				tMorph.Translation[i][2] = -tMorph.Translation[i][2];
			}
		}
		// Morphes end

		pModel->m_Mesh.m_Selekton = std::make_shared<n2dSkeleton>();
		pModel->m_Mesh.m_Selekton->CreateFromDynamicModel(pModel);
		for (nuInt i = 0u; i < pModel->m_Mesh.m_IK.size(); ++i)
		{
			pModel->m_Mesh.m_IK[i].CreateFromDynamicModel(pModel, i);
		}
		for (nuInt i = 1u; i < pModel->m_Mesh.m_Morphes.size(); ++i)
		{
			pModel->m_Mesh.m_Expression[pModel->m_Mesh.m_Morphes[i].Name] = i;
		}
	}
	else if (0 == strcmp("PMX", reinterpret_cast<ncStr>(tBuf)))
	{
		pStream->ReadBytes(tBuf, 1ull);
		if (*tBuf != 20u)
			return NatErr_InternalErr;

		// To be implemented
		return NatErr_NotImpl;
	}
	else
	{
		return NatErr_InternalErr;
	}

	*pOut = pModel;
	return NatErr_OK;
}

nResult n2dModelLoaderImpl::CreateDynamicModelFromFile(nStrView lpPath, n2dModelData ** pOut)
{
	natRefPointer<natStream> pStream = make_ref<natFileStream>(lpPath, true, false);
	nResult tRet = CreateDynamicModelFromStream(pStream, pOut);
	return tRet;
}

void n2dModelLoaderImpl::SetDefaultTexture(n2dTexture2D* Texture)
{
	if (Texture)
	{
		m_DefaultTexture = natRefPointer<n2dTexture2D>(Texture);
	}
}

void n2dModelLoaderImpl::loadMeshData(n2dStaticModelDataImpl* pModel, const aiScene* pScene, const aiNode* pNode, nFloat fScale) const
{
	natMat4<> m(pNode->mTransformation[0]);
	m = natTransform::scale(m, natVec3<>(fScale));

	for (nuInt i = 0u; i < pNode->mNumMeshes; ++i)
	{
		const aiMesh* tMesh = pScene->mMeshes[pNode->mMeshes[i]];

		aiMaterial* pMaterial = pScene->mMaterials[tMesh->mMaterialIndex];
		
		n2dStaticMeshDataImpl::Material tMaterial;
		memset(&tMaterial, 0, sizeof(tMaterial));

		aiString tTexPath;
		tMaterial.Texture = make_ref<n2dTexture2DImpl>();
		if (AI_SUCCESS != pMaterial->GetTexture(aiTextureType_DIFFUSE, 0u, &tTexPath) || !tMaterial.Texture->LoadTexture(AnsiStringView(tTexPath.C_Str())))
		{
			if (tTexPath.length > 0)
			{
				m_pRenderDevice->GetEngine()->GetLogger().LogWarn("Cannot load texture \"%s\", use default texture instead"_nv, AnsiStringView(tTexPath.data));
			}

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
		tMaterial.WireFrame = AI_SUCCESS == aiGetMaterialIntegerArray(pMaterial, AI_MATKEY_ENABLE_WIREFRAME, &tmp, &max) && tmp != 0;

		max = 1u;
		tMaterial.Both_sided = AI_SUCCESS == aiGetMaterialIntegerArray(pMaterial, AI_MATKEY_TWOSIDED, &tmp, &max) && tmp != 0;

		const aiFace* pFace = tMesh->mFaces;
		if (pFace->mNumIndices != 3u)
		{
			nat_Throw(natException, "Not Supported model"_nv);
		}

		auto pMesh = make_ref<n2dStaticMeshDataImpl>();
		pModel->m_Meshes.push_back(pMesh);

		pMesh->m_VertCount = tMesh->mNumVertices;
		pMesh->m_Vert.resize(tMesh->mNumVertices);
		for (nuInt iv = 0u; iv < tMesh->mNumVertices; ++iv)
		{
			n2dGraphics3DVertex& tVert = pMesh->m_Vert[iv];
			tVert.vert = natVec3<>(&tMesh->mVertices[iv][0]);
			tVert.uv = natVec2<>(&tMesh->mTextureCoords[0][iv][0]);
			tVert.normal = natVec3<>(&tMesh->mNormals[iv][0]);
		}

		pMesh->ApplyMaterial(tMaterial);
		pMesh->m_IndexCount = tMesh->mNumFaces * 3u;
		pMesh->m_Index.clear();
		pMesh->m_Index.reserve(tMesh->mNumFaces * 3u);

		std::vector<nuShort>& tmpIndex = pMesh->m_Index;

		std::for_each(pFace, pFace + tMesh->mNumFaces, [&tmpIndex] (const aiFace& tFace)
		{
			tmpIndex.insert(tmpIndex.end(), tFace.mIndices, tFace.mIndices + 3);
		});
	}

	for (nuInt i = 0u; i < pNode->mNumChildren; ++i)
	{
		loadMeshData(pModel, pScene, pNode->mChildren[i], fScale);
	}
}
