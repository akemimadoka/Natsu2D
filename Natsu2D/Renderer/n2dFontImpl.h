////////////////////////////////////////////////////////////////////////////////
///	@file	n2dFontImpl.h
///	@brief	Natsu2D字体类
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../n2dFont.h"

#include <Windows.h>
#include <memory>
#include <unordered_map>
#include "../RenderDevice/n2dTextureImpl.h"

struct n2dRenderDevice;
typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;

class n2dEngineImpl;
class n2dGraphics2DImpl;
class n2dRenderDeviceImpl;

////////////////////////////////////////////////////////////////////////////////
///	@brief		Natsu2D字体类实现
///	@deprecated	维护中，注释已过时，请勿使用
////////////////////////////////////////////////////////////////////////////////
class n2dFontImpl final
	: public natRefObjImpl<n2dFont>
{
public:
	n2dFontImpl(int Height, int Width, int Escapement, int Orientation, int Weight, bool Italic, bool Underline, bool StrikeOut, int CharSet, int OutPrecision, int ClipPrecision, int Quality, int PitchAndFamily, LPCTSTR FaceName);
	explicit n2dFontImpl(n2dRenderDeviceImpl* pRenderer);
	~n2dFontImpl();

	nResult InitFont(int Height, int Width, int Escapement, int Orientation, int Weight, bool Italic, bool Underline, bool StrikeOut, int CharSet, int OutPrecision, int ClipPrecision, int Quality, int PitchAndFamily, ncTStr FaceName) override;
	
	nResult InitFont(n2dRenderDevice* pRenderer, ncTStr lpFontFile, nuInt iWidth, nuInt iHeight) override;
	nResult InitFont(n2dRenderDevice* pRenderer, natStream* pStream, nuInt iWidth, nuInt iHeight) override;

	nResult InitText(ncTStr str, nLen lStrlen) override;

	nResult PrintFont(n2dGraphics2D* pGraphic, ncTStr str, natRect<> const& rect) override;

private:
	FT_Library m_pFTLib;
	FT_Face m_pFTFace;
	HFONT m_hFont;
	nData m_pFileBuf;
	std::shared_ptr<n2dImage> m_TmpImage;
	std::unordered_map<nWChar, n2dCharTexture> m_FontCache;

	n2dRenderDevice* m_pRenderer;
};