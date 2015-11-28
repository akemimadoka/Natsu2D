////////////////////////////////////////////////////////////////////////////////
///	@file	n2dEngine.h
///	@brief	Natsu2D引擎相关
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\n2dEngine.h"

#include <Windows.h>
#include <unordered_map>
#include "n2dWindowImpl.h"
#include "..\n2dKeyState.h"
#include <natEvent.h>
#include <natMultiThread.h>
#include <natException.h>
#include <natType.h>

struct n2dFPSController;
struct n2dRenderDevice;
struct n2dEngine;

////////////////////////////////////////////////////////////////////////////////
///	@addtogroup	Natsu2D引擎
///	@brief		Natsu2D引擎部分
///	@{

////////////////////////////////////////////////////////////////////////////////
///	@brief	n2d引擎
////////////////////////////////////////////////////////////////////////////////
class n2dEngineImpl
	: public natRefObjImpl<n2dEngine>
{
public:

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	更新线程
	////////////////////////////////////////////////////////////////////////////////
	class UpdateThread final
		: public natThread
	{
	public:
		UpdateThread(n2dEngineImpl* pGLApp, nuInt FPSLimit);
		~UpdateThread() = default;

	protected:
		nuInt ThreadJob() override;

	private:
		n2dEngineImpl* m_Thisapp;
		nuInt m_FPSLimit;
		DWORD m_MainThreadID;
	};

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	渲染线程
	////////////////////////////////////////////////////////////////////////////////
	class RenderThread final
		: public natThread
	{
	public:
		RenderThread(n2dEngineImpl* pGLApp, nuInt FPSLimit);
		~RenderThread() = default;

	protected:
		nuInt ThreadJob() override;

	private:
		n2dEngineImpl* m_Thisapp;
		nuInt m_FPSLimit;
		DWORD m_MainThreadID;

		nBool m_bAlreadyMadeCurrent;
	};

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	窗口消息事件
	///	@note	可以仅触发指定的事件的处理函数
	////////////////////////////////////////////////////////////////////////////////
	class WndMsgEventImpl final
		: public WndMsgEvent
	{
	public:

		explicit WndMsgEventImpl(n2dEngineImpl* App);
		~WndMsgEventImpl();

		nuInt Register(EventHandler func, DWORD WndMsg, nInt priority = Priority::Normal) override;
		nBool Unregister(nuInt HandlerIndex, DWORD WndMsg, nInt Priority) override;
		void Unregister(DWORD WndMsg, EventHandler Handler) override;

		nBool Activate(DWORD WndMsg, nInt PriorityLimitmin = Priority::Low, nInt PriorityLimitmax = Priority::High) override;
		nBool Activate(DWORD WndMsg, dataType const& data, nInt PriorityLimitmin, nInt PriorityLimitmax) override;

		nBool operator()(DWORD WndMsg, dataType const& data) override;

		void Release() override;

		n2dEngineImpl* GetEngine() override;
	private:
		std::unordered_map<DWORD, std::map<nInt, std::vector<EventHandler>>> m_EventHandler;
		n2dEngineImpl* m_pEngine;
	};

	///	@brief	GL应用构造函数
	///	@param[in]	classname	窗口类名
	///	@param[in]	hInstance	实例句柄
	///	@param[in]	threadMode	线程模式
	///	@note	其余参数见GL窗口构造函数
	///	@see	n2dWindowImpl::n2dWindowImpl
	n2dEngineImpl(ncTStr lpClassname, nuInt x, nuInt y, nuInt WindowWidth, nuInt WindowHeight, nuInt ScreenWidth, nuInt ScreenHeight, nuInt BitsPerPixel, nBool fullscreen, HINSTANCE hInstance, ThreadMode threadMode = ThreadMode::SingleThread, n2dEngineEventListener* pListener = nullptr);
	virtual ~n2dEngineImpl();

	///	@brief	切换全屏
	void ToggleFullscreen() override;
	///	@brief	结束应用程序
	void TerminateApplication() override;
	///	@brief	设定是否在改变窗口大小时重绘
	///	@note	已失效
	void ResizeDraw(nBool enable);

	///	@brief	显示/隐藏窗口
	///	@param[in]	show	是否显示窗口
	void Show(nBool show = true) override;
	///	@brief	交换缓冲区
	void SwapBuffers() override
	{ m_Window.SwapBuffers();	}
	///	@brief	获得窗口
	n2dWindow* GetWindow() override
	{ return &m_Window; }
	n2dRenderDevice* GetRenderDevice() override
	{ return m_pRenderer; }
	n2dEngineEventListener* GetListener() override
	{ return m_pListener; }

	ThreadMode GetThreadMode() const override;
	HINSTANCE GetHInstance() const override;

	///	@brief	注册窗口消息处理函数
	///	@param[in]	func		窗口消息处理函数
	///	@param[in]	WndMsg		要处理的窗口消息
	///	@param[in]	priority	该消息处理函数的优先级
	///	@note	函数接受一个参数WndMsgEvent&，请使用WndMsgEvent::getData方法获得窗口消息
	///	@see	Msgdata
	///	@see	n2dEngine::WndMsgEvent
	void AddMessageHandler(WndMsgEvent::EventHandler func, DWORD WndMsg, Priority::Priority priority) override;

	///	@brief	获得按键的状态
	///	@param[in]	Key		按键键值
	///	@return	按键是否按下
	nBool IsKeyPressed(nuInt Key) const override;

	///	@brief	应用程序主循环
	///	@param[in]	title	窗口名
	///	@param[in]	FPS		渲染和更新FPS
	///	@note	将来可能分离渲染及更新FPS
	void MainLoop(ncTStr title, nuInt FPS) override;

	///	@brief	应用程序消息处理函数
	LRESULT Message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void CommonInit();
	void SingleThreadMainLoop(ncTStr title, nuInt FPS);
	void MultiThreadMainLoop(ncTStr title, nuInt FPS);

	n2dEngineEventListener*	m_pListener;
	n2dWindowImpl			m_Window;
	natCriticalSection		m_Section;
	ncTStr					m_ClassName;
	nBool					m_IsProgramLooping;
	nBool					m_IsVisible;
	nBool					m_ResizeDraw;
	HINSTANCE				m_hInstance;
	ThreadMode				m_ThreadMode;

	n2dRenderDevice*		m_pRenderer;

	///	@brief	当前按键状况
	///	@note	仅当当前窗口激活时有效
	n2dKeyState				m_Keys;

	///	@brief	这个事件可以被取消，被取消后将不会使用默认的处理，返回值为Msgdata::result，data为包含MSG信息的结构体Msgdata
	///	@see	Msgdata
	WndMsgEventImpl				m_EventMSG;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

///	@}