#pragma once
#include "n2dInterface.h"
#include <Windows.h>

class n2dEngineImpl;

struct n2dWindow
	: n2dInterface
{
	///	@brief	创建窗口
	///	@note	需使用Show方法显示窗口
	///	@param[in]	title		窗口标题
	///	@param[in]	classname	窗口类名
	///	@param[in]	hInstance	实例句柄
	///	@param[in]	lpParam		创建参数
	virtual nBool Create(nStrView lpTitle, nStrView lpClassname, HINSTANCE hInstance, LPVOID lpParam) = 0;

	///	@brief	显示/隐藏窗口
	///	@param[in]	show	是否显示窗口
	virtual void Show(nBool show = true) = 0;
	///	@brief	销毁窗口
	virtual void Destroy() = 0;
	///	@brief	改变屏幕设置
	///	@note	仅用于切换全屏
	virtual nBool ChangeScreenSetting() = 0;
	///	@brief	修改gl设置以适应当前屏幕
	virtual void ReshapeGL() = 0;
	///	@brief	交换缓冲区
	virtual void SwapBuffers() = 0;

	///	@brief	设置X坐标
	///	@note	仅窗口模式有效，全屏模式下不影响任何设置
	///	@param[in]	X	要设置的坐标
	virtual void SetPosX(nuInt X) = 0;
	///	@brief	获得X坐标
	///	@note	仅窗口模式有效，全屏模式下始终返回0
	virtual nuInt GetPosX() = 0;

	///	@brief	设置Y坐标
	///	@note	仅窗口模式有效，全屏模式下不影响任何设置
	///	@param[in]	Y	要设置的坐标
	virtual void SetPosY(nuInt Y) = 0;
	///	@brief	获得Y坐标
	///	@note	仅窗口模式有效，全屏模式下始终返回0
	virtual nuInt GetPosY() = 0;

	///	@brief	设置窗口/全屏模式下宽度
	///	@note	仅影响当前模式下设置
	///	@param[in]	Width	要设置的宽度
	virtual void SetWidth(nuInt Width) = 0;
	///	@brief	获得窗口/全屏模式下宽度
	///	@note	仅可获得当前模式下设置
	virtual nuInt GetWidth() = 0;

	///	@brief	设置窗口/全屏模式下高度
	///	@note	仅影响当前模式下设置
	///	@param[in]	Height	要设置的高度
	virtual void SetHeight(nuInt Height) = 0;
	///	@brief	获得窗口/全屏模式下高度
	///	@note	仅可获得当前模式下设置
	virtual nuInt GetHeight() = 0;

	///	@brief	设置色彩类型为增强色
	///	@note	仅在创建之前设置有效
	virtual void SetHiColor() = 0;
	///	@brief	设置色彩类型为真彩色
	///	@note	仅在创建之前设置有效
	virtual void SetTrueColor() = 0;

	///	@brief	设置全屏模式
	///	@param[in]	mode	是否全屏模式
	virtual void SetFullScreen(nBool mode) = 0;
	///	@brief	获得当前是否为全屏模式
	virtual nBool GetFullScreen() = 0;

	///	@brief	色彩类型
	enum class colorType
	{
		HiColor,	///< @brief	增强色
		TrueColor	///< @brief	真彩色
	};

	///	@brief	设置色彩类型
	///	@param[in]	ct	要设置的色彩类型
	virtual void SetColorType(colorType ct) = 0;
	///	@brief	获得当前的色彩类型
	virtual colorType GetColorType() = 0;

	///	@brief	获得窗口句柄
	virtual HWND GetWnd() = 0;

	///	@brief	获得设备上下文
	virtual HDC GetDC() = 0;

	///	@brief	获得渲染上下文
	virtual HGLRC GetRC() = 0;

	virtual n2dEngineImpl* GetEngine() = 0;
};
