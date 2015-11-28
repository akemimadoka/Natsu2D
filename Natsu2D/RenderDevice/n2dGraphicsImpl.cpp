#include "n2dGraphicsImpl.h"
#include "n2dRenderDeviceImpl.h"
#include "n2dTextureImpl.h"
#include "..\Engine\n2dEngineImpl.h"
#include "n2dModelLoaderImpl.h"
#include "..\n2dCommon.h"

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
		global::EventException(&ex);
		End();
	}
}

nBool n2dGraphics2DImpl::IsRendering()
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

nResult n2dGraphics2DImpl::DrawRaw(n2dTexture2D* pTex, n2dModelLoader& Model)
{
	auto IB = Model.GetIndexBuffer();
	m_Commands.emplace_back(RenderCommand{ true, pTex, Model.GetVertexBuffer(), Model.GetUVBuffer(), Model.GetNormalBuffer(), IB, Model.GetVertexCount(), Model.GetIndexCount() });

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