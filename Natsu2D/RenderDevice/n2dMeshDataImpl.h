#pragma once
#include "..\n2dCommon.h"
#include "..\n2dModel.h"
#include "..\n2dGraphics.h"
#include "..\n2dTexture.h"
#include <vector>
#include <natMat.h>

class n2dModelLoaderImpl;

class n2dMeshDataImpl
	: public natRefObjImpl<n2dMeshData>
{
	friend class n2dGraphics3DImpl;
	friend class n2dModelLoaderImpl;

public:
	~n2dMeshDataImpl();
	
	void ApplyMaterial(Material const& MeshMaterial);

	nuInt GetVertexBuffer() override;
	nuInt GetIndexBuffer() override;

	nuInt GetVertexCount() const override;
	nuInt GetIndexCount() const override;

private:
	n2dMeshDataImpl();

	Material m_Material;
	natMat4<> m_Transform;

	nuInt m_VB, m_IB;

	nuInt m_VertCount, m_IndexCount;

	/*std::vector<natVec3<>> m_Vertex;
	std::vector<natVec2<>> m_UV;
	std::vector<natVec3<>> m_Normal;*/

	std::vector<n2dGraphics3DVertex> m_Vert;

	std::vector<nuShort> m_Index;
};

