////////////////////////////////////////////////////////////////////////////////
///	@file	n2dEngine.h
///	@brief	Natsu2D�������
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
///	@addtogroup	Natsu2D����
///	@brief		Natsu2D���沿��
///	@{

////////////////////////////////////////////////////////////////////////////////
///	@brief	n2d����
////////////////////////////////////////////////////////////////////////////////
class n2dEngineImpl
	: public natRefObjImpl<n2dEngine>
{
public:

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	�����߳�
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
	///	@brief	��Ⱦ�߳�
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
	///	@brief	������Ϣ�¼�
	///	@note	���Խ�����ָ�����¼��Ĵ�����
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

	///	@brief	GLӦ�ù��캯��
	///	@param[in]	classname	��������
	///	@param[in]	hInstance	ʵ�����
	///	@param[in]	threadMode	�߳�ģʽ
	///	@note	���������GL���ڹ��캯��
	///	@see	n2dWindowImpl::n2dWindowImpl
	n2dEngineImpl(ncTStr lpClassname, nuInt x, nuInt y, nuInt WindowWidth, nuInt WindowHeight, nuInt ScreenWidth, nuInt ScreenHeight, nuInt BitsPerPixel, nBool fullscreen, HINSTANCE hInstance, ThreadMode threadMode = ThreadMode::SingleThread, n2dEngineEventListener* pListener = nullptr);
	virtual ~n2dEngineImpl();

	///	@brief	�л�ȫ��
	void ToggleFullscreen() override;
	///	@brief	����Ӧ�ó���
	void TerminateApplication() override;
	///	@brief	�趨�Ƿ��ڸı䴰�ڴ�Сʱ�ػ�
	///	@note	��ʧЧ
	void ResizeDraw(nBool enable);

	///	@brief	��ʾ/���ش���
	///	@param[in]	show	�Ƿ���ʾ����
	void Show(nBool show = true) override;
	///	@brief	����������
	void SwapBuffers() override
	{ m_Window.SwapBuffers();	}
	///	@brief	��ô���
	n2dWindow* GetWindow() override
	{ return &m_Window; }
	n2dRenderDevice* GetRenderDevice() override
	{ return m_pRenderer; }
	n2dEngineEventListener* GetListener() override
	{ return m_pListener; }

	ThreadMode GetThreadMode() const override;
	HINSTANCE GetHInstance() const override;

	///	@brief	ע�ᴰ����Ϣ������
	///	@param[in]	func		������Ϣ������
	///	@param[in]	WndMsg		Ҫ����Ĵ�����Ϣ
	///	@param[in]	priority	����Ϣ�����������ȼ�
	///	@note	��������һ������WndMsgEvent&����ʹ��WndMsgEvent::getData������ô�����Ϣ
	///	@see	Msgdata
	///	@see	n2dEngine::WndMsgEvent
	void AddMessageHandler(WndMsgEvent::EventHandler func, DWORD WndMsg, Priority::Priority priority) override;

	///	@brief	��ð�����״̬
	///	@param[in]	Key		������ֵ
	///	@return	�����Ƿ���
	nBool IsKeyPressed(nuInt Key) const override;

	///	@brief	Ӧ�ó�����ѭ��
	///	@param[in]	title	������
	///	@param[in]	FPS		��Ⱦ�͸���FPS
	///	@note	�������ܷ�����Ⱦ������FPS
	void MainLoop(ncTStr title, nuInt FPS) override;

	///	@brief	Ӧ�ó�����Ϣ������
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

	///	@brief	��ǰ����״��
	///	@note	������ǰ���ڼ���ʱ��Ч
	n2dKeyState				m_Keys;

	///	@brief	����¼����Ա�ȡ������ȡ���󽫲���ʹ��Ĭ�ϵĴ�������ֵΪMsgdata::result��dataΪ����MSG��Ϣ�Ľṹ��Msgdata
	///	@see	Msgdata
	WndMsgEventImpl				m_EventMSG;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

///	@}