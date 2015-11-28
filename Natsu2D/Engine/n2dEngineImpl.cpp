#include "n2dEngineImpl.h"
#include <natException.h>
#include <natStream.h>
#include "n2dFPSControllerImpl.h"
#include "..\RenderDevice\n2dRenderDeviceImpl.h"
#include "..\n2dCommon.h"
#include "natLog.h"
#include "natStopWatch.h"

#include "..\Natsu2D.h"
#include "..\resource.h"

#pragma warning (disable: 4996)

extern "C" nResult N2DFUNC CreateN2DEngine(
	nInt Reserved,
	ncTStr classname,
	nuInt x,
	nuInt y,
	nuInt WindowWidth,
	nuInt WindowHeight,
	nuInt ScreenWidth,
	nuInt ScreenHeight,
	nuInt BitsPerPixel,
	nBool fullscreen,
	HINSTANCE hInstance,
	n2dEngineImpl::ThreadMode threadMode,
	n2dEngineEventListener* pListener,
	n2dEngine** pOut)
{
	if (pOut == nullptr)
	{
		return -1;
	}
	*pOut = nullptr;

	if (Reserved != Reserved)
	{
		// ignored
	}

	*pOut = new n2dEngineImpl(classname, x, y, WindowWidth, WindowHeight, ScreenWidth, ScreenHeight, BitsPerPixel, fullscreen, hInstance, threadMode, pListener);

	return 0;
}

///	@brief	全局变量
namespace global
{
	///	@brief	异常事件
	///	@note	data为natException，不可取消
	natEvent<natException*> EventException(nullptr, false);

#ifndef Natsu2DStatic
	HMODULE hDll;
#endif
}

#ifndef Natsu2DStatic

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID /*lpReserved*/
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		global::hDll = hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif

void n2dEngineImpl::ToggleFullscreen()
{
	PostMessage(m_Window.GetWnd(), WM_TOGGLEFULLSCREEN, 0, 0);
}

void n2dEngineImpl::TerminateApplication()
{
	PostMessage(m_Window.GetWnd(), WM_QUIT, 0, 0);

	m_Section.Lock();
	m_IsProgramLooping = false;
	m_Section.UnLock();
}

void n2dEngineImpl::ResizeDraw(nBool enable)
{
	m_ResizeDraw = enable;
}

void n2dEngineImpl::Show(nBool show)
{
	m_IsVisible = show;
	m_Window.Show(show);
}

n2dEngineImpl::ThreadMode n2dEngineImpl::GetThreadMode() const
{
	return m_ThreadMode;
}

HINSTANCE n2dEngineImpl::GetHInstance() const
{
	return m_hInstance;
}

void n2dEngineImpl::AddMessageHandler(WndMsgEvent::EventHandler func, DWORD WndMsg, Priority::Priority priority)
{
	m_EventMSG.Register(func, WndMsg, priority);
}

n2dEngineImpl::n2dEngineImpl(ncTStr classname, nuInt x, nuInt y, nuInt WindowWidth, nuInt WindowHeight, nuInt ScreenWidth, nuInt ScreenHeight, nuInt BitsPerPixel, nBool fullscreen, HINSTANCE hInstance, ThreadMode threadMode, n2dEngineEventListener* pListener)
	: m_pListener(pListener),
	m_Window(this, x, y, WindowWidth, WindowHeight, ScreenWidth, ScreenHeight, BitsPerPixel, fullscreen),
	m_ClassName(classname),
	m_IsProgramLooping(true),
	m_IsVisible(false),
	m_ResizeDraw(false),
	m_hInstance(hInstance),
	m_ThreadMode(threadMode),
	m_pRenderer(nullptr),
	m_EventMSG(this)
{
	if (!m_pListener)
	{
		throw natException(TEXT("n2dEngineImpl::n2dEngineImpl"), TEXT("Null EventListener"));
	}
	if (!m_ClassName)
	{
		throw natException(TEXT("n2dEngineImpl::n2dEngineImpl"), TEXT("Null ClassName"));
	}

	WNDCLASSEX windowClass;
	memset(&windowClass, 0, sizeof(WNDCLASSEX));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = m_hInstance;
	windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_APPWORKSPACE);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = m_ClassName;

	if (!RegisterClassEx(&windowClass))
	{
		throw natException(_T("n2dEngineImpl::n2dEngineImpl"), _T("Failed to register window"));
	}

	m_pRenderer = new n2dRenderDeviceImpl(this);
}

n2dEngineImpl::~n2dEngineImpl()
{
	if (m_pRenderer)
	{
		delete m_pRenderer;
	}
}

nBool n2dEngineImpl::IsKeyPressed(nuInt Key) const
{
	return m_Keys.IsPressed(Key);
}

void n2dEngineImpl::MainLoop(ncTStr title, nuInt FPS)
{
	try
	{
		m_Section.Lock();
		m_IsProgramLooping = true;
		m_Section.UnLock();

		switch (m_ThreadMode)
		{
		case ThreadMode::SingleThread:
			SingleThreadMainLoop(title, FPS);
			break;
		case ThreadMode::MultiThread:
			MultiThreadMainLoop(title, FPS);
			break;
		default:
			break;
		}

		UnregisterClass(m_ClassName, m_hInstance);
	}
	catch (natException& ex)
	{
		natException tEx(_T("n2dEngineImpl::MainLoop"), _T("Main loop exit"), &ex);
		global::EventException(&tEx);
	}
}

LRESULT n2dEngineImpl::Message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SYSCOMMAND:
		if (m_EventMSG(uMsg, Msgdata{ hWnd, uMsg, wParam, lParam }))
		{
			return m_EventMSG.GetData().result;
		}
		switch (wParam)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
		default:
			break;
		}
		break;

	case WM_CLOSE:
		if (m_EventMSG(uMsg, Msgdata{ hWnd, uMsg, wParam, lParam }))
		{
			return m_EventMSG.GetData().result;
		}
		TerminateApplication();
		return 0;

	case WM_EXITMENULOOP:
	case WM_EXITSIZEMOVE:
		if (m_EventMSG(uMsg, Msgdata{ hWnd, uMsg, wParam, lParam }))
		{
			return m_EventMSG.GetData().result;
		}
		return 0;

	case WM_MOVE:
		if (m_EventMSG(uMsg, Msgdata{ hWnd, uMsg, wParam, lParam }))
		{
			return m_EventMSG.GetData().result;
		}
		m_Window.SetPosX(LOWORD(lParam));
		m_Window.SetPosY(HIWORD(wParam));
		return 0;

	case WM_PAINT:
		break;

	case WM_SIZING:
	{
		if (m_EventMSG(uMsg, Msgdata{ hWnd, uMsg, wParam, lParam }))
		{
			return m_EventMSG.GetData().result;
		}
		RECT *rect = reinterpret_cast<RECT *>(lParam);
		m_Window.SetWidth(rect->right - rect->left);
		m_Window.SetHeight(rect->bottom - rect->top);
		return true;
	}

	case WM_SIZE:
		if (m_EventMSG(uMsg, Msgdata{ hWnd, uMsg, wParam, lParam }))
		{
			return m_EventMSG.GetData().result;
		}
		switch (wParam)
		{
		case SIZE_MINIMIZED:
			m_IsVisible = false;
			return 0;

		case SIZE_MAXIMIZED:
		case SIZE_RESTORED:
			m_IsVisible = true;
			m_Window.SetWidth(LOWORD(lParam));
			m_Window.SetHeight(HIWORD(lParam));
			m_Window.ReshapeGL();
			return 0;
		}
		break;

	case WM_KEYDOWN:
		m_Keys.SetPressed(wParam);
		if (m_EventMSG(uMsg, Msgdata{ hWnd, uMsg, wParam, lParam }))
		{
			return m_EventMSG.GetData().result;
		}
		return 0;

	case WM_KEYUP:
		m_Keys.SetReleased(wParam);
		if (m_EventMSG(uMsg, Msgdata{ hWnd, uMsg, wParam, lParam }))
		{
			return m_EventMSG.GetData().result;
		}
		return 0;

	case WM_TOGGLEFULLSCREEN:
		m_Window.FullScreen = !m_Window.FullScreen;
		if (m_Window.FullScreen)
		{
			ShowCursor(FALSE);
		}
		else
		{
			ShowCursor(TRUE);
		}

		PostMessage(hWnd, WM_QUIT, 0, 0);
		break;

	default:
		break;
	}

	if (m_EventMSG(uMsg, Msgdata{ hWnd, uMsg, wParam, lParam }))
	{
		return m_EventMSG.GetData().result;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void n2dEngineImpl::CommonInit()
{
	global::InitGlew();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LESS);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glLoadIdentity();

#ifndef Natsu2DStatic

	std::vector<nByte> tVert = natUtil::GetResourceData(IDR_DefaultVertShader, _T("SHADER"), global::hDll);
	std::vector<nByte> tFrag = natUtil::GetResourceData(IDR_DefaultFragShader, _T("SHADER"), global::hDll);

	if (!tVert.empty() && !tFrag.empty())
	{
		natMemoryStream* pStream = new natMemoryStream(reinterpret_cast<ncData>(&tVert[0]), tVert.size(), true, true, true);
		n2dShaderWrapper* pShader = m_pRenderer->GetShaderWrapper();
		pShader->AddShader(n2dShaderWrapper::Vertex, pStream);
		pStream->SetPosition(NatSeek::Beg, 0l);
		pStream->SetSize(tFrag.size());
		pStream->WriteBytes(reinterpret_cast<ncData>(&tFrag[0]), tFrag.size());
		pShader->AddShader(n2dShaderWrapper::Fragment, pStream);
		pStream->Release();
		glUseProgram(pShader->CompileProgram());
	}

#endif
}

void n2dEngineImpl::SingleThreadMainLoop(ncTStr title, nuInt FPS)
{
	// 啪！
	SetThreadAffinityMask(GetCurrentThread(), 1);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	natStopWatch tTimer, tMsgTimer;
	double tTime, tMsgTime;
	n2dFPSControllerImpl tfpsc(FPS);

	if (!m_pListener->EngineInit())
	{
		TerminateApplication();
	}

	while (m_IsProgramLooping)
	{
		if (m_Window.Create(title, m_ClassName, m_hInstance, this))
		{
			m_Window.Show();

			CommonInit();
			
			if (!m_pListener->WindowInit())
			{
				TerminateApplication();
			}
			else
			{
				MSG msg;
				bool isMessagePumpActive = true;
				m_Keys.Clear();
				while (isMessagePumpActive)
				{
					tMsgTimer.Reset();
					if (PeekMessage(&msg, m_Window.GetWnd(), 0, 0, PM_REMOVE))
					{
						if (msg.message != WM_QUIT)
						{
							DispatchMessage(&msg);
						}
						else
						{
							isMessagePumpActive = false;
						}
					}
					tMsgTime = tMsgTimer.GetElpased();

					tTime = tfpsc.Update(tTimer) - tMsgTime;

					if (!m_IsVisible)
					{
						WaitMessage();
					}
					else
					{
						m_pListener->Update(tTime, &tfpsc);
						m_pListener->Render(tTime, &tfpsc, m_pRenderer);
						//m_Window.SwapBuffers();
					}
				}
			}

			m_pListener->WindowUninit();
			m_Window.Destroy();
		}
		else
		{
			m_Section.Lock();
			m_IsProgramLooping = false;
			m_Section.UnLock();
			throw natException(TEXT("n2dEngineImpl::SingleThreadMainLoop"), TEXT("Failed to create GL window"));
		}
	}

	m_pListener->EngineUninit();
}

void n2dEngineImpl::MultiThreadMainLoop(ncTStr title, nuInt FPS)
{
	UpdateThread updateThread(this, FPS);
	RenderThread renderThread(this, FPS);

	if (!m_pListener->EngineInit())
	{
		TerminateApplication();
	}

	while (m_IsProgramLooping)
	{
		if (m_Window.Create(title, m_ClassName, m_hInstance, this))
		{
			MSG msg;
			m_Window.Show();

			CommonInit();
			
			if (!m_pListener->WindowInit())
			{
				TerminateApplication();
			}
			else
			{
				wglMakeCurrent(NULL, NULL);
				updateThread.Resume();
				renderThread.Resume();

				bool isMessagePumpActive = true;
				m_Keys.Clear();
				while (isMessagePumpActive)
				{
					if (PeekMessage(&msg, m_Window.GetWnd(), 0, 0, PM_REMOVE))
					{
						if (msg.message != WM_QUIT)
						{
							DispatchMessage(&msg);
						}
						else
						{
							isMessagePumpActive = false;
						}
					}

					if (!m_IsVisible)
					{
						WaitMessage();
					}
				}

				if (WAIT_TIMEOUT != updateThread.Wait(10u))
					if (WAIT_TIMEOUT != renderThread.Wait(10u))
						break;

				if (PeekMessage(&msg, m_Window.GetWnd(), 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			m_pListener->WindowUninit();
			m_Window.Destroy();
		}
		else
		{
			m_Section.Lock();
			m_IsProgramLooping = false;
			m_Section.UnLock();
			throw natException(TEXT("n2dEngineImpl::MultiThreadMainLoop"), TEXT("Failed to create GL window"));
		}
	}

	updateThread.Wait();
	renderThread.Wait();

	m_pListener->EngineUninit();
}

LRESULT n2dEngineImpl::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG userdata = GetWindowLong(hWnd, GWL_USERDATA);
	try
	{
		if (userdata == NULL)
		{
			if (uMsg == WM_CREATE)
			{
				CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
				n2dEngineImpl* ptr = static_cast<n2dEngineImpl *>(cs->lpCreateParams);
				SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<LONG>(ptr));
				ptr->Show();
				return 0;
			}
		}
		else
		{
			return reinterpret_cast<n2dEngineImpl *>(userdata)->Message(hWnd, uMsg, wParam, lParam);
		}
	}
	catch (natException& ex)
	{
		global::EventException(&ex);
		reinterpret_cast<n2dEngineImpl *>(userdata)->TerminateApplication();
	}
	catch (...)
	{
		natException ex(TEXT("Unknown source"), TEXT("Unknown exception"));
		global::EventException(&ex);
		reinterpret_cast<n2dEngineImpl *>(userdata)->TerminateApplication();
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

n2dEngineImpl::UpdateThread::UpdateThread(n2dEngineImpl* pGLApp, nuInt FPSLimit)
	: m_Thisapp(pGLApp), m_FPSLimit(FPSLimit), m_MainThreadID(GetCurrentThreadId())
{
}

nuInt n2dEngineImpl::UpdateThread::ThreadJob()
{
	try
	{
		natStopWatch tTimer;
		n2dFPSControllerImpl tFPSC(m_FPSLimit);

		tTimer.Reset();

		natCriticalSection& rCS = m_Thisapp->m_Section;
		bool bLoop;

		while (true)
		{
			rCS.Lock();
			bLoop = m_Thisapp->m_IsProgramLooping;
			rCS.UnLock();

			if (!bLoop)
			{
				break;
			}

			m_Thisapp->m_pListener->Update(tFPSC.Update(tTimer), &tFPSC);
		}

		// 投递线程终止消息
		PostThreadMessage(m_MainThreadID, WM_USER, 0, 0);
	}
	catch (natException& ex)
	{
		natException tEx(_T("n2dEngineImpl::UpdateThread::ThreadJob"), _T("Update break"), &ex);
		global::EventException(&tEx);
	}
	catch (...)
	{
		natException ex(_T("Unknown source"), _T("Unknown exception"));
		natException tEx(_T("n2dEngineImpl::UpdateThread::ThreadJob"), _T("Update break"), &ex);
		global::EventException(&tEx);
	}

	return 0u;
}

n2dEngineImpl::RenderThread::RenderThread(n2dEngineImpl* pGLApp, nuInt FPSLimit)
	: m_Thisapp(pGLApp), m_FPSLimit(FPSLimit), m_MainThreadID(GetCurrentThreadId()), m_bAlreadyMadeCurrent(false)
{
}

nuInt n2dEngineImpl::RenderThread::ThreadJob()
{
	try
	{
		natStopWatch tTimer;
		n2dFPSControllerImpl tFPSC(m_FPSLimit);

		tTimer.Reset();

		natCriticalSection& rCS = m_Thisapp->m_Section;
		bool bLoop;

		while (true)
		{
			rCS.Lock();
			bLoop = m_Thisapp->m_IsProgramLooping;
			rCS.UnLock();

			if (!bLoop)
			{
				break;
			}

			if (!m_bAlreadyMadeCurrent)
			{
				m_Thisapp->m_pRenderer->MakeCurrent();
				m_bAlreadyMadeCurrent = true;
			}

			m_Thisapp->m_pListener->Render(tFPSC.Update(tTimer), &tFPSC, m_Thisapp->m_pRenderer);
			//m_Thisapp->m_Window.SwapBuffers();
		}

		// 投递线程终止消息
		PostThreadMessage(m_MainThreadID, WM_USER, 0, 0);
	}
	catch (natException& ex)
	{
		natException tEx(_T("n2dEngineImpl::RenderThread::ThreadJob"), _T("Render break"), &ex);
		global::EventException(&tEx);
	}
	catch (...)
	{
		natException ex(_T("Unknown source"), _T("Unknown exception"));
		natException tEx(_T("n2dEngineImpl::RenderThread::ThreadJob"), _T("Render break"), &ex);
		global::EventException(&tEx);
	}

	return 0u;
}

n2dEngineImpl::WndMsgEventImpl::WndMsgEventImpl(n2dEngineImpl* App)
	: m_pEngine(App)
{
	if (m_pEngine == nullptr)
	{
		natLog::GetInstance().LogWarn(TEXT("App should not be an null pointer, this may be an error, please check"));
	}
}

n2dEngineImpl::WndMsgEventImpl::~WndMsgEventImpl()
{
}

nuInt n2dEngineImpl::WndMsgEventImpl::Register(EventHandler func, DWORD WndMsg, nInt priority)
{
	auto& vec = m_EventHandler[WndMsg][priority];
	vec.push_back(func);
	return vec.size() - 1u;
}

nBool n2dEngineImpl::WndMsgEventImpl::Unregister(nuInt HandlerIndex, DWORD WndMsg, nInt Priority)
{
	auto itea = m_EventHandler.find(WndMsg);
	if (itea != m_EventHandler.end())
	{
		auto itea1 = itea->second.find(Priority);
		if (itea1 != itea->second.end())
		{
			auto& vec = itea1->second;
			if (HandlerIndex < vec.size())
			{
				vec.erase(vec.begin() + HandlerIndex);
				return true;
			}
		}
	}
	
	return false;
}

void n2dEngineImpl::WndMsgEventImpl::Unregister(DWORD WndMsg, EventHandler Handler)
{
	auto itea = m_EventHandler[WndMsg];
	for each (auto itea2 in itea)
	{
		std::vector<EventHandler>::iterator itea3;
		while ((itea3 = std::find(itea2.second.begin(), itea2.second.end(), Handler)) != itea2.second.end())
		{
			itea2.second.erase(itea3);
		}
	}
}

bool n2dEngineImpl::WndMsgEventImpl::Activate(DWORD WndMsg, nInt PriorityLimitmin, nInt PriorityLimitmax)
{
	auto i = m_EventHandler[WndMsg];
	for (auto i1 = PriorityLimitmax; i1 <= PriorityLimitmin; ++i1)
	{
		for each (auto eh in i[i1])
		{
			eh(*this);
		}
	}

	return m_isCanceled;
}

bool n2dEngineImpl::WndMsgEventImpl::Activate(DWORD WndMsg, dataType const& data, nInt PriorityLimitmin, nInt PriorityLimitmax)
{
	SetData(data);
	return Activate(WndMsg, PriorityLimitmin, PriorityLimitmax);
}

bool n2dEngineImpl::WndMsgEventImpl::operator()(DWORD WndMsg, dataType const& data)
{
	return Activate(WndMsg, data, Priority::Low, Priority::High);
}

void n2dEngineImpl::WndMsgEventImpl::Release()
{
	m_EventHandler.clear();
	SetCanceled();
}

n2dEngineImpl* n2dEngineImpl::WndMsgEventImpl::GetEngine()
{
	return m_pEngine;
}