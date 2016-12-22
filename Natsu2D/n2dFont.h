////////////////////////////////////////////////////////////////////////////////
///	@file	n2dFont.h
///	@brief	Natsu2D字体
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "n2dInterface.h"
#include "natMath.h"

struct n2dTexture;
struct n2dRenderDevice;
namespace NatsuLib
{
	struct natStream;
}
struct n2dGraphics2D;
struct n2dTexture2D;

////////////////////////////////////////////////////////////////////////////////
/// @brief	Natsu2D字体
////////////////////////////////////////////////////////////////////////////////
struct n2dFont
	: n2dInterface
{

	/// @brief	使用字体文件进行初始化
	/// @param[in]	pRenderer	渲染设备
	/// @param[in]	lpFontFile	字体文件名
	/// @param[in]	iWidth		字体宽度（像素）
	/// @param[in]	iHeight		字体高度（像素）
	virtual nResult InitFont(n2dRenderDevice* pRenderer, nStrView lpFontFile, nuInt iWidth, nuInt iHeight) = 0;
	/// @brief	使用字体文件进行初始化
	/// @param[in]	pRenderer	渲染设备
	/// @param[in]	pStream		字体流
	/// @param[in]	iWidth		字体宽度（像素）
	/// @param[in]	iHeight		字体高度（像素）
	virtual nResult InitFont(n2dRenderDevice* pRenderer, natStream* pStream, nuInt iWidth, nuInt iHeight) = 0;

	///	@brief	初始化文本
	///	@param[in]	str		文本
	///	@param[in]	lStrlen	文本长度
	///	@note	重复的文本不会重复初始化
	virtual nResult InitText(nStrView str, nLen lStrlen) = 0;

	nResult InitText(nStrView str)
	{
		return InitText(str, str.size());
	}

	template <size_t n>
	nResult InitText(const nTChar (&str)[n])
	{
		return InitText(str, n - 1);
	}

	nResult InitText(nString const& str)
	{
		return InitText(str, str.size());
	}

	/// @brief	使用颜色绘制字体
	/// @param[in]	pGraphic	图元渲染器
	/// @param[in]	str			要绘制的字符串
	/// @param[in]	x			要绘制到的x坐标
	/// @param[in]	y			要绘制到的y坐标
	/// @param[in]	scale		缩放系数
	/// @param[in]	color		字体颜色
	virtual nResult PrintFont(n2dGraphics2D* pGraphic, nStrView str, nFloat x, nFloat y, nFloat scale, natVec3<> const& color) = 0;

	/// @brief	使用纹理绘制字体
	/// @param[in]	pGraphic	图元渲染器
	/// @param[in]	str			要绘制的字符串
	/// @param[in]	x			要绘制到的x坐标
	/// @param[in]	y			要绘制到的y坐标
	/// @param[in]	scale		缩放系数
	/// @param[in]	pTexture	字体纹理
	virtual nResult PrintFont(n2dGraphics2D* pGraphic, nStrView str, nFloat x, nFloat y, nFloat scale, n2dTexture* pTexture) = 0;
};
