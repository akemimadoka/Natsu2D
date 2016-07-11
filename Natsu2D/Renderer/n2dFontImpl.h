////////////////////////////////////////////////////////////////////////////////
///	@file	n2dFontImpl.h
///	@brief	Natsu2D字体类
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\n2dFont.h"

#include <Windows.h>
#include <memory>
#include <unordered_map>
#include "..\RenderDevice\n2dTextureImpl.h"

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
	///	@brief	n2dFont构造函数
	///	@note	初始化失败会抛出异常，单独使用n2dFont::InitFont不会抛出异常
	///	@see	n2dFontImpl::InitFont
	n2dFontImpl(int Height, int Width, int Escapement, int Orientation, int Weight, bool Italic, bool Underline, bool StrikeOut, int CharSet, int OutPrecision, int ClipPrecision, int Quality, int PitchAndFamily, LPCTSTR FaceName);
	explicit n2dFontImpl(n2dRenderDeviceImpl* pRenderer);
	~n2dFontImpl();

	///	@brief	初始化字体
	///	@param[in]	Height			字体高度
	///	@param[in]	Width			字体宽度
	///	@param[in]	Escapement		移位向量和设备X轴之间的角度
	///	@param[in]	Orientation		每个字符的基线和设备X轴之间的角度
	///	@param[in]	Weight			字体的权值，更高的值使得字体更粗，取值[0, 1000]
	///	@param[in]	Italic			斜体
	///	@param[in]	Underline		下划线
	///	@param[in]	StrikeOut		删除线
	///	@param[in]	CharSet			字符集
	///	@param[in]	OutPrecision	输出精度
	///	@param[in]	ClipPrecision	裁剪精度
	///	@param[in]	Quality			输出质量
	///	@param[in]	PitchAndFamily	字体间距和字体族
	///	@param[in]	FaceName		字体字样名，长度不得超过32个字符（包括\\0）
	///	@note	在构造时被调用，之后可以再次使用该方法更换字体
	///	@return	是否成功初始化
	nResult InitFont(int Height, int Width, int Escapement, int Orientation, int Weight, bool Italic, bool Underline, bool StrikeOut, int CharSet, int OutPrecision, int ClipPrecision, int Quality, int PitchAndFamily, ncTStr FaceName) override;
	
	nResult InitFont(n2dRenderDevice* pRenderer, ncTStr lpFontFile, nuInt iWidth, nuInt iHeight) override;
	nResult InitFont(n2dRenderDevice* pRenderer, natStream* pStream, nuInt iWidth, nuInt iHeight) override;

	///	@brief	初始化文本
	///	@param[in]	str		文本
	///	@param[in]	lStrlen	文本长度
	///	@note	重复的文本不会重复初始化
	nResult InitText(ncTStr str, nLen lStrlen) override;

	///	@brief	绘制字体
	///	@param[in]	pGraphic	图元渲染器
	///	@param[in]	str			要绘制的文本
	///	@param[in]	x			要绘制到的x坐标
	///	@param[in]	y			要绘制到的y坐标
	///	@param[in]	z			要绘制到的z坐标
	nResult PrintFont(n2dGraphics2D* pGraphic, ncTStr str, nFloat x, nFloat y, nFloat z) override;

private:
	FT_Library m_pFTLib;
	FT_Face m_pFTFace;
	HFONT m_hFont;
	nData m_pFileBuf;
	std::shared_ptr<n2dImage> m_TmpImage;
	std::unordered_map<nWChar, n2dCharTexture> m_FontCache;

	n2dRenderDevice* m_pRenderer;
};