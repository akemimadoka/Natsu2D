////////////////////////////////////////////////////////////////////////////////
///	@file	n2dEngine.h
///	@brief	Natsu2D�������
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

///	@brief	ȫ�ֱ���
namespace n2dGlobal
{
	///	@brief	�쳣�¼�
	///	@note	dataΪnatException������ȡ��
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
///	@brief	FPS������
////////////////////////////////////////////////////////////////////////////////
struct n2dFPSController
	: n2dInterface
{
	///	@brief	����
	virtual nDouble Update(natStopWatch& watch) = 0;

	///	@brief	���FPS����
	virtual nuInt GetFPSLimit() const = 0;
	///	@brief	����FPS����
	virtual void SetFPSLimit(nuInt FPSLimit) = 0;

	///	@brief	��õ�ǰFPS
	virtual nDouble GetFPS() const = 0;

	///	@brief	����ܹ�֡��
	virtual nuInt GetTotalFrame() const = 0;
	///	@brief	����ܹ�ʱ��
	virtual nDouble GetTotalTime() const = 0;

	///	@brief	���ƽ��FPS
	virtual nDouble GetAvgFPS() const = 0;
	///	@brief	������FPS
	virtual nDouble GetMaxFPS() const = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	������ʵ�֣���ʾ���̵İ����������
////////////////////////////////////////////////////////////////////////////////
class n2dKeyState final
{
public:
	n2dKeyState() : m_KeyDown{ false } {};

	///	@brief	�����ǰ״̬
	void Clear() { memset(m_KeyDown, 0, MAX_KEYS); };
	///	@brief	��ð���״̬
	nBool IsPressed(nuInt key) const { return key < MAX_KEYS ? m_KeyDown[key] : false; };
	///	@brief	���ð�������
	void SetPressed(nuInt key) { if (key < MAX_KEYS) m_KeyDown[key] = true; };
	///	@brief	���ð����ͷ�
	void SetReleased(nuInt key) { if (key < MAX_KEYS) m_KeyDown[key] = false; };
private:
	enum : nuInt
	{
		MAX_KEYS = 256u		///< @brief	��󰴼���
	};
	nBool m_KeyDown[MAX_KEYS];
};

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
	///	@param[in]		pFPSController	FPS������
	virtual void Update(nDouble dElapsedTime, n2dFPSController* pFPSController) = 0;
	///	@brief	�滭����
	///	@note	�ڴ�ʵ�ֻ滭�ĸ�������
	///	@param[in]		dElapsedTime	�����ϴθ��µ�ʱ��
	///	@param[in]		pFPSController	FPS������
	///	@param[in]		pRenderDevice	��Ⱦ�豸
	virtual void Render(nDouble dElapsedTime, n2dFPSController* pFPSController, n2dRenderDevice* pRenderDevice) = 0;
};

struct n2dEngine
	: n2dInterface
{
	///	@brief	�߳�ģʽ
	enum class ThreadMode
	{
		SingleThread,	///< @brief	 ���߳�ģʽ
		MultiThread		///< @brief	 ���߳�ģʽ
	};

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	������Ϣ�¼�
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

		///	@brief	��ù���������
		n2dEngine* GetEngine() const noexcept
		{
			return m_pEngine;
		}
	private:
		n2dEngine* m_pEngine;
		Msgdata& m_Msg;
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
	virtual natRefPointer<n2dWindow> GetWindow() = 0;
	///	@brief	�����Ⱦ�豸
	virtual natRefPointer<n2dRenderDevice> GetRenderDevice() = 0;
	///	@brief	�������ϵͳ
	virtual natRefPointer<n2dSoundSys> GetSoundSys() = 0;
	///	@brief	��ù��������������
	virtual n2dEngineEventListener* GetListener() = 0;

	///	@brief	����߳�ģʽ
	virtual ThreadMode GetThreadMode() const = 0;
	///	@brief	���ʵ�����
	virtual HINSTANCE GetHInstance() const = 0;
	///	@brief	�����־��¼��
	virtual natLog& GetLogger() = 0;
	///	@brief	����¼�����
	virtual natEventBus& GetEventBus() = 0;
	///	@brief	����̳߳�
	virtual natThreadPool& GetThreadPool() = 0;

	///	@brief	ע�ᴰ����Ϣ������
	///	@param[in]	func		������Ϣ������
	///	@param[in]	priority	����Ϣ�����������ȼ�
	///	@note	��������һ������natEventBase&����ת��ΪWndMsgEvent&��ʹ��GetMsg������ô�����Ϣ
	///	@see	Msgdata
	///	@see	n2dEngine::WndMsgEvent
	virtual void AddMessageHandler(natEventBus::EventListenerDelegate func, Priority::Priority priority = Priority::Normal) = 0;

	///	@brief	��ð�����״̬
	///	@param[in]	Key		������ֵ
	///	@return	�����Ƿ���
	virtual nBool IsKeyPressed(nuInt Key) const = 0;

	///	@brief	Ӧ�ó�����ѭ��
	///	@param[in]	title	������
	///	@param[in]	FPS		��Ⱦ�͸���FPS
	///	@note	�������ܷ�����Ⱦ������FPS
	virtual void MainLoop(nStrView title, nuInt FPS) = 0;
};