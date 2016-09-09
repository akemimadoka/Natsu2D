#include "n2dFontImpl.h"
#include "../n2dCommon.h"
#include "../n2dUtil.h"
#include "../RenderDevice/n2dGraphicsImpl.h"
#include "../RenderDevice/n2dRenderDeviceImpl.h"
#include <natStream.h>
#include "../extern/freetype/include/ft2build.h"
#include "../extern/freetype/include/freetype/freetype.h"
#include "../extern/freetype/include/freetype/ftglyph.h"
#include "../n2dGraphics.h"

n2dFontImpl::n2dFontImpl(n2dRenderDeviceImpl* pRenderer)
	: m_pFTLib(nullptr), m_pFTFace(nullptr), m_pRenderer(pRenderer)
{
}

n2dFontImpl::~n2dFontImpl()
{
	FT_Done_Face(m_pFTFace);
	FT_Done_FreeType(m_pFTLib);
}

nResult n2dFontImpl::InitFont(n2dRenderDevice* pRenderer, ncTStr lpFontFile, nuInt iWidth, nuInt iHeight)
{
	return InitFont(pRenderer, make_ref<natFileStream>(lpFontFile, true, false), iWidth, iHeight);
}

nResult n2dFontImpl::InitFont(n2dRenderDevice* pRenderer, natStream* pStream, nuInt iWidth, nuInt iHeight)
{
	if (pRenderer != nullptr)
	{
		m_pRenderer = pRenderer;
	}

	m_FontCache.clear();
	FT_Done_Face(m_pFTFace);
	FT_Done_FreeType(m_pFTLib);

	if (FT_Init_FreeType(&m_pFTLib) != 0)
	{
		return NatErr_InternalErr;
	}

	nLen tLen = pStream->GetSize();
	m_FileBuf.resize(static_cast<size_t>(tLen));
	pStream->ReadBytes(m_FileBuf.data(), tLen);

	if (FT_New_Memory_Face(m_pFTLib, m_FileBuf.data(), static_cast<FT_Long>(tLen), 0, &m_pFTFace) != 0)
	{
		return NatErr_InternalErr;
	}

	FT_Select_Charmap(m_pFTFace, FT_ENCODING_UNICODE);
	FT_Set_Pixel_Sizes(m_pFTFace, iWidth, iHeight);

	return NatErr_OK;
}

nResult n2dFontImpl::InitText(ncTStr str, nLen lStrlen)
{
	if (!m_pFTLib)
	{
		return NatErr_IllegalState;
	}

	GLint oldAlignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldAlignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (nLen i = 0; i < lStrlen; ++i)
	{
		if (m_FontCache.find(str[i]) != m_FontCache.end())
			continue;

		if (FT_Load_Char(m_pFTFace, str[i], FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_NORMAL) != 0)
		{
			return NatErr_InternalErr;
		}

		FT_Glyph tGlyph;

		if (FT_Get_Glyph(m_pFTFace->glyph, &tGlyph) != 0)
		{
			return NatErr_InternalErr;
		}

		FT_Render_Glyph(m_pFTFace->glyph, FT_RENDER_MODE_NORMAL);
		FT_Glyph_To_Bitmap(&tGlyph, FT_RENDER_MODE_NORMAL, nullptr, TRUE);
		FT_BitmapGlyph tBitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(tGlyph);

		FT_Bitmap& tBitmap = tBitmapGlyph->bitmap;
		auto& tCharTexture = m_FontCache[str[i]];

		tCharTexture.Width = tBitmap.width;
		tCharTexture.Height = tBitmap.rows;
		tCharTexture.adv_x = m_pFTFace->glyph->advance.x >> 6;
		tCharTexture.adv_y = m_pFTFace->size->metrics.y_ppem;
		tCharTexture.bearing_x = m_pFTFace->glyph->bitmap_left;
		tCharTexture.bearing_y = m_pFTFace->glyph->bitmap_top;
		tCharTexture.delta_x = tBitmapGlyph->left;
		tCharTexture.delta_y = tBitmapGlyph->top - tBitmap.rows;

		tCharTexture.CharTexture = make_ref<n2dTexture2DImpl>();
		tCharTexture.CharTexture->LoadTexture(n2dImage{ GL_RED, GL_RED, tBitmap.buffer, tCharTexture.Width * tCharTexture.Height, tCharTexture.Width, tCharTexture.Height });
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, oldAlignment);
	return NatErr_OK;
}

nResult n2dFontImpl::PrintFont(n2dGraphics2D* pGraphic, ncTStr str, nFloat x, nFloat y, nFloat scale, natVec3<> const& color)
{
	if (!m_pFTLib)
	{
		return NatErr_IllegalState;
	}

	nBool shouldEnd;
	auto pUsingProgram = m_pRenderer->GetShaderWrapper()->GetCurrentProgram();
	auto pFontProgram = m_pRenderer->GetShaderWrapper()->GetFontProgram();
	if (pGraphic->IsRendering())
	{
		shouldEnd = false;
		pGraphic->Flush();
	}
	else
	{
		shouldEnd = true;
		pGraphic->Begin();
	}
	m_pRenderer->PushMVPMat();
	m_pRenderer->InitMVPMat();
	auto window = m_pRenderer->GetEngine()->GetWindow();
	m_pRenderer->SubmitProjMat(natTransform::ortho(0.0f, static_cast<nFloat>(window->GetWidth()), 0.0f, static_cast<nFloat>(window->GetHeight())));
	auto scope = make_scope([shouldEnd, pGraphic, pUsingProgram, this]
	{
		if (shouldEnd)
		{
			pGraphic->End();
		}
		else
		{
			pGraphic->Flush();
		}

		m_pRenderer->PopMVPMat();
		pUsingProgram->Use();
	});

	pFontProgram->Use();
	GLint useTexture = GL_FALSE;
	pFontProgram->GetUniformReference(_T("useTexture"))->SetValue(1, &useTexture);
	pFontProgram->GetUniformReference(_T("textColor"))->SetValue(1, &color);

	return PrintFontImpl(pGraphic, str, x, y, scale);
}

nResult n2dFontImpl::PrintFont(n2dGraphics2D* pGraphic, ncTStr str, nFloat x, nFloat y, nFloat scale, n2dTexture* pTexture)
{
	if (!m_pFTLib)
	{
		return NatErr_IllegalState;
	}

	nBool shouldEnd;
	auto pUsingProgram = m_pRenderer->GetShaderWrapper()->GetCurrentProgram();
	auto pFontProgram = m_pRenderer->GetShaderWrapper()->GetFontProgram();
	if (pGraphic->IsRendering())
	{
		shouldEnd = false;
		pGraphic->Flush();
	}
	else
	{
		shouldEnd = true;
		pGraphic->Begin();
	}
	m_pRenderer->PushMVPMat();
	m_pRenderer->InitMVPMat();
	auto window = m_pRenderer->GetEngine()->GetWindow();
	m_pRenderer->SubmitProjMat(natTransform::ortho(0.0f, static_cast<nFloat>(window->GetWidth()), 0.0f, static_cast<nFloat>(window->GetHeight())));
	auto scope = make_scope([shouldEnd, pGraphic, pUsingProgram, this]
	{
		if (shouldEnd)
		{
			pGraphic->End();
		}
		else
		{
			pGraphic->Flush();
		}

		m_pRenderer->PopMVPMat();
		pUsingProgram->Use();
	});

	pFontProgram->Use();
	GLint useTexture = GL_TRUE;
	pFontProgram->GetUniformReference(_T("useTexture"))->SetValue(1, &useTexture);
	GLint textureID = 0;
	pFontProgram->GetUniformReference(_T("text"))->SetValue(1, &textureID);
	textureID = 1;
	pFontProgram->GetUniformReference(_T("textTexture"))->SetValue(1, &textureID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pTexture->GetTextureID());

	return PrintFontImpl(pGraphic, str, x, y, scale);
}

nResult n2dFontImpl::PrintFontImpl(n2dGraphics2D* pGraphic, ncTStr str, nFloat x, nFloat y, nFloat scale)
{
	nLen tLen = std::char_traits<nTChar>::length(str);
	nFloat currentx = x, currenty = y;
	for (nLen i = 0; i < tLen; ++i)
	{
		auto mapitea = m_FontCache.find(str[i]);
		if (mapitea == m_FontCache.end())
		{
			InitText(&str[i], 1);
			if ((mapitea = m_FontCache.find(str[i])) == m_FontCache.end())
			{
				return NatErr_InternalErr;
			}
		}

		if (mapitea->first == '\n')
		{
			currentx = x;
			currenty += mapitea->second.adv_y;
			continue;
		}

		auto w = mapitea->second.Width * scale, h = mapitea->second.Height * scale;
		auto xPos = currentx + mapitea->second.bearing_x * scale;
		auto yPos = currenty - (mapitea->second.Height - mapitea->second.bearing_y) * scale;

		pGraphic->DrawQuad(mapitea->second.CharTexture,
			n2dGraphics2DVertex{ { xPos, yPos + h, 0.f }, 0,{ 0.f, 0.f } },
			n2dGraphics2DVertex{ { xPos, yPos, 0.f }, 0,{ 0.f, 1.f } },
			n2dGraphics2DVertex{ { xPos + w, yPos, 0.f }, 0,{ 1.f, 1.f } },
			n2dGraphics2DVertex{ { xPos + w, yPos + h, 0.f }, 0,{ 1.f, 0.f } });

		currentx += mapitea->second.adv_x * scale;
	}

	return NatErr_OK;
}
