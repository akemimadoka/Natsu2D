#include "n2dMeshDataImpl.h"
#include "n2dTextureImpl.h"
#include "n2dMotionManagerImpl.h"
#include "OpenGL.h"
#include <natStream.h>

n2dStaticMeshDataImpl::~n2dStaticMeshDataImpl()
{
}

void n2dStaticMeshDataImpl::ApplyMaterial(Material const& MeshMaterial)
{
	m_Material = MeshMaterial;
}

n2dBufferImpl* n2dStaticMeshDataImpl::GetVertexBuffer()
{
	if (!m_VB)
	{
		if (!m_pRenderDevice)
		{
			return nullptr;
		}

		for (auto& v : m_Vert)
		{
			v.vert *= m_Zoom;
		}

		m_VB = new n2dBufferImpl(n2dBuffer::BufferTarget::ArrayBuffer, static_cast<n2dShaderWrapperImpl*>(m_pRenderDevice->GetShaderWrapper()));
		m_VB->AllocData(sizeof(n2dGraphics3DVertex) * m_Vert.size(), reinterpret_cast<ncData>(m_Vert.data()), n2dBuffer::BufferUsage::StaticDraw);
		m_Vert.clear();
	}

	return m_VB;
}

n2dBufferImpl* n2dStaticMeshDataImpl::GetIndexBuffer()
{
	if (!m_IB)
	{
		if (!m_pRenderDevice)
		{
			return nullptr;
		}

		m_IB = new n2dBufferImpl(n2dBuffer::BufferTarget::ElementArrayBuffer, static_cast<n2dShaderWrapperImpl*>(m_pRenderDevice->GetShaderWrapper()));
		m_IB->AllocData(sizeof(nuShort) * m_Index.size(), reinterpret_cast<ncData>(m_Index.data()), n2dBuffer::BufferUsage::StaticDraw);
		m_Index.clear();
	}

	return m_IB;
}

nuInt n2dStaticMeshDataImpl::GetVertexCount() const
{
	return m_VertCount;
}

nuInt n2dStaticMeshDataImpl::GetIndexCount() const
{
	return m_IndexCount;
}

nBool n2dStaticMeshDataImpl::IsStatic() const
{
	return true;
}

n2dStaticMeshDataImpl::n2dStaticMeshDataImpl()
	: m_VB(0u),
	m_IB(0u),
	m_Zoom(1.0f)
{
	memset(&m_Material, 0, sizeof(m_Material));
}

n2dDynamicMeshDataImpl::~n2dDynamicMeshDataImpl()
{
	SafeRelease(m_VB);
	SafeRelease(m_IB);
}

n2dBufferImpl* n2dDynamicMeshDataImpl::GetVertexBuffer()
{
	if (!m_pRenderDevice)
	{
		return 0u;
	}

	if (!m_VB)
	{
		m_VB = new n2dBufferImpl(n2dBuffer::BufferTarget::ArrayBuffer, static_cast<n2dShaderWrapperImpl*>(m_pRenderDevice->GetShaderWrapper()));

		m_VB->AllocData(sizeof(n2dGraphics3DVertex) * m_Vert.size(), reinterpret_cast<ncData>(m_Vert.data()), n2dBuffer::BufferUsage::DynamicDraw);
	}

	if (m_Updated)
	{
		static nuInt size = m_Vert.size();
		static std::vector<n2dGraphics3DVertex> tVerts(size);
		tVerts = m_Vert;
		const n2dBone* pBone;
		static natVec4<> vt;
		for (nuInt i = 0u; i < size; ++i)
		{
			n2dGraphics3DVertex& tVert = tVerts[i];
			auto const& tVertAdd = m_VertAdd[i];
			memcpy_s(&vt, sizeof(nFloat) * 3, &tVert.vert, sizeof(tVert.vert));
			vt.w = 1.0f;
			
			pBone = m_Selekton->m_Bones.data();

			natMat4<> const& m1 = pBone[tVertAdd.TargetBone[0]].GetMatrix();
			natMat4<> const& m2 = pBone[tVertAdd.TargetBone[1]].GetMatrix();

			nFloat w = static_cast<nFloat>(tVertAdd.Weight) / 100.0f;
			tVert.normal = natTransform::normalize(w * (natMat3<>(m1) * tVert.normal) + (1.0f - w) * (natMat3<>(m2) * tVert.normal));
			tVert.vert = natVec3<>(w * (m1 * vt) + (1.0f - w) * (m2 * vt)) * m_Zoom;
		}
		m_VB->AllocData(sizeof(n2dGraphics3DVertex) * tVerts.size(), reinterpret_cast<ncData>(tVerts.data()), n2dBuffer::BufferUsage::DynamicDraw);

		m_Updated = false;
	}

	return m_VB;
}

n2dBufferImpl* n2dDynamicMeshDataImpl::GetIndexBuffer()
{
	if (!m_pRenderDevice)
	{
		return nullptr;
	}

	if (!m_IB)
	{
		m_IB = new n2dBufferImpl(n2dBuffer::BufferTarget::ElementArrayBuffer, static_cast<n2dShaderWrapperImpl*>(m_pRenderDevice->GetShaderWrapper()));
		m_IB->AllocData(m_Index.size() * sizeof(nuShort), reinterpret_cast<ncData>(m_Index.data()), n2dBuffer::BufferUsage::StaticDraw);
		m_Index.clear();
	}

	return m_IB;
}

nuInt n2dDynamicMeshDataImpl::GetVertexCount() const
{
	return m_Vert.size();
}

nuInt n2dDynamicMeshDataImpl::GetIndexCount() const
{
	return m_Index.size();
}

nBool n2dDynamicMeshDataImpl::IsStatic() const
{
	return false;
}

nResult n2dDynamicMeshDataImpl::ApplyExpression(nTString const & Name, nFloat Weight)
{
	auto tItea = m_Expression.find(Name);
	if (m_Expression.end() == tItea)
	{
		return NatErr_InternalErr;
	}
	m_pMotionManager->doApplyMorph(this, tItea->second, Weight);

	return NatErr_OK;
}

void n2dDynamicMeshDataImpl::Update(nuInt nFrame)
{
	if (m_LastFrame == nFrame)
	{
		return;
	}

	m_LastFrame = nFrame;

	m_pMotionManager->doRestoreMotion(this);
	m_pMotionManager->applyMotion(this, m_pMotion, nFrame);
	m_pMotionManager->doRestoreMorph(this);
	m_pMotionManager->applyMorph(this, m_pMotion, nFrame);
	for (auto& ik : m_IK)
		ik.Solve();

	m_Updated = true;
}

n2dDynamicMeshDataImpl::n2dDynamicMeshDataImpl()
	: m_pMotion(nullptr),
	m_pRenderDevice(nullptr),
	m_VB(nullptr),
	m_IB(nullptr),
	m_Updated(false),
	m_Zoom(1.0f),
	m_LastFrame(nuInt(-1))
{
}
