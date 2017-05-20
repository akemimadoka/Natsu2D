////////////////////////////////////////////////////////////////////////////////
///	@file	n2dFontImpl.h
///	@brief	Natsu2D字体类
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../n2dFont.h"

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
///	@brief	字符纹理
////////////////////////////////////////////////////////////////////////////////
struct n2dCharTexture final
{
	natRefPointer<n2dTexture2D>	CharTexture;	///< @brief	生成的字符纹理
	nuInt			Width;			///< @brief	宽
	nuInt			Height;			///< @brief	高

	nuInt			adv_x;
	nuInt			adv_y;
	nuInt			bearing_x;
	nuInt			bearing_y;
	nuInt			delta_x;
	nuInt			delta_y;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief		Natsu2D字体类实现
////////////////////////////////////////////////////////////////////////////////
class n2dFontImpl final
	: public natRefObjImpl<n2dFontImpl, n2dFont>
{
public:
	explicit n2dFontImpl(n2dRenderDeviceImpl* pRenderer);
	~n2dFontImpl();

	nResult InitFont(natRefPointer<n2dRenderDevice> pRenderer, nStrView lpFontFile, nuInt iWidth, nuInt iHeight) override;
	nResult InitFont(natRefPointer<n2dRenderDevice> pRenderer, natRefPointer<natStream> pStream, nuInt iWidth, nuInt iHeight) override;

	nResult InitText(nStrView str, nLen lStrlen) override;

	nResult PrintFont(natRefPointer<n2dGraphics2D> pGraphic, nStrView str, nFloat x, nFloat y, nFloat scale, natVec3<> const& color) override;
	nResult PrintFont(natRefPointer<n2dGraphics2D> pGraphic, nStrView str, nFloat x, nFloat y, nFloat scale, natRefPointer<n2dTexture> pTexture) override;

private:
	nResult PrintFontImpl(natRefPointer<n2dGraphics2D> pGraphic, nStrView str, nFloat x, nFloat y, nFloat scale);

	FT_Library m_pFTLib;
	FT_Face m_pFTFace;
	std::vector<nByte> m_FileBuf;
	std::shared_ptr<n2dImage> m_TmpImage;
	std::unordered_map<nWChar, n2dCharTexture> m_FontCache;

	n2dRenderDevice* m_pRenderer;
};