#include "n2dEngineImpl.h"
#include <natException.h>
#include <natStream.h>
#include "n2dFPSControllerImpl.h"
#include "..\RenderDevice\n2dRenderDeviceImpl.h"
#include "..\Sound\n2dSoundSysImpl.h"
#include "..\n2dCommon.h"
#include "..\RenderDevice\OpenGL.h"
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
namespace n2dGlobal
{
	extern ncTStr Logfile;

	extern "C"
	{
		N2DFUNC void SetLogFile(ncTStr Path)
		{
			Logfile = Path;
		}

		N2DFUNC void LogMsg(ncTStr Str)
		{
			natLog::GetInstance().LogMsg(Str);
		}

		N2DFUNC void LogWarn(ncTStr Str)
		{
			natLog::GetInstance().LogWarn(Str);
		}

		N2DFUNC void LogErr(ncTStr Str)
		{
			natLog::GetInstance().LogErr(Str);
		}

		N2DFUNC void RegisterLogUpdateEventFunc(natEventBus::EventListenerFunc func)
		{
			natLog::GetInstance().RegisterLogUpdateEventFunc(func);
		}

		N2DFUNC void RegisterExceptionEventFunc(natEventBus::EventListenerFunc func)
		{
			natEventBus::GetInstance().RegisterEventListener<natExceptionEvent>(func);
		}

		N2DFUNC natLog& GetLogger()
		{
			return natLog::GetInstance();
		}

		N2DFUNC natEventBus& GetEventBus()
		{
			return natEventBus::GetInstance();
		}
	}

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
		n2dGlobal::hDll = hModule;
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

n2dRenderDevice* n2dEngineImpl::GetRenderDevice()
{
	if (!m_pRenderer)
	{
		m_pRenderer = new n2dRenderDeviceImpl(this);
	}

	return m_pRenderer;
}

n2dSoundSys * n2dEngineImpl::GetSoundSys()
{
	if (!m_pSoundSys)
	{
		m_pSoundSys = new n2dSoundSysImpl(this);
	}

	return m_pSoundSys;
}

n2dEngineImpl::ThreadMode n2dEngineImpl::GetThreadMode() const
{
	return m_ThreadMode;
}

HINSTANCE n2dEngineImpl::GetHInstance() const
{
	return m_hInstance;
}

void n2dEngineImpl::AddMessageHandler(natEventBus::EventListenerFunc func, Priority::Priority priority)
{
	natEventBus::GetInstance().RegisterEventListener<WndMsgEvent>(func, priority);
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
	m_pRenderer(nullptr), m_pSoundSys(nullptr)
{
	natEventBus::GetInstance().RegisterEvent<WndMsgEvent>();
	natEventBus::GetInstance().RegisterEvent<n2dGlobal::natExceptionEvent>();

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
		nat_Throw(natException, _T("Failed to register window"));
	}
}

n2dEngineImpl::~n2dEngineImpl()
{
	SafeRelease(m_pRenderer);
	SafeRelease(m_pSoundSys);
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
		m_Window.Destroy();
	}
	catch (natException& ex)
	{
		n2dGlobal::natExceptionEvent event(ex);
		natEventBus::GetInstance().Post(event);
	}
}

LRESULT n2dEngineImpl::Message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Msgdata msg{ hWnd, uMsg, wParam, lParam, 0 };
	WndMsgEvent event(this, msg);
	switch (uMsg)
	{
	case WM_SYSCOMMAND:
		if (natEventBus::GetInstance().Post(event))
		{
			return msg.result;
		}

		switch (wParam)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return FALSE;
		default:
			break;
		}
		break;

	case WM_CLOSE:
		if (natEventBus::GetInstance().Post(event))
		{
			return msg.result;
		}

		TerminateApplication();
		return FALSE;

	case WM_EXITMENULOOP:
	case WM_EXITSIZEMOVE:
		if (natEventBus::GetInstance().Post(event))
		{
			return msg.result;
		}
		return FALSE;

	case WM_MOVE:
		if (natEventBus::GetInstance().Post(event))
		{
			return msg.result;
		}
		m_Window.SetPosX(LOWORD(lParam));
		m_Window.SetPosY(HIWORD(wParam));
		return FALSE;

	case WM_PAINT:
		break;

	case WM_SIZING:
	{
		if (natEventBus::GetInstance().Post(event))
		{
			return msg.result;
		}
		RECT *rect = reinterpret_cast<RECT *>(lParam);
		m_Window.SetWidth(rect->right - rect->left);
		m_Window.SetHeight(rect->bottom - rect->top);
		return TRUE;
	}

	case WM_SIZE:
		if (natEventBus::GetInstance().Post(event))
		{
			return msg.result;
		}
		switch (wParam)
		{
		case SIZE_MINIMIZED:
			m_IsVisible = false;
			return FALSE;

		case SIZE_MAXIMIZED:
		case SIZE_RESTORED:
			m_IsVisible = true;
			m_Window.SetWidth(LOWORD(lParam));
			m_Window.SetHeight(HIWORD(lParam));
			m_Window.ReshapeGL();
			return FALSE;
		}
		break;

	case WM_KEYDOWN:
		m_Keys.SetPressed(wParam);
		if (natEventBus::GetInstance().Post(event))
		{
			return msg.result;
		}
		return FALSE;

	case WM_KEYUP:
		m_Keys.SetReleased(wParam);
		if (natEventBus::GetInstance().Post(event))
		{
			return msg.result;
		}
		return FALSE;

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

	if (natEventBus::GetInstance().Post(event))
	{
		return msg.result;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void n2dEngineImpl::CommonInit()
{
	GLenum tRet;
	glewExperimental = true;
	if ((tRet = glewInit()) != GLEW_OK)
	{
		nat_Throw(natException, _T("GLEW initializing failed, id=%d, description:%s"), tRet, glewGetErrorString(tRet));
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LESS);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glLoadIdentity();

#ifndef Natsu2DStatic

	std::vector<nByte> tVert = natUtil::GetResourceData(IDR_DefaultVertShader, _T("SHADER"), n2dGlobal::hDll);
	std::vector<nByte> tFrag = natUtil::GetResourceData(IDR_DefaultFragShader, _T("SHADER"), n2dGlobal::hDll);

	if (!tVert.empty() && !tFrag.empty())
	{
		n2dShaderWrapperImpl* pShaderWrapper = dynamic_cast<n2dShaderWrapperImpl*>(m_pRenderer->GetShaderWrapper());
		n2dShaderProgramImpl* pProgram = new n2dShaderProgramImpl;
		
		n2dShader* pShader[2];
		auto pStream = natMemoryStream::CreateFromExternMemory(tVert.data(), tVert.size(), true, false);
		pShaderWrapper->CreateShaderFromStream(pStream, n2dShader::ShaderType::Vertex, false, &pShader[0]);
		if (!pShader[0]->Compiled())
		{
			nat_Throw(natException, _T("Compile DefaultVertexShader Failed, Log: %s"), pShader[0]->GetInfoLog());
		}

		pStream = natMemoryStream::CreateFromExternMemory(tFrag.data(), tFrag.size(), true, false);
		pShaderWrapper->CreateShaderFromStream(pStream, n2dShader::ShaderType::Fragment, false, &pShader[1]);
		if (!pShader[1]->Compiled())
		{
			nat_Throw(natException, _T("Compile DefaultFragmentShader Failed, Log: %s"), pShader[1]->GetInfoLog());
		}

		pProgram->AttachShader(pShader[0]);
		pProgram->AttachShader(pShader[1]);

		pProgram->Link();
		if (!pProgram->IsLinked())
		{
			nat_Throw(natException, _T("Link DefaultShaderProgram Failed, Log: %s"), pProgram->GetInfoLog());
		}
		pProgram->Use();

		pProgram->DetachShader(pShader[0]);
		pProgram->DetachShader(pShader[1]);

		SafeRelease(pShader[0]);
		SafeRelease(pShader[1]);

		pShaderWrapper->m_DefaultProgram = pProgram;
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
		n2dGlobal::natExceptionEvent event(ex);
		natEventBus::GetInstance().Post(event);
		reinterpret_cast<n2dEngineImpl *>(userdata)->TerminateApplication();
	}
	catch (...)
	{
		natException ex(TEXT("Unknown source"), TEXT("Unknown exception"));
		n2dGlobal::natExceptionEvent event(ex);
		natEventBus::GetInstance().Post(event);
		reinterpret_cast<n2dEngineImpl *>(userdata)->TerminateApplication();
		throw;
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
		n2dGlobal::natExceptionEvent event(ex);
		natEventBus::GetInstance().Post(event);
	}
	catch (...)
	{
		natException ex(_T("n2dEngineImpl::UpdateThread::ThreadJob"), _T("Unknown exception"));
		n2dGlobal::natExceptionEvent event(ex);
		natEventBus::GetInstance().Post(event);
		throw;
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
		n2dGlobal::natExceptionEvent event(ex);
		natEventBus::GetInstance().Post(event);
	}
	catch (...)
	{
		natException ex(_T("n2dEngineImpl::RenderThread::ThreadJob"), _T("Unknown exception"));
		n2dGlobal::natExceptionEvent event(ex);
		natEventBus::GetInstance().Post(event);
		throw;
	}

	return 0u;
}
