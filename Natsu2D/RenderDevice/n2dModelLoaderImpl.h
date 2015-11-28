#pragma once
#include "..\n2dModelLoader.h"
#include <vector>
#include <natMath.h>

class n2dModelLoaderImpl
	: public natRefObjImpl<n2dModelLoader>
{
public:
	n2dModelLoaderImpl();

	nResult LoadFromStream(natStream* pStream) override;
	nResult LoadFromFile(ncTStr lpPath) override;

	nuInt GetVertexBuffer() override;
	nuInt GetUVBuffer() override;
	nuInt GetNormalBuffer() override;
	nuInt GetIndexBuffer() override;

	nuInt GetVertexCount() const override;
	nuInt GetUVCount() const override;
	nuInt GetNormalCount() const override;
	nuInt GetIndexCount() const override;
private:
	std::vector<natVec3<>> m_Vertices;
	std::vector<natVec2<>> m_UVs;
	std::vector<natVec3<>> m_Normals;
	std::vector<nuShort> m_ElementIndexes;

	nuInt m_VertexBuffer, m_UVBuffer, m_NormalBuffer, m_ElementBuffer;
};