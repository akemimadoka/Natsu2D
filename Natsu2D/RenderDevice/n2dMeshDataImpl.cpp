#include "n2dMeshDataImpl.h"
#include "n2dTextureImpl.h"
#include "n2dMotionManagerImpl.h"
#include "OpenGL.h"
#include <natStream.h>

#ifdef max
#	undef max
#endif

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

		for (auto&& v : m_Vert)
		{
			v.vert *= m_Zoom;
		}

		m_VB = make_ref<n2dBufferImpl>(n2dBuffer::BufferTarget::ArrayBuffer, static_cast<natRefPointer<n2dShaderWrapperImpl>>(m_pRenderDevice->GetShaderWrapper()));
		m_VB->AllocData(static_cast<nuInt>(sizeof(n2dGraphics3DVertex) * m_Vert.size()), reinterpret_cast<ncData>(m_Vert.data()), n2dBuffer::BufferUsage::StaticDraw);
		m_Vert.clear();
	}

	return m_VB.Get();
}

n2dBufferImpl* n2dStaticMeshDataImpl::GetIndexBuffer()
{
	if (!m_IB)
	{
		if (!m_pRenderDevice)
		{
			return nullptr;
		}

		m_IB = make_ref<n2dBufferImpl>(n2dBuffer::BufferTarget::ElementArrayBuffer, static_cast<natRefPointer<n2dShaderWrapperImpl>>(m_pRenderDevice->GetShaderWrapper()));
		m_IB->AllocData(static_cast<nuInt>(sizeof(nuShort) * m_Index.size()), reinterpret_cast<ncData>(m_Index.data()), n2dBuffer::BufferUsage::StaticDraw);
		m_Index.clear();
	}

	return m_IB.Get();
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
	: m_Material { false }, m_pRenderDevice(nullptr), m_VB(), m_IB(), m_VertCount(0), m_IndexCount(0), m_Zoom(1.0f)
{
}

n2dDynamicMeshDataImpl::~n2dDynamicMeshDataImpl()
{
}

void n2dDynamicMeshDataImpl::UpdateVertexBuffer()
{
	if (!m_VB)
	{
		m_VB = make_ref<n2dBufferImpl>(n2dBuffer::BufferTarget::ArrayBuffer, static_cast<natRefPointer<n2dShaderWrapperImpl>>(m_pRenderDevice->GetShaderWrapper()));
		m_VB->AllocData(static_cast<nuInt>(sizeof(n2dGraphics3DVertex) * m_Vert.size()), reinterpret_cast<ncData>(m_Vert.data()), n2dBuffer::BufferUsage::DynamicDraw);
	}

	if (m_Updated)
	{
		static std::vector<n2dGraphics3DVertex> tVerts;
		tVerts = m_Vert;
		const n2dBone* pBone = m_Selekton->m_Bones.data();
		static natVec4<> vt;
		for (auto i = 0u; i < m_Vert.size(); ++i)
		{
			auto&& tVert = tVerts[i];
			auto&& tVertAdd = m_VertAdd[i];
			vt.x = tVert.vert.x;
			vt.y = tVert.vert.y;
			vt.z = tVert.vert.z;
			vt.w = 1.0f;

			auto&& m1 = pBone[tVertAdd.TargetBone[0]].GetMatrix();
			auto&& m2 = pBone[tVertAdd.TargetBone[1]].GetMatrix();

			auto w = static_cast<nFloat>(tVertAdd.Weight) / 100.0f;
			tVert.normal = natTransform::normalize(w * (static_cast<natMat3<>>(m1) * tVert.normal) + (1.0f - w) * (static_cast<natMat3<>>(m2) * tVert.normal));
			tVert.vert = natVec3<>(w * (m1 * vt) + (1.0f - w) * (m2 * vt)) * m_Zoom;
		}
		m_VB->AllocData(static_cast<nuInt>(sizeof(n2dGraphics3DVertex) * tVerts.size()), reinterpret_cast<ncData>(tVerts.data()), n2dBuffer::BufferUsage::DynamicDraw);

		m_Updated = false;
	}
}

n2dBufferImpl* n2dDynamicMeshDataImpl::GetVertexBuffer()
{
	if (!m_pRenderDevice)
	{
		return 0u;
	}

	UpdateVertexBuffer();

	return m_VB.Get();
}

n2dBufferImpl* n2dDynamicMeshDataImpl::GetIndexBuffer()
{
	if (!m_pRenderDevice)
	{
		return nullptr;
	}

	if (!m_IB)
	{
		m_IB = make_ref<n2dBufferImpl>(n2dBuffer::BufferTarget::ElementArrayBuffer, static_cast<natRefPointer<n2dShaderWrapperImpl>>(m_pRenderDevice->GetShaderWrapper()));
		m_IB->AllocData(static_cast<nuInt>(m_Index.size() * sizeof(nuShort)), reinterpret_cast<ncData>(m_Index.data()), n2dBuffer::BufferUsage::StaticDraw);
		m_Index.clear();
	}

	return m_IB.Get();
}

nuInt n2dDynamicMeshDataImpl::GetVertexCount() const
{
	return static_cast<nuInt>(m_Vert.size());
}

nuInt n2dDynamicMeshDataImpl::GetIndexCount() const
{
	return static_cast<nuInt>(m_Index.size());
}

nBool n2dDynamicMeshDataImpl::IsStatic() const
{
	return false;
}

nResult n2dDynamicMeshDataImpl::ApplyExpression(nStrView const& Name, nFloat Weight)
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
	for (auto&& ik : m_IK)
		ik.Solve();

	m_Updated = true;
}

n2dDynamicMeshDataImpl::n2dDynamicMeshDataImpl()
	: m_pMotion(nullptr),
	m_pMotionManager(nullptr),
	m_pRenderDevice(nullptr),
	m_VB(nullptr),
	m_IB(nullptr),
	m_Updated(false),
	m_Zoom(1.0f),
	m_LastFrame(std::numeric_limits<nuInt>::max())
{
}
