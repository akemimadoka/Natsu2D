#include "n2dMeshDataImpl.h"
#include "OpenGL.h"
#include <natStream.h>

n2dMeshDataImpl::~n2dMeshDataImpl()
{
}

void n2dMeshDataImpl::ApplyMaterial(Material const& MeshMaterial)
{
	m_Material = MeshMaterial;
}

nuInt n2dMeshDataImpl::GetVertexBuffer()
{
	if (!m_VB)
	{
		glGenBuffers(1, &m_VB);
		glBindBuffer(GL_ARRAY_BUFFER, m_VB);
		glBufferData(GL_ARRAY_BUFFER, sizeof(n2dGraphics3DVertex) * m_Vert.size(), &m_Vert[0], GL_STATIC_DRAW);
		m_Vert.clear();
	}

	return m_VB;
}

nuInt n2dMeshDataImpl::GetIndexBuffer()
{
	if (!m_IB)
	{
		glGenBuffers(1, &m_IB);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(nuShort) * m_Index.size(), &m_Index[0], GL_STATIC_DRAW);
		m_Index.clear();
	}

	return m_IB;
}

nuInt n2dMeshDataImpl::GetVertexCount() const
{
	return m_VertCount;
}

nuInt n2dMeshDataImpl::GetIndexCount() const
{
	return m_IndexCount;
}

n2dMeshDataImpl::n2dMeshDataImpl()
	: m_VB(0u),
	m_IB(0u)
{
	memset(&m_Material, 0, sizeof(m_Material));
}