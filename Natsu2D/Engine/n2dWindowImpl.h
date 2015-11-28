////////////////////////////////////////////////////////////////////////////////
///	@file	n2dWindowImpl.h
///	@brief	Natsu2D窗口相关
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "..\n2dWindow.h"
#include <Windows.h>
#include <natType.h>

class n2dEngineImpl;

////////////////////////////////////////////////////////////////////////////////
///	@addtogroup	Natsu2D窗口
///	@brief		Natsu2D窗口部分
///	@{

////////////////////////////////////////////////////////////////////////////////
///	@brief	Natsu2D窗口类
////////////////////////////////////////////////////////////////////////////////
class n2dWindowImpl
	: public natRefObjImpl<n2dWindow>
{
public:
	///	@brief	GL窗口构造函数
	///	@param[in]	pEngine			Natsu2D引擎
	///	@param[in]	X				窗口模式下x坐标
	///	@param[in]	Y				窗口模式下y坐标
	///	@param[in]	WindowWidth		窗口宽度
	///	@param[in]	WindowHeight	窗口高度
	///	@param[in]	ScreenWidth		全屏宽度
	///	@param[in]	ScreenHeight	全屏高度
	///	@param[in]	BitsPerPixel	色彩深度
	///	@param[in]	fullscreen		创建时是否为全屏
	///	@note	色彩深度目前仅支持16位和32位
	explicit n2dWindowImpl(n2dEngineImpl* pEngine, nuInt X = 0u, nuInt Y = 0u, nuInt WindowWidth = 0u, nuInt WindowHeight = 0u, nuInt ScreenWidth = 0u, nuInt ScreenHeight = 0u, nuInt BitsPerPixel = 0u, nBool fullscreen = false);
	virtual ~n2dWindowImpl();

	///	@brief	创建窗口
	///	@note	需使用Show方法显示窗口
	///	@param[in]	title		窗口标题
	///	@param[in]	classname	窗口类名
	///	@param[in]	hInstance	实例句柄
	///	@param[in]	lpParam		创建参数
	nBool Create(ncTStr lpTitle, ncTStr lpClassname, HINSTANCE hInstance, LPVOID lpParam) override;

	///	@brief	显示/隐藏窗口
	///	@param[in]	show	是否显示窗口
	void Show(nBool show = true) override;
	///	@brief	销毁窗口
	void Destroy() override;
	///	@brief	改变屏幕设置
	///	@note	仅用于切换全屏
	nBool ChangeScreenSetting() override;
	///	@brief	修改gl设置以适应当前屏幕
	void ReshapeGL() override;
	///	@brief	交换缓冲区
	void SwapBuffers() override;

	///	@brief	设置X坐标
	///	@note	仅窗口模式有效，全屏模式下不影响任何设置
	///	@param[in]	X	要设置的坐标
	void SetPosX(nuInt X) override;
	///	@brief	获得X坐标
	///	@note	仅窗口模式有效，全屏模式下始终返回0u
	nuInt GetPosX() override;

	///	@brief	设置Y坐标
	///	@note	仅窗口模式有效，全屏模式下不影响任何设置
	///	@param[in]	Y	要设置的坐标
	void SetPosY(nuInt Y) override;
	///	@brief	获得Y坐标
	///	@note	仅窗口模式有效，全屏模式下始终返回0u
	nuInt GetPosY() override;

	///	@brief	设置窗口/全屏模式下宽度
	///	@note	仅影响当前模式下设置
	///	@param[in]	Width	要设置的宽度
	void SetWidth(nuInt Width) override;
	///	@brief	获得窗口/全屏模式下宽度
	///	@note	仅可获得当前模式下设置
	nuInt GetWidth() override;

	///	@brief	设置窗口/全屏模式下高度
	///	@note	仅影响当前模式下设置
	///	@param[in]	Height	要设置的高度
	void SetHeight(nuInt Height) override;
	///	@brief	获得窗口/全屏模式下高度
	///	@note	仅可获得当前模式下设置
	nuInt GetHeight() override;

	///	@brief	设置色彩类型为增强色
	///	@note	仅在创建之前设置有效
	void SetHiColor() override;
	///	@brief	设置色彩类型为真彩色
	///	@note	仅在创建之前设置有效
	void SetTrueColor() override;

	///	@brief	设置全屏模式
	///	@param[in]	mode	是否全屏模式
	void SetFullScreen(nBool mode) override;
	///	@brief	获得当前是否为全屏模式
	nBool GetFullScreen() override;

	///	@brief	设置色彩类型
	///	@param[in]	ct	要设置的色彩类型
	void SetColorType(colorType ct) override;
	///	@brief	获得当前的色彩类型
	colorType GetColorType() override;

	__declspec(property(put = SetColorType, get = GetColorType))
	///	@brief	色彩模式
	colorType ColorType;

	__declspec(property(put = SetPosX, get = GetPosX))
	///	@brief	X坐标
	nuInt PosX;
	__declspec(property(put = SetPosY, get = GetPosY))
	///	@brief	Y坐标
	nuInt PosY;

	__declspec(property(put = SetWidth, get = GetWidth))
	///	@brief	宽度
	nuInt Width;
	__declspec(property(put = SetHeight, get = GetHeight))
	///	@brief	高度
	nuInt Height;

	__declspec(property(put = SetFullScreen, get = GetFullScreen))
	///	@brief	是否全屏
	nBool FullScreen;

	///	@brief	获得窗口句柄
	HWND GetWnd() override;

	///	@brief	获得设备上下文
	HDC GetDC() override;

	///	@brief	获得渲染上下文
	HGLRC GetRC() override;

	n2dEngineImpl* GetEngine() override;

private:
	HWND m_hWnd;
	HDC m_hDC;
	HGLRC m_hRC;

	n2dEngineImpl* m_pEngine;

	nuInt m_WindowPosX;
	nuInt m_WindowPosY;
	nuInt m_WindowWidth;
	nuInt m_WindowHeight;
	nuInt m_ScreenWidth;
	nuInt m_ScreenHeight;
	nuInt m_BitsPerPixel;
	nBool m_IsFullScreen;
};

///	@}