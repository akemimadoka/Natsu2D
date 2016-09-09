////////////////////////////////////////////////////////////////////////////////
///	@file	n2dEngineImpl.h
///	@brief	Natsu2D�������
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../n2dEngine.h"
#include "n2dWindowImpl.h"
#include "n2dVirtualFileSystemImpl.h"
#include "n2dSchemaImpl.h"
#include <natEvent.h>
#include <natMultiThread.h>
#include <natType.h>
#include <natLog.h>

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
	///	@brief	�Զ��崰����Ϣ
	enum WindowMessage
	{
		WM_TOGGLEFULLSCREEN = WM_USER + 1	///< @brief �л�ȫ��
	};

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
		ResultType ThreadJob() override;

	private:
		n2dEngineImpl* m_pEngine;
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
		ResultType ThreadJob() override;

	private:
		n2dEngineImpl* m_pEngine;
		nuInt m_FPSLimit;
		DWORD m_MainThreadID;

		nBool m_bAlreadyMadeCurrent;
	};

	///	@brief	GLӦ�ù��캯��
	///	@param[in]	lpClassname	��������
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

	n2dRenderDevice* GetRenderDevice() override;
	n2dSoundSys* GetSoundSys() override;

	n2dEngineEventListener* GetListener() override
	{ return m_pListener; }

	ThreadMode GetThreadMode() const override;
	HINSTANCE GetHInstance() const override;

	natLog& GetLogger() override;
	natEventBus& GetEventBus() override;
	natThreadPool& GetThreadPool() override;
	n2dSchemaFactory& GetSchemaFactory() override;
	n2dVirtualFileSystem& GetVirtualFileSystem() override;

	///	@brief	ע�ᴰ����Ϣ������
	///	@param[in]	func		������Ϣ������
	///	@param[in]	priority	����Ϣ�����������ȼ�
	///	@note	��������һ������WndMsgEvent&����ʹ��WndMsgEvent::getData������ô�����Ϣ
	///	@see	Msgdata
	///	@see	n2dEngine::WndMsgEvent
	void AddMessageHandler(natEventBus::EventListenerDelegate func, Priority::Priority priority) override;

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
	std::atomic<nBool>		m_IsProgramLooping;
	nBool					m_IsVisible;
	nBool					m_ResizeDraw;
	HINSTANCE				m_hInstance;
	ThreadMode				m_ThreadMode;

	natRefPointer<n2dRenderDevice>		m_pRenderer;
	natRefPointer<n2dSoundSys>			m_pSoundSys;

	n2dVirtualFileSystemImpl			m_VirtualFileSystem;
	n2dSchemaFactoryImpl				m_SchemaFactory;

	///	@brief	��ǰ����״��
	///	@note	������ǰ���ڼ���ʱ��Ч
	n2dKeyState				m_Keys;

	natEventBus				m_EventBus;
	natLog					m_Logger;
	natThreadPool			m_ThreadPool;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

///	@}
