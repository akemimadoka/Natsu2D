#pragma once
#include <Windows.h>
#include <natEvent.h>
#include <natException.h>
#include <natLog.h>
#include "n2dInterface.h"
#include "n2dWindow.h"
#include "n2dSoundSys.h"

//	����Natsu2D�Ѿ���֧�����ɾ�̬�⣬����궨�弴������֧��
#ifdef Natsu2DStatic
#	ifdef N2DEXPORT
#		define N2DFUNC
#	else
#		define N2DFUNC extern
#	endif // N2DEXPORT
#	define N2DOBJECT extern
#else
#	ifdef N2DEXPORT
#		define N2DFUNC __declspec(dllexport)
#	else
#		define N2DFUNC __declspec(dllimport)
#	endif // N2DEXPORT
#	define N2DOBJECT extern N2DFUNC
#endif // Natsu2DStatic

struct n2dRenderDevice;
class natStopWatch;

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

	extern "C"
	{
		///	@brief	������־�ļ��洢Ŀ¼
		///	@note	���ڵ�һ��ʹ����־֮ǰ��Ч
		void N2DFUNC SetLogFile(ncTStr Path);
		///	@brief	��¼��Ϣ
		void N2DFUNC LogMsg(ncTStr Str);
		///	@brief	��¼����
		void N2DFUNC LogWarn(ncTStr Str);
		///	@brief	��¼����
		void N2DFUNC LogErr(ncTStr Str);
		///	@brief	ע����־�����¼�������
		void N2DFUNC RegisterLogUpdateEventFunc(natEvent<ncTStr>::EventHandle func);
	}
}

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

	///	@brief	��ǰFPS�����ԣ�
	__declspec(property(get = GetFPS))
		nDouble FPS;

	///	@brief	FPS���ƣ����ԣ�
	__declspec(property(get = GetFPSLimit, put = SetFPSLimit))
		nuInt FPSLimit;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	������ʵ�֣���ʾ���̵İ����������
////////////////////////////////////////////////////////////////////////////////
class n2dKeyState final
{
public:
	n2dKeyState() {};
	~n2dKeyState() = default;

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

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	������Ϣ�¼�
	///	@note	���Խ�����ָ�����¼��Ĵ�����
	////////////////////////////////////////////////////////////////////////////////
	struct WndMsgEvent
		: natEvent<Msgdata>
	{
		typedef void(*EventHandler)(WndMsgEvent&);

		WndMsgEvent() : natEvent(true) {}
		
		///	@brief	ע�ᴦ����
		virtual nuInt Register(EventHandler func, DWORD WndMsg, nInt priority = Priority::Normal) = 0;
		///	@brief	��ע�ᴦ����
		virtual void Unregister(DWORD WndMsg, EventHandler Handler) = 0;

		///	@brief	�����¼�
		virtual nBool Activate(DWORD WndMsg, nInt PriorityLimitmin = Priority::Low, nInt PriorityLimitmax = Priority::High) = 0;
		///	@brief	�����¼�
		virtual nBool Activate(DWORD WndMsg, dataType const& data, nInt PriorityLimitmin, nInt PriorityLimitmax) = 0;

		///	@brief	�����¼�
		virtual nBool operator()(DWORD WndMsg, dataType const& data) = 0;

		///	@brief	�ͷ��¼�
		virtual void Release() override = 0;
		
		///	@brief	��ù���������
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
	///	@brief	�����Ⱦ�豸
	virtual n2dRenderDevice* GetRenderDevice() = 0;
	///	@brief	�������ϵͳ
	virtual n2dSoundSys* GetSoundSys() = 0;
	///	@brief	��ù��������������
	virtual n2dEngineEventListener* GetListener() = 0;

	///	@brief	����߳�ģʽ
	virtual ThreadMode GetThreadMode() const = 0;
	///	@brief	���ʵ�����
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