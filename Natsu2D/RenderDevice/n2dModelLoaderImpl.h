#pragma once
#include "..\n2dModel.h"
#include <vector>
#include <natVec.h>

class n2dModelDataImpl;
class n2dMeshDataImpl;
struct aiScene;
struct aiNode;

class n2dModelLoaderImpl
	: public natRefObjImpl<n2dModelLoader>
{
public:
	n2dModelLoaderImpl();

	nResult LoadFromStream(natStream* pStream) override;
	nResult LoadFromFile(ncTStr lpPath) override;

	void SetDefaultTexture(n2dTexture2D* Texture) override;

	n2dModelData* GetModel() override;

	nuInt GetVertexBuffer() override;
	nuInt GetUVBuffer() override;
	nuInt GetNormalBuffer() override;
	nuInt GetIndexBuffer() override;

	nuInt GetVertexCount() const override;
	nuInt GetIndexCount() const override;
private:
	void loadMeshData(const aiScene* pScene, const aiNode* pNode, nFloat fScale);

	std::vector<natVec3<>> m_Vertices;
	std::vector<natVec2<>> m_UVs;
	std::vector<natVec3<>> m_Normals;
	std::vector<nuShort> m_ElementIndexes;

	nuInt m_VertexBuffer, m_UVBuffer, m_NormalBuffer, m_ElementBuffer;

	n2dModelDataImpl* m_Model;
	natRefPointer<n2dTexture2D> m_DefaultTexture;
};