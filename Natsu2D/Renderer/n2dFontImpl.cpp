#include "n2dFontImpl.h"
#include "..\n2dCommon.h"
#include "..\n2dUtil.h"
#include "..\RenderDevice\n2dGraphicsImpl.h"
#include "..\RenderDevice\n2dRenderDeviceImpl.h"
#include <natStream.h>
#include <fstream>
#include "..\extern\freetype\include\ft2build.h"
#include "..\extern\freetype\include\freetype\freetype.h"
#include "..\extern\freetype\include\freetype\ftglyph.h"
#include "..\n2dGraphics.h"

n2dFontImpl::n2dFontImpl(int Height, int Width, int Escapement, int Orientation, int Weight, bool Italic, bool Underline, bool StrikeOut, int CharSet, int OutPrecision, int ClipPrecision, int Quality, int PitchAndFamily, LPCTSTR FaceName)
	: m_pFTLib(nullptr), m_pFTFace(nullptr), m_hFont(NULL), m_pFileBuf(nullptr)
{
	if (NATFAIL(InitFont(Height, Width, Escapement, Orientation, Weight, Italic, Underline, StrikeOut, CharSet, OutPrecision, ClipPrecision, Quality, PitchAndFamily, FaceName)))
	{
		nat_Throw(natException, _T("Failed to Create Font"));
	}
}

n2dFontImpl::n2dFontImpl(n2dRenderDeviceImpl* pRenderer)
	: m_pFTLib(nullptr), m_pFTFace(nullptr), m_pFileBuf(nullptr), m_pRenderer(pRenderer)
{
}

n2dFontImpl::~n2dFontImpl()
{
	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = nullptr;
	}
	FT_Done_Face(m_pFTFace);
	FT_Done_FreeType(m_pFTLib);
	SafeDelArr(m_pFileBuf);
}

nResult n2dFontImpl::InitFont(int Height, int Width, int Escapement, int Orientation, int Weight, bool Italic, bool Underline, bool StrikeOut, int CharSet, int OutPrecision, int ClipPrecision, int Quality, int PitchAndFamily, LPCTSTR FaceName)
{
	HFONT oldFont = m_hFont;
	m_hFont = CreateFont(Height, Width, Escapement, Orientation, Weight, Italic, Underline, StrikeOut, CharSet, OutPrecision, ClipPrecision, Quality, PitchAndFamily, FaceName);

	if (oldFont != m_hFont)
	{
		DeleteObject(oldFont);
	}

	return m_hFont != nullptr ? NatErr_OK : NatErr_InternalErr;
}

nResult n2dFontImpl::InitFont(n2dRenderDevice* pRenderer, ncTStr lpFontFile, nuInt iWidth, nuInt iHeight)
{
	natRefPointer<natStream> pStream = make_ref<natFileStream>(lpFontFile, true, false);
	return InitFont(pRenderer, pStream, iWidth, iHeight);
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
	SafeDelArr(m_pFileBuf);
	m_pFileBuf = new nByte[static_cast<nuInt>(tLen)];
	pStream->ReadBytes(m_pFileBuf, tLen);

	if (FT_New_Memory_Face(m_pFTLib, m_pFileBuf, static_cast<FT_Long>(tLen), 0, &m_pFTFace) != 0)
	{
		return NatErr_InternalErr;
	}

	FT_Select_Charmap(m_pFTFace, FT_ENCODING_UNICODE);
	FT_Set_Pixel_Sizes(m_pFTFace, iWidth, iHeight);

	return NatErr_OK;
}

nResult n2dFontImpl::InitText(ncTStr str, nLen lStrlen)
{
	/*if (!m_hFont)
	{
		return NatErr_IllegalState;
	}*/

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
		n2dCharTexture& tCharTexture = m_FontCache[str[i]];

		tCharTexture.Width = tBitmap.width;
		tCharTexture.Height = tBitmap.rows;
		tCharTexture.adv_x = m_pFTFace->glyph->advance.x / 64;
		tCharTexture.adv_y = m_pFTFace->size->metrics.y_ppem;
		tCharTexture.delta_x = tBitmapGlyph->left;
		tCharTexture.delta_y = tBitmapGlyph->top - tBitmap.rows;

		nData pBuf = new nByte[tCharTexture.Width * tCharTexture.Height * 4];
		//memset(pBuf, 0, tCharTexture.Width * tCharTexture.Height * 4);

		for (nuInt y = 0u; y < tCharTexture.Height; ++y)
		{
			for (nuInt x = 0u; x < tCharTexture.Width; ++x)
			{
				*reinterpret_cast<COLORREF*>(pBuf + 4 * i + (tCharTexture.Height - y - 1) * tCharTexture.Width * 4) = RGB(0xFF, 0xFF, 0xFF);
				pBuf[4 * i + (tCharTexture.Height - y - 1) * tCharTexture.Width * 4 + 3] = tBitmap.buffer[x + y * tBitmap.width];
			}
		}

		tCharTexture.CharTexture->LoadTexture(n2dImage(4u, GL_RGBA, pBuf, tCharTexture.Width * tCharTexture.Height * 4, tCharTexture.Width, tCharTexture.Height));
	}
	/*
	HBITMAP hBitmap, hOldBitmap;
	BITMAP bmp;
	SIZE size;

	HDC hDC = CreateCompatibleDC(NULL);

	SelectObject(hDC, m_hFont);

	auto len = lstrlen(str);
	GetTextExtentPoint32(hDC, str, len, &size);

	hBitmap = CreateBitmap(size.cx, size.cy, 1, 1, nullptr);
	hOldBitmap = static_cast<HBITMAP>(SelectObject(hDC, hBitmap));

	SetBkColor(hDC, RGB(0, 0, 0));
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkMode(hDC, OPAQUE);
	TextOut(hDC, 0, 0, str, lstrlen(str));
	//RECT tRect;
	
	//DrawText(hDC, str, len, &tRect, DT_CALCRECT);
	GetObject(hBitmap, sizeof(bmp), &bmp);
	size.cx = (bmp.bmWidth + 31) & ~31;
	size.cy = bmp.bmHeight;
	int bufsize = size.cx * size.cy * 3;
	BITMAPINFO* bin = static_cast<BITMAPINFO *>(malloc(sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD)));
	memset(bin, 0, sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD));
	bin->bmiHeader.biSize = sizeof(bin->bmiHeader);
	bin->bmiHeader.biWidth = bmp.bmWidth;
	bin->bmiHeader.biHeight = bmp.bmHeight;
	bin->bmiHeader.biPlanes = 1;
	bin->bmiHeader.biBitCount = 24;
	bin->bmiHeader.biCompression = BI_RGB;
	bin->bmiHeader.biSizeImage = bufsize;
	bin->bmiHeader.biXPelsPerMeter = 1;
	bin->bmiHeader.biYPelsPerMeter = 1;
	bin->bmiHeader.biClrUsed = 0;
	bin->bmiHeader.biClrImportant = 0;

	nByte* pBmpBits = nullptr;
	if (GetDIBits(hDC, hBitmap, 0u, bmp.bmHeight, pBmpBits, bin, DIB_RGB_COLORS) == 0)
	{
		nat_Throw(natException, n2dUtil::FormatString(_T("Errno=%d"), GetLastError()));
	}
	bufsize = bin->bmiHeader.biSizeImage;
	pBmpBits = new nByte[bufsize];
	memset(pBmpBits, 0, bufsize);
	//hBitmap = CreateDIBSection(hDC, bin, DIB_RGB_COLORS, reinterpret_cast<void**>(&pBmpBits), NULL, NULL);

	GetDIBits(hDC, hBitmap, 0u, bmp.bmHeight, pBmpBits, bin, DIB_RGB_COLORS);
	m_TmpImage = std::make_shared<n2dImage>(1, GL_RED, pBmpBits, bufsize, size.cx, size.cy);

	free(bin);
	DeleteObject(SelectObject(hDC, hOldBitmap));
	DeleteDC(hDC);*/

	return NatErr_OK;
}

nResult n2dFontImpl::PrintFont(n2dGraphics2D* pGraphic, ncTStr str, nFloat x, nFloat y, nFloat z)
{
	/*if (!m_hFont)
	{
		nat_Throw(natException, _T("Font had not initialized"));
	}*/

	nLen tLen = lstrlen(str);
	nInt sx, sy;
	n2dUtil::TransformCoord(m_pRenderer, x, y, sx, sy);

	for (nLen i = 0ul; i < tLen; ++i)
	{
		auto mapitea = m_FontCache.find(str[i]);
		if (mapitea == m_FontCache.end())
		{
			InitText(&str[i], 1ul);
			if ((mapitea = m_FontCache.find(str[i])) == m_FontCache.end())
			{
				return NatErr_InternalErr;
			}
		}

		nuInt w = mapitea->second.Width, h = mapitea->second.Height;
		nuInt chx = sx + mapitea->second.delta_x, chy = sy - h - mapitea->second.delta_y;

		n2dGraphics2DVertex tVert[4];
		n2dUtil::TransformCoord(m_pRenderer, chx,		chy,		tVert[0].vert.x, tVert[0].vert.y);
		tVert[0].uv = natVec2<>(0.f, 0.f);
		n2dUtil::TransformCoord(m_pRenderer, chx,		chy + h,	tVert[1].vert.x, tVert[1].vert.y);
		tVert[1].uv = natVec2<>(0.f, 1.f);
		n2dUtil::TransformCoord(m_pRenderer, chx + w,	chy + h,	tVert[2].vert.x, tVert[2].vert.y);
		tVert[2].uv = natVec2<>(1.f, 1.f);
		n2dUtil::TransformCoord(m_pRenderer, chx + w,	chy,		tVert[3].vert.x, tVert[3].vert.y);
		tVert[3].uv = natVec2<>(1.f, 0.f);
		
		pGraphic->DrawQuad(mapitea->second.CharTexture, tVert);

		sx += mapitea->second.adv_x;
	}

	return NatErr_OK;
}