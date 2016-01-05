#pragma once
#include "n2dInterface.h"

struct n2dRenderDevice;
struct natStream;
struct n2dGraphics2D;
struct n2dTexture2D;

////////////////////////////////////////////////////////////////////////////////
///	@brief	字符纹理
////////////////////////////////////////////////////////////////////////////////
struct n2dCharTexture final
{
	n2dTexture2D*	CharTexture;	///< @brief	生成的字符纹理
	nuInt			Width;			///< @brief	宽
	nuInt			Height;			///< @brief	高

	nuInt			adv_x;
	nuInt			adv_y;
	nuInt			delta_x;
	nuInt			delta_y;

	n2dCharTexture()
		: CharTexture(nullptr),
		Width(0),
		Height(0),
		adv_x(0u),
		adv_y(0u),
		delta_x(0u),
		delta_y(0u)
	{
	}
};

struct n2dFont
	: n2dInterface
{
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
	virtual nResult InitFont(int Height, int Width, int Escapement, int Orientation, int Weight, bool Italic, bool Underline, bool StrikeOut, int CharSet, int OutPrecision, int ClipPrecision, int Quality, int PitchAndFamily, LPCTSTR FaceName) = 0;

	virtual nResult InitFont(n2dRenderDevice* pRenderer, ncTStr lpFontFile, nuInt iWidth, nuInt iHeight) = 0;
	virtual nResult InitFont(n2dRenderDevice* pRenderer, natStream* pStream, nuInt iWidth, nuInt iHeight) = 0;

	///	@brief	初始化文本
	///	@param[in]	str		文本
	///	@param[in]	lStrlen	文本长度
	///	@note	重复的文本不会重复初始化
	virtual nResult InitText(ncTStr str, nLen lStrlen) = 0;

	///	@brief	绘制字体
	///	@param[in]	pGraphic	图元渲染器
	///	@param[in]	str			要绘制的文本
	///	@param[in]	x			要绘制到的x坐标
	///	@param[in]	y			要绘制到的y坐标
	///	@param[in]	z			要绘制到的z坐标
	virtual nResult PrintFont(n2dGraphics2D* pGraphic, ncTStr str, nFloat x, nFloat y, nFloat z) = 0;
};