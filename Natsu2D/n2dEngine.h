#pragma once
#include <natEvent.h>
#include <natException.h>
#include "n2dInterface.h"
#include <Windows.h>
#include "n2dWindow.h"

struct n2dRenderDevice;
struct n2dFPSController;

///	@brief	ȫ�ֱ���
namespace n2dGlobal
{
	///	@brief	�쳣�¼�
	///	@note	dataΪnatException������ȡ��
	extern
#ifndef Natsu2DStatic
#	ifdef N2DEXPORT
		__declspec(dllexport)
#	else
		__declspec(dllimport)
#	endif
#endif
	natEvent<natException*> EventException;
}

////////////////////////////////////////////////////////////////////////////////
///	@brief	EventMSGʹ�õĴ�����Ϣ�ṹ��
////////////////////////////////////////////////////////////////////////////////
struct Msgdata
{
	HWND hWnd;		///< @brief	���ھ��
	UINT uMsg;		///< @brief	��Ϣ����
	WPARAM wParam;	///< @brief	wParam
	LPARAM lParam;	///< @brief	lParam
	LRESULT result;	///< @brief	������
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	Natsu2D�����¼�������
///	@note	ͨ���̳д��ಢ���ط�����ʵ����Ϣ�ص�
////////////////////////////////////////////////////////////////////////////////
struct n2dEngineEventListener
{
	virtual ~n2dEngineEventListener() = default;

	///	@brief	�����ʼ��
	///	@note	�ڴ��ڳ�ʼ��֮ǰ������������һ��
	virtual nBool EngineInit() = 0;
	///	@brief	���淴��ʼ��
	///	@note	������ֹͣ��ѭ���Ҵ��ڷ���ʼ����ɺ󴥷���������һ��
	virtual void EngineUninit() = 0;
	///	@brief	��ʼ������
	///	@note	ÿ�δ��ڳ�ʼ��ʱ�����ã������л�ȫ��
	virtual nBool WindowInit() = 0;
	///	@brief	����ʼ������
	///	@note	ÿ�δ��ڷ���ʼ��ʱ�����ã������л�ȫ��
	virtual void WindowUninit() = 0;
	///	@brief	���º���
	///	@note	�ڴ�ʵ�ַǻ滭�ĸ�������
	///	@param[in]		dElapsedTime	�����ϴθ��µ�ʱ��
	///	@param[inout]	pFPSController	FPS������
	virtual void Update(nDouble dElapsedTime, n2dFPSController* pFPSController) = 0;
	///	@brief	�滭����
	///	@note	�ڴ�ʵ�ֻ滭�ĸ�������
	///	@param[in]		ElapsedTime		�����ϴθ��µ�ʱ��
	///	@param[inout]	pFPSController	FPS������
	///	@param[inout]	pRenderDevice	��Ⱦ�豸
	virtual void Render(nDouble dElapsedTime, n2dFPSController* pFPSController, n2dRenderDevice* pRenderDevice) = 0;
};

struct n2dEngine
	: n2dInterface
{
	///	@brief	�Զ��崰����Ϣ
	enum WindowMessage
	{
		WM_TOGGLEFULLSCREEN = WM_USER + 1	///< @brief �л�ȫ��
	};

	///	@brief	�߳�ģʽ
	enum class ThreadMode
	{
		SingleThread,	///< @brief	���߳�ģʽ
		MultiThread		///< @brief	���߳�ģʽ
	};

	struct WndMsgEvent
		: natEvent<Msgdata>
	{
		typedef void(*EventHandler)(WndMsgEvent&);

		WndMsgEvent() : natEvent(true) {}
		virtual nuInt Register(EventHandler func, DWORD WndMsg, nInt priority = Priority::Normal) = 0;
		virtual nBool Unregister(nuInt HandlerIndex, DWORD WndMsg, nInt Priority) = 0;
		virtual void Unregister(DWORD WndMsg, EventHandler Handler) = 0;

		virtual nBool Activate(DWORD WndMsg, nInt PriorityLimitmin = Priority::Low, nInt PriorityLimitmax = Priority::High) = 0;
		virtual nBool Activate(DWORD WndMsg, dataType const& data, nInt PriorityLimitmin, nInt PriorityLimitmax) = 0;

		virtual nBool operator()(DWORD WndMsg, dataType const& data) = 0;

		virtual void Release() override = 0;

		virtual n2dEngine* GetEngine() = 0;
	};

	///	@brief	�л�ȫ��
	virtual void ToggleFullscreen() = 0;
	///	@brief	����Ӧ�ó���
	virtual void TerminateApplication() = 0;

	///	@brief	��ʾ/���ش���
	///	@param[in]	show	�Ƿ���ʾ����
	virtual void Show(nBool show = true) = 0;
	///	@brief	����������
	virtual void SwapBuffers() = 0;
	///	@brief	��ô���
	virtual n2dWindow* GetWindow() = 0;
	virtual n2dRenderDevice* GetRenderDevice() = 0;
	virtual n2dEngineEventListener* GetListener() = 0;

	virtual ThreadMode GetThreadMode() const = 0;
	virtual HINSTANCE GetHInstance() const = 0;

	///	@brief	ע�ᴰ����Ϣ������
	///	@param[in]	func		������Ϣ������
	///	@param[in]	WndMsg		Ҫ����Ĵ�����Ϣ
	///	@param[in]	priority	����Ϣ�����������ȼ�
	///	@note	��������һ������WndMsgEvent&����ʹ��WndMsgEvent::getData������ô�����Ϣ
	///	@see	Msgdata
	///	@see	n2dEngine::WndMsgEvent
	virtual void AddMessageHandler(WndMsgEvent::EventHandler func, DWORD WndMsg, Priority::Priority priority = Priority::Normal) = 0;

	///	@brief	��ð�����״̬
	///	@param[in]	Key		������ֵ
	///	@return	�����Ƿ���
	virtual nBool IsKeyPressed(nuInt Key) const = 0;

	///	@brief	Ӧ�ó�����ѭ��
	///	@param[in]	title	������
	///	@param[in]	FPS		��Ⱦ�͸���FPS
	///	@note	�������ܷ�����Ⱦ������FPS
	virtual void MainLoop(ncTStr title, nuInt FPS) = 0;
};