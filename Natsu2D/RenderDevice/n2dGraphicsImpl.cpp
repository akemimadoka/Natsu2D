#include "n2dGraphicsImpl.h"
#include "n2dRenderDeviceImpl.h"
#include "..\Engine\n2dEngineImpl.h"
#include "..\n2dCommon.h"
#include "n2dMeshDataImpl.h"
#include "n2dModelImpl.h"
#include "n2dBufferImpl.h"

n2dGraphics2DImpl::n2dGraphics2DImpl(n2dRenderDevice* pRenderDevice)
	: m_pRenderDevice(pRenderDevice),
	m_bIsRendering(false)
{
}

n2dGraphics2DImpl::~n2dGraphics2DImpl()
{
	if (m_bIsRendering)
	{
		// 析构函数不应当抛出异常
		// 待解决
		natException ex(_T("n2dGraphics2DImpl::~n2dGraphics2DImpl"), _T("End should be invoked before destroy"));
		n2dGlobal::EventException(&ex);
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

nResult n2dGraphics2DImpl::DrawQuad(n2dTexture2D* pTex, n2dGraphics2DVertex const& v1, n2dGraphics2DVertex const& v2, n2dGraphics2DVertex const& v3, n2dGraphics2DVertex const& v4)
{
	if (!m_bIsRendering)
		return NatErr_IllegalState;

	n2dGraphics2DVertex tVert[4] = { v1, v2, v3, v4 };
	
	pushCommand(pTex, 4, 6, tVert, tQuadIndex);

	return NatErr_OK;
}

nResult n2dGraphics2DImpl::DrawQuad(n2dTexture2D* pTex, const n2dGraphics2DVertex* varr)
{
	if (!m_bIsRendering)
		return NatErr_IllegalState;

	pushCommand(pTex, 4, 6, varr, tQuadIndex);

	return NatErr_OK;
}

nResult n2dGraphics2DImpl::DrawRaw(n2dTexture2D* pTex, nuInt cVertex, nuInt cIndex, const n2dGraphics2DVertex* varr, const nuShort* iarr)
{
	if (!m_bIsRendering)
		return NatErr_IllegalState;

	pushCommand(pTex, cVertex, cIndex, varr, iarr);

	return NatErr_OK;
}

void n2dGraphics2DImpl::pushCommand(n2dTexture2D* pTex, nuInt cVertex, nuInt cIndex, const n2dGraphics2DVertex* varr, const nuShort* iarr)
{
	if (pTex)
	{
		pTex->AddRef();
	}

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
	m_Commands.emplace_back(RenderCommand{ false, pTex, VB, UVB, 0u, IB, cVertex, cIndex });
}

void n2dGraphics2DImpl::flush()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	for (nuInt i = 0u; i < m_Commands.size(); ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_Commands[i].pTex->GetTextureID());
		// 1st attribute buffer : vertices
		glBindBuffer(GL_ARRAY_BUFFER, m_Commands[i].VertexBuffer);
		glVertexAttribPointer(
			0,			// attribute. No particular reason for 0, but must match the layout in the shader.
			3,			// size
			GL_FLOAT,	// type
			GL_FALSE,	// normalized?
			0,			// stride
			nullptr		// array buffer offset
			);

		// 2nd attribute buffer : UVs

		glBindBuffer(GL_ARRAY_BUFFER, m_Commands[i].UVBuffer);
		glVertexAttribPointer(
			1,			// attribute. No particular reason for 1, but must match the layout in the shader.
			2,			// size : U+V => 2
			GL_FLOAT,	// type
			GL_FALSE,	// normalized?
			0,			// stride
			nullptr		// array buffer offset
			);

		glBindBuffer(GL_ARRAY_BUFFER, m_Commands[i].NormalBuffer);
		glVertexAttribPointer(
			2,			// attribute
			3,			// size
			GL_FLOAT,	// type
			GL_FALSE,	// normalized?
			0,			// stride
			nullptr		// array buffer offset
			);

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

	m_Commands.clear();
}

n2dGraphics3DImpl::n2dGraphics3DImpl(n2dRenderDevice* pRenderDevice)
	: m_MaterialID(0u),
	m_pRenderDevice(pRenderDevice),
	m_MaterialBuffer(0u)
{
}

n2dGraphics3DImpl::~n2dGraphics3DImpl()
{
	if (m_bIsRendering)
	{
		// 析构函数不应当抛出异常
		// 待解决
		natException ex(_T("n2dGraphics3DImpl::~n2dGraphics3DImpl"), _T("End should be invoked before destroy"));
		n2dGlobal::EventException(&ex);
		End();
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

nResult n2dGraphics3DImpl::RenderModel(n2dModelData* pModelData)
{
	n2dModelDataImpl* pMesh = dynamic_cast<n2dModelDataImpl *>(pModelData);

	if (!pMesh)
	{
		return NatErr_InvalidArg;
	}

	for (auto& i : pMesh->m_Meshes)
	{
		m_Materials.emplace_back(i->m_Material);
		m_Commands.emplace_back(RenderCommand{ m_Materials.size() - 1, i->GetVertexBuffer(), i->GetIndexBuffer(), i->GetVertexCount(), i->GetIndexCount() });
	}

	return NatErr_OK;
}

void n2dGraphics3DImpl::flush()
{
	/*if (!m_MaterialID)
	{
		GLint ProgramID = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &ProgramID);

		m_MaterialID = glGetUniformBlockIndex(ProgramID, "Material");

		if (!m_MaterialBuffer)
		{
			glGenBuffers(1, &m_MaterialBuffer);
			glBindBuffer(GL_UNIFORM_BUFFER, m_MaterialBuffer);
			glBindBufferBase(GL_UNIFORM_BUFFER, m_MaterialID, m_MaterialBuffer);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(n2dMeshData::Material) - offsetof(n2dMeshData::Material, Diffuse), nullptr, GL_DYNAMIC_DRAW);
		}
	}*/

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	for (auto& c : m_Commands)
	{
		n2dMeshDataImpl::Material& material = m_Materials[c.iMaterial];
		glBindTexture(GL_TEXTURE_2D, material.Texture->GetTextureID());
		glBindBuffer(GL_UNIFORM_BUFFER, m_MaterialBuffer);
		//glBufferData(GL_UNIFORM_BUFFER, sizeof(n2dMeshData::Material) - offsetof(n2dMeshData::Material, Diffuse), &material.Diffuse, GL_DYNAMIC_DRAW);
		void* pMaterial = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
		if (pMaterial)
		{
			memcpy_s(pMaterial, sizeof(n2dMeshData::Material) - offsetof(n2dMeshData::Material, Diffuse), &material.Diffuse, sizeof(n2dMeshData::Material) - offsetof(n2dMeshData::Material, Diffuse));
		}
		glUnmapBuffer(GL_UNIFORM_BUFFER);

		glPolygonMode(GL_FRONT_AND_BACK, material.WireFrame ? GL_LINE : GL_FILL);

		if (material.Both_sided)
		{
			glEnable(GL_CULL_FACE);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}

		glBindBuffer(GL_ARRAY_BUFFER, c.VertexBuffer);
		glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, sizeof(n2dGraphics3DVertex), reinterpret_cast<void*>(offsetof(n2dGraphics3DVertex, vert)));
		glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, sizeof(n2dGraphics3DVertex), reinterpret_cast<void*>(offsetof(n2dGraphics3DVertex, uv)));
		glVertexAttribPointer(2u, 3, GL_FLOAT, GL_FALSE, sizeof(n2dGraphics3DVertex), reinterpret_cast<void*>(offsetof(n2dGraphics3DVertex, normal)));
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c.IndexBuffer);
		glDrawElements(GL_TRIANGLES, c.cIndex, GL_UNSIGNED_SHORT, nullptr);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	m_Commands.clear();
}