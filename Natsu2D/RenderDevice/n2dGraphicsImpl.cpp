#include "n2dGraphicsImpl.h"
#include "n2dRenderDeviceImpl.h"
#include "../Engine/n2dEngineImpl.h"
#include "n2dMeshDataImpl.h"
#include "n2dModelImpl.h"
#include <natStream.h>

n2dGraphics2DImpl::n2dGraphics2DImpl(n2dRenderDeviceImpl* pRenderDevice)
	: m_pRenderDevice(pRenderDevice),
	m_VB(nullptr),
	m_IB(nullptr),
	m_bIsRendering(false)
{
	nat_ThrowIfFailed(m_pRenderDevice->CreateBuffer(n2dBuffer::BufferTarget::ArrayBuffer, m_VB), "Create vertex buffer failed."_nv);
	nat_ThrowIfFailed(m_pRenderDevice->CreateBuffer(n2dBuffer::BufferTarget::ElementArrayBuffer, m_IB), "Create index buffer failed."_nv);
}

n2dGraphics2DImpl::~n2dGraphics2DImpl()
{
	if (m_bIsRendering)
	{
		m_pRenderDevice->GetEngine()->GetLogger().LogWarn("n2dGraphics2DImpl::End should be invoked before destroy"_nv);
		End();
	}
}

nBool n2dGraphics2DImpl::IsRendering() const
{
	return m_bIsRendering;
}

nResult n2dGraphics2DImpl::Begin()
{
	m_bIsRendering = true;
	return NatErr_OK;
}

nResult n2dGraphics2DImpl::Flush()
{
	if (!m_bIsRendering)
		return NatErr_IllegalState;

	flush();
	
	return NatErr_OK;
}

nResult n2dGraphics2DImpl::End()
{
	flush();
	m_bIsRendering = false;

	return NatErr_OK;
}

namespace
{
	const nuShort tQuadIndex[] = { 0u, 1u, 2u, 0u, 2u, 3u };
}

nResult n2dGraphics2DImpl::DrawQuad(natRefPointer<n2dTexture2D> pTex, n2dGraphics2DVertex const& v1, n2dGraphics2DVertex const& v2, n2dGraphics2DVertex const& v3, n2dGraphics2DVertex const& v4)
{
	if (!m_bIsRendering)
		return NatErr_IllegalState;

	n2dGraphics2DVertex tVert[4] = { v1, v2, v3, v4 };
	
	pushCommand(pTex, 4, 6, tVert, tQuadIndex);

	return NatErr_OK;
}

nResult n2dGraphics2DImpl::DrawQuad(natRefPointer<n2dTexture2D> pTex, const n2dGraphics2DVertex* varr)
{
	if (!m_bIsRendering)
		return NatErr_IllegalState;

	pushCommand(pTex, 4, 6, varr, tQuadIndex);

	return NatErr_OK;
}

nResult n2dGraphics2DImpl::DrawRaw(natRefPointer<n2dTexture2D> pTex, nuInt cVertex, nuInt cIndex, const n2dGraphics2DVertex* varr, const nuShort* iarr)
{
	if (!m_bIsRendering)
		return NatErr_IllegalState;

	pushCommand(pTex, cVertex, cIndex, varr, iarr);

	return NatErr_OK;
}

void n2dGraphics2DImpl::pushCommand(natRefPointer<n2dTexture2D> pTex, nuInt cVertex, nuInt cIndex, const n2dGraphics2DVertex* varr, const nuShort* iarr)
{
	std::vector<natVec3<>> tVert(cVertex);
	std::vector<natVec2<>> tUV(cVertex);
	for (nuInt i = 0u; i < cVertex; ++i)
	{
		tVert[i] = varr[i].vert;
		tUV[i] = varr[i].uv;
	}

	nuInt VB, UVB, IB;

	glGenBuffers(1, &VB);
	glGenBuffers(1, &UVB);
	glGenBuffers(1, &IB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, cVertex * sizeof(natVec3<>), &tVert[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, UVB);
	glBufferData(GL_ARRAY_BUFFER, cVertex * sizeof(natVec2<>), &tUV[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cIndex * sizeof(nuShort), iarr, GL_STATIC_DRAW);
	m_Commands.emplace_back(RenderCommand{ false, natRefPointer<n2dTexture2D>(pTex), VB, UVB, 0u, IB, cVertex, cIndex });
}

void n2dGraphics2DImpl::flush()
{
	m_pRenderDevice->updateMVP();

	GLboolean bCullFace = glIsEnabled(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	for (nuInt i = 0u; i < m_Commands.size(); ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_Commands[i].pTex->GetTextureID());

		glBindBuffer(GL_ARRAY_BUFFER, m_Commands[i].VertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, m_Commands[i].UVBuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, m_Commands[i].NormalBuffer);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Commands[i].IndexBuffer);
		glDrawElements(GL_TRIANGLES, m_Commands[i].cIndex, GL_UNSIGNED_SHORT, NULL);

		if (!m_Commands[i].bKeepBuffer)
		{
			glDeleteBuffers(1, &m_Commands[i].VertexBuffer);
			glDeleteBuffers(1, &m_Commands[i].UVBuffer);
			glDeleteBuffers(1, &m_Commands[i].NormalBuffer);
			glDeleteBuffers(1, &m_Commands[i].IndexBuffer);
		}
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	if (bCullFace)
	{
		glEnable(GL_CULL_FACE);
	}

	m_Commands.clear();
}

n2dGraphics3DImpl::n2dGraphics3DImpl(n2dRenderDeviceImpl* pRenderDevice)
	: m_MaterialID(0u),
	m_pRenderDevice(pRenderDevice),
	m_MaterialBuffer(nullptr), m_bIsRendering(false)
{
	m_MaterialBuffer = make_ref<n2dBufferImpl>(n2dBuffer::BufferTarget::UniformBuffer, static_cast<natRefPointer<n2dShaderWrapperImpl>>(m_pRenderDevice->GetShaderWrapper()));
	m_MaterialBuffer->BindBase(2u);
}

n2dGraphics3DImpl::~n2dGraphics3DImpl()
{
	if (m_bIsRendering)
	{
		m_pRenderDevice->GetEngine()->GetLogger().LogWarn("n2dGraphics3DImpl::End should be invoked before destroy"_nv);
		n2dGraphics3DImpl::End();
	}
}

nBool n2dGraphics3DImpl::IsRendering() const
{
	return m_bIsRendering;
}

nResult n2dGraphics3DImpl::Begin()
{
	m_bIsRendering = true;

	return NatErr_OK;
}

nResult n2dGraphics3DImpl::Flush()
{
	if (!m_bIsRendering)
	{
		return NatErr_IllegalState;
	}

	flush();
	return NatErr_OK;
}

nResult n2dGraphics3DImpl::End()
{
	m_bIsRendering = false;

	flush();

	return NatErr_OK;
}

nResult n2dGraphics3DImpl::RenderModel(natRefPointer<n2dModelData> pModelData)
{
	if (!pModelData)
	{
		return NatErr_InvalidArg;
	}

	if (pModelData->IsStatic())
	{
		natRefPointer<n2dStaticModelDataImpl> pModel = pModelData;

		if (!pModel)
		{
			return NatErr_InvalidArg;
		}

		m_StaticMaterials.reserve(m_StaticMaterials.size() + pModel->m_Meshes.size());
		for (auto&& i : pModel->m_Meshes)
		{
			m_StaticMaterials.emplace_back(&i->m_Material);
			std::vector<nuInt> tMatID = { static_cast<nuInt>(m_StaticMaterials.size() - 1) };
			m_Commands.emplace_back(RenderCommand{ true, tMatID, i->GetVertexBuffer(), i->GetIndexBuffer(), i->GetVertexCount(), i->GetIndexCount() });
		}
	}
	else
	{
		natRefPointer<n2dDynamicModelDataImpl> pModel = pModelData;

		if (!pModel)
		{
			return NatErr_InvalidArg;
		}

		m_DynamicMaterials.reserve(m_DynamicMaterials.size() + pModel->m_Mesh.m_Materials.size());
		auto cMaterials = pModel->m_Mesh.m_Materials.size();
		std::vector<nuInt> tMatID(cMaterials);
		
		for (size_t i = 0u; i < cMaterials; ++i)
		{
			m_DynamicMaterials.push_back(&pModel->m_Mesh.m_Materials[i]);
			tMatID[i] = static_cast<nuInt>(m_DynamicMaterials.size() - 1);
		}

		m_Commands.emplace_back(RenderCommand{ false, tMatID, pModel->m_Mesh.GetVertexBuffer(), pModel->m_Mesh.GetIndexBuffer(), pModel->m_Mesh.GetVertexCount(), pModel->m_Mesh.GetIndexCount() });
	}

	return NatErr_OK;
}

void n2dGraphics3DImpl::flush()
{
	m_pRenderDevice->updateMVP();

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);

	n2dMeshData::Material* material;

	for (auto& c : m_Commands)
	{
		c.VertexBuffer->Bind();
		glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, sizeof(n2dGraphics3DVertex), reinterpret_cast<void*>(offsetof(n2dGraphics3DVertex, vert)));
		glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, sizeof(n2dGraphics3DVertex), reinterpret_cast<void*>(offsetof(n2dGraphics3DVertex, uv)));
		glVertexAttribPointer(2u, 3, GL_FLOAT, GL_FALSE, sizeof(n2dGraphics3DVertex), reinterpret_cast<void*>(offsetof(n2dGraphics3DVertex, normal)));
		
		c.IndexBuffer->Bind();

		nuInt Count;
		ncData tOffset;
		for (auto iMater : c.iMaterial)
		{
			if (c.bStatic)
			{
				material = m_StaticMaterials[iMater];
				Count = c.cIndex;
				tOffset = nullptr;
			}
			else
			{
				n2dMeshData::DynamicMaterial* pdynamicmaterial = m_DynamicMaterials[iMater];
				material = &pdynamicmaterial->BaseMaterial;
				Count = pdynamicmaterial->Length;
				tOffset = reinterpret_cast<ncData>(pdynamicmaterial->Start * sizeof(nuShort));
			}

			m_MaterialBuffer->AllocData(sizeof(n2dMeshData::Material) - offsetof(n2dMeshData::Material, Diffuse), reinterpret_cast<ncData>(&material->Diffuse), n2dBuffer::BufferUsage::DynamicDraw);

			glPolygonMode(GL_FRONT_AND_BACK, material->WireFrame ? GL_LINE : GL_FILL);

			(material->Both_sided ? glEnable : glDisable)(GL_CULL_FACE);

			glBindTexture(GL_TEXTURE_2D, material->Texture->GetTextureID());
			glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_SHORT, tOffset);

			if (c.bStatic)
				break;
		}
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	m_Commands.clear();
	m_StaticMaterials.clear();
	m_DynamicMaterials.clear();
}
