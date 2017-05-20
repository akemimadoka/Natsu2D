////////////////////////////////////////////////////////////////////////////////
///	@file	n2dEngine.h
///	@brief	Natsu2D引擎相关
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <Windows.h>
#include <natEvent.h>
#include <natException.h>
#include "n2dInterface.h"
#include "n2dWindow.h"
#include "n2dSoundSys.h"
#include "n2dVirtualFileSystem.h"

#ifdef N2DEXPORT
#	define N2DFUNC __declspec(dllexport)
#else
#	define N2DFUNC __declspec(dllimport)
#endif // N2DEXPORT
#define N2DOBJECT extern N2DFUNC

struct n2dRenderDevice;
namespace NatsuLib
{
	class natStopWatch;
	class natLog;
	class natThreadPool;
}

///	@brief	全局变量
namespace n2dGlobal
{
	///	@brief	异常事件
	///	@note	data为natException，不可取消
	class natExceptionEvent
		: public natEventBase
	{
	public:
		explicit natExceptionEvent(natException const& ex)
			: m_Exception(ex)
		{
		}

		natException const& GetData() const noexcept
		{
			return m_Exception;
		}

	private:
		natException const& m_Exception;
	};
}

////////////////////////////////////////////////////////////////////////////////
///	@brief	FPS控制器
////////////////////////////////////////////////////////////////////////////////
struct n2dFPSController
	: n2dInterface
{
	///	@brief	更新
	virtual nDouble Update(natStopWatch& watch) = 0;

	///	@brief	获得FPS限制
	virtual nuInt GetFPSLimit() const = 0;
	///	@brief	设置FPS限制
	virtual void SetFPSLimit(nuInt FPSLimit) = 0;

	///	@brief	获得当前FPS
	virtual nDouble GetFPS() const = 0;

	///	@brief	获得总共帧数
	virtual nuInt GetTotalFrame() const = 0;
	///	@brief	获得总共时间
	virtual nDouble GetTotalTime() const = 0;

	///	@brief	获得平均FPS
	virtual nDouble GetAvgFPS() const = 0;
	///	@brief	获得最大FPS
	virtual nDouble GetMaxFPS() const = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	键盘类实现，表示键盘的按键按下情况
////////////////////////////////////////////////////////////////////////////////
class n2dKeyState final
{
public:
	n2dKeyState() : m_KeyDown{ false } {};

	///	@brief	清除当前状态
	void Clear() { memset(m_KeyDown, 0, MAX_KEYS); };
	///	@brief	获得按键状态
	nBool IsPressed(nuInt key) const { return key < MAX_KEYS ? m_KeyDown[key] : false; };
	///	@brief	设置按键按下
	void SetPressed(nuInt key) { if (key < MAX_KEYS) m_KeyDown[key] = true; };
	///	@brief	设置按键释放
	void SetReleased(nuInt key) { if (key < MAX_KEYS) m_KeyDown[key] = false; };
private:
	enum : nuInt
	{
		MAX_KEYS = 256u		///< @brief	最大按键数
	};
	nBool m_KeyDown[MAX_KEYS];
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	EventMSG使用的窗口消息结构体
////////////////////////////////////////////////////////////////////////////////
struct Msgdata
{
	HWND hWnd;		///< @brief	窗口句柄
	UINT uMsg;		///< @brief	信息类型
	WPARAM wParam;	///< @brief	wParam
	LPARAM lParam;	///< @brief	lParam
	LRESULT result;	///< @brief	处理结果
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	Natsu2D引擎事件监听器
///	@note	通过继承此类并重载方法以实现信息回调
////////////////////////////////////////////////////////////////////////////////
struct n2dEngineEventListener
{
	virtual ~n2dEngineEventListener() = default;

	///	@brief	引擎初始化
	///	@note	在窗口初始化之前触发，仅触发一次
	virtual nBool EngineInit() = 0;
	///	@brief	引擎反初始化
	///	@note	在引擎停止主循环且窗口反初始化完成后触发，仅触发一次
	virtual void EngineUninit() = 0;
	///	@brief	初始化函数
	///	@note	每次窗口初始化时被调用，包括切换全屏
	virtual nBool WindowInit() = 0;
	///	@brief	反初始化函数
	///	@note	每次窗口反初始化时被调用，包括切换全屏
	virtual void WindowUninit() = 0;
	///	@brief	更新函数
	///	@note	在此实现非绘画的更新内容
	///	@param[in]		dElapsedTime	距离上次更新的时间
	///	@param[in]		pFPSController	FPS控制器
	virtual void Update(nDouble dElapsedTime, n2dFPSController* pFPSController) = 0;
	///	@brief	绘画函数
	///	@note	在此实现绘画的更新内容
	///	@param[in]		dElapsedTime	距离上次更新的时间
	///	@param[in]		pFPSController	FPS控制器
	///	@param[in]		pRenderDevice	渲染设备
	virtual void Render(nDouble dElapsedTime, n2dFPSController* pFPSController, n2dRenderDevice* pRenderDevice) = 0;
};

struct n2dEngine
	: n2dInterface
{
	///	@brief	线程模式
	enum class ThreadMode
	{
		SingleThread,	///< @brief	 单线程模式
		MultiThread		///< @brief	 多线程模式
	};

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	窗口消息事件
	////////////////////////////////////////////////////////////////////////////////
	class WndMsgEvent
		: public natEventBase
	{
	public:
		WndMsgEvent(n2dEngine* pEngine, Msgdata& Msg)
			: m_pEngine(pEngine), m_Msg(Msg)
		{
		}

		bool CanCancel() const noexcept override
		{
			return true;
		}

		Msgdata& GetMsg() const noexcept
		{
			return m_Msg;
		}

		///	@brief	获得关联的引擎
		n2dEngine* GetEngine() const noexcept
		{
			return m_pEngine;
		}
	private:
		n2dEngine* m_pEngine;
		Msgdata& m_Msg;
	};

	///	@brief	切换全屏
	virtual void ToggleFullscreen() = 0;
	///	@brief	结束应用程序
	virtual void TerminateApplication() = 0;

	///	@brief	显示/隐藏窗口
	///	@param[in]	show	是否显示窗口
	virtual void Show(nBool show = true) = 0;
	///	@brief	交换缓冲区
	virtual void SwapBuffers() = 0;
	///	@brief	获得窗口
	virtual natRefPointer<n2dWindow> GetWindow() = 0;
	///	@brief	获得渲染设备
	virtual natRefPointer<n2dRenderDevice> GetRenderDevice() = 0;
	///	@brief	获得声音系统
	virtual natRefPointer<n2dSoundSys> GetSoundSys() = 0;
	///	@brief	获得关联的引擎监听器
	virtual n2dEngineEventListener* GetListener() = 0;

	///	@brief	获得线程模式
	virtual ThreadMode GetThreadMode() const = 0;
	///	@brief	获得实例句柄
	virtual HINSTANCE GetHInstance() const = 0;
	///	@brief	获得日志记录器
	virtual natLog& GetLogger() = 0;
	///	@brief	获得事件总线
	virtual natEventBus& GetEventBus() = 0;
	///	@brief	获得线程池
	virtual natThreadPool& GetThreadPool() = 0;

	///	@brief	注册窗口消息处理函数
	///	@param[in]	func		窗口消息处理函数
	///	@param[in]	priority	该消息处理函数的优先级
	///	@note	函数接受一个参数natEventBase&，请转换为WndMsgEvent&并使用GetMsg方法获得窗口消息
	///	@see	Msgdata
	///	@see	n2dEngine::WndMsgEvent
	virtual void AddMessageHandler(natEventBus::EventListenerDelegate func, Priority::Priority priority = Priority::Normal) = 0;

	///	@brief	获得按键的状态
	///	@param[in]	Key		按键键值
	///	@return	按键是否按下
	virtual nBool IsKeyPressed(nuInt Key) const = 0;

	///	@brief	应用程序主循环
	///	@param[in]	title	窗口名
	///	@param[in]	FPS		渲染和更新FPS
	///	@note	将来可能分离渲染及更新FPS
	virtual void MainLoop(nStrView title, nuInt FPS) = 0;
};