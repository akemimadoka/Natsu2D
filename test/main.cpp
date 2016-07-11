#pragma region 预编译

// 标准头文件
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <fstream>

// 脚本相关头文件
#include <sqrat.h>
#include <sqstdsystem.h>
#include <sqstdstring.h>
#include <sqstdblob.h>
#include <sqstdmath.h>

#include "resource.h"

#include <Natsu2D.h>

#include <n2dFont.h>

#include <n2dRenderDevice.h>
#include <n2dGraphics.h>
#include <n2dUtil.h>
#include <n2dModel.h>

// NatsuLib头文件
#include <natException.h>
#include <natStream.h>
#include <natLog.h>

#pragma endregion

namespace Global
{
	n2dEngine* g_pEngine;
	std::ofstream g_LogFile;
}

void custom_invalid_parameter(ncTStr expression, ncTStr function, ncTStr file, nuInt line, uintptr_t pReserved);

///	@brief	调试用缓存
struct DebugBuffer
{
	natVec4<> vert;
	natVec2<> uv;
	natVec3<> normal;
	natMat4<> mvp;
};

///	@brief	脚本打印函数
void printfunc(HSQUIRRELVM, const SQChar *s, ...)
{
	va_list vl;
	va_start(vl, s);
	auto length = _vsctprintf(s, vl);
	std::vector<SQChar> buf(length + 1);
	_vsntprintf_s(buf.data(), buf.size(), buf.size(), s, vl);
	if (Global::g_pEngine)
	{
		Global::g_pEngine->GetLogger().LogMsg(buf.data());
	}
	else
	{
		std::wcerr << buf.data() << std::endl;
	}
	va_end(vl);
}

///	@brief	脚本错误函数
void errorfunc(HSQUIRRELVM, const SQChar *s, ...)
{
	va_list vl;
	va_start(vl, s);
	auto length = _vsctprintf(s, vl);
	std::vector<SQChar> buf(length + 1);
	_vsntprintf_s(buf.data(), buf.size(), buf.size(), s, vl);
	if (Global::g_pEngine)
	{
		Global::g_pEngine->GetLogger().LogErr(buf.data());
	}
	else
	{
		std::wcerr << buf.data() << std::endl;
	}
	va_end(vl);
}

///	@brief	测试用App类
///	@note	仅用于测试
class test final
	: public n2dEngineEventListener
{
public:
	static void WndEventHandler(natEventBase& e)
	{
		n2dEngine::WndMsgEvent& wndEvent = dynamic_cast<n2dEngine::WndMsgEvent&>(e);
		test* pTest = dynamic_cast<test*>(wndEvent.GetEngine()->GetListener());
		if (!pTest)
		{
			return;
		}

		switch (wndEvent.GetMsg().uMsg)
		{
		case WM_LBUTTONDOWN:
			pTest->MouseClick(wndEvent);
			break;
		case WM_MOUSEMOVE:
			pTest->MouseMove(wndEvent);
			break;
		case WM_MOUSEWHEEL:
			pTest->MouseWheel(wndEvent);
			break;
		case WM_KEYDOWN:
			pTest->KeyDown(wndEvent);
			break;
		default:
			break;
		}
	}

	void MouseClick(n2dEngine::WndMsgEvent& e)
	{
		m_bShouldControl = !m_bShouldControl;
		m_Speed = natVec3<nFloat>(0.0f);
	}

	void MouseMove(n2dEngine::WndMsgEvent& e)
	{
		if (m_bShouldControl)
		{
			POINTS mp = MAKEPOINTS(e.GetMsg().lParam);

			m_Speed.x = -5.0f + 10.0f * mp.x / m_pEngine->GetWindow()->GetWidth();
			m_Speed.y = 5.0f - 10.0f * mp.y / m_pEngine->GetWindow()->GetHeight();
		}
	}

	void MouseWheel(n2dEngine::WndMsgEvent& e)
	{
		auto Delta = static_cast<nShort>(HIWORD(e.GetMsg().wParam)) / WHEEL_DELTA;
		
		//m_Speed.z = 10.0f ;
		// 立即改变
		natMat4<> tmat(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, Delta * 10.f, 1.0f
			);
		m_pEngine->GetRenderDevice()->SubmitViewMat(m_pEngine->GetRenderDevice()->GetCurViewMat() * tmat);
	}

	void KeyDown(n2dEngine::WndMsgEvent& e)
	{
		static natCriticalSection sec;
		switch (e.GetMsg().wParam)
		{
		case VK_RETURN:
		{
			auto state = m_soundsrc->GetState();
			switch (state)
			{
			case n2dSoundSource::SourceState::Initial:
			case n2dSoundSource::SourceState::Paused:
				m_soundsrc->Play();
				break;
			case n2dSoundSource::SourceState::Playing:
				m_soundsrc->Pause();
				break;
			case n2dSoundSource::SourceState::Stopped:
				m_soundsrc->Rewind();
				break;
			default:
				break;
			}
			break;
		}
		case VK_F1:
			if (NATOK(sec.TryLock()))
			{
				e.GetEngine()->ToggleFullscreen();
				sec.UnLock();
			}

			break;
		case VK_ESCAPE:
			e.GetEngine()->TerminateApplication();
			break;
		default:
			break;
		}
	}

	explicit test(HINSTANCE hInstance)
		: m_Speed(0.0f),m_bShouldControl(false),m_RotateL(0.0f),m_RotateH(0.0f),f(nullptr),
		m_model(nullptr)//f(48, 0, 0, 0, FW_BOLD, false, false, false, GB2312_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, _T("微软雅黑"))
	{
		if (NATFAIL(CreateN2DEngine(0, _T("GLAPP"), 0u, 0u, 800u, 600u, 1280u, 720u, 32u, false, hInstance, n2dEngine::ThreadMode::MultiThread, this, &m_pEngine)))
		{
			nat_Throw(natException, _T("Failed to create Natsu2D engine"));
		}

		Global::g_pEngine = m_pEngine;
		m_pEngine->GetEventBus().RegisterEventListener<n2dGlobal::natExceptionEvent>([this](natEventBase& event)
		{
			std::basic_stringstream<nTChar, std::char_traits<nTChar>, std::allocator<nTChar>> ss;
			auto pEvent = dynamic_cast<n2dGlobal::natExceptionEvent*>(&event);
			if (!pEvent)
			{
				return;
			}

			auto ex = &pEvent->GetData();
			if (ex != nullptr)
			{
				ss << _T("Unhandled exception: In ") << ex->GetSource() << _T(" : ") << ex->GetDesc() << std::endl;

				m_pEngine->GetLogger().LogErr(ss.str().c_str());
				MessageBox(nullptr, ss.str().c_str(), _T("Unhandled exception"), MB_OK | MB_ICONERROR);
			}

			m_pEngine->TerminateApplication();
		});

		m_pEngine->GetLogger().RegisterLogUpdateEventFunc([this](natEventBase& event)
		{
			decltype(auto) eventLogUpdated = static_cast<natLog::EventLogUpdated&>(event);
			time_t time = std::chrono::system_clock::to_time_t(eventLogUpdated.GetTime());
			tm timeStruct;
			localtime_s(&timeStruct, &time);
			natLog::LogType logType = static_cast<natLog::LogType>(eventLogUpdated.GetLogType());
			nTString logStr = std::move(natUtil::FormatString(_T("[{0}] [{1}] {2}"), std::put_time(&timeStruct, _T("%F %T")), natLog::GetDefaultLogTypeName(logType), eventLogUpdated.GetData()));
			switch (logType)
			{
			case natLog::Msg:
			case natLog::Warn:
				std::wclog << logStr << std::endl;
				break;
			case natLog::Err:
				std::wcerr << logStr << std::endl;
				break;
			default:
				break;
			}
		});

		m_pEngine->GetLogger().LogMsg(_T("中文测试"));
		m_pEngine->MainLoop(_T("夏之幻想"), 6000u);
	}
	~test()
	{
	}

	nBool EngineInit() override
	{
		// 脚本初始化
		// 语言是Squirrel
		HSQUIRRELVM vm = sq_open(1024);
		sqstd_seterrorhandlers(vm);
		sq_setprintfunc(vm, printfunc, errorfunc);

		sq_pushroottable(vm);

		sqstd_register_systemlib(vm);
		sqstd_register_iolib(vm);
		sqstd_register_stringlib(vm);
		sqstd_register_bloblib(vm);
		sqstd_register_mathlib(vm);

		Sqrat::DefaultVM::Set(vm);

		Sqrat::Script script;
		script.CompileFile(_SC(R"(script\boot.nut)"));
		script.Run();

		Sqrat::RootTable root;

		// 脚本中使用这些回调函数完成相关逻辑
		m_Init = root.GetFunction(_T("Init"));
		m_Uninit = root.GetFunction(_T("Uninit"));

		if (!m_Init.IsNull())
		{
			m_Init.Execute();
		}

		return true;
	}

	void EngineUninit() override
	{
		if (!m_Uninit.IsNull())
		{
			m_Uninit.Execute();
		}

		m_Init.Release();
		m_Uninit.Release();

		sq_close(Sqrat::DefaultVM::Get());
		m_pEngine->GetLogger().LogMsg(_T("GLAPP quit"));
	}

	nBool WindowInit() override
	{
		static bool inited = false;
		n2dRenderDevice* renderdevice = m_pEngine->GetRenderDevice();
		
		if (!inited)
		{
			// 此处忽略所有可能发生的错误，需要时请使用NATFAIL宏判断函数是否失败

			// 创建二维图元渲染器
			renderdevice->CreateGraphics2D(&m_pGraphics);
			// 创建三维图元渲染器
			renderdevice->CreateGraphics3D(&m_pGraphics3D);
			// 纹理加载
			// TODO: 完成纹理管理器
			//renderdevice->CreateTexture(&m_texture);
			//renderdevice->CreateTexture(&m_texture2);
			//renderdevice->CreateTexture(&m_texture3);

			// 创建模型加载器
			renderdevice->CreateModelLoader(&m_modelloader);
			// 用于调试的缓存
			renderdevice->CreateBuffer(n2dBuffer::BufferTarget::ShaderStorageBuffer, &m_DebugBuffer);
			// 创建动作管理器
			renderdevice->CreateMotionManager(&m_MotionManager);
		}
		
		m_pEngine->GetLogger().LogMsg(natUtil::FormatString(_T("GLAPP initialized as {0} thread mode"), (m_pEngine->GetThreadMode() == n2dEngine::ThreadMode::SingleThread ? _T("single") : _T("multi"))).c_str());
		m_pEngine->GetLogger().LogMsg(_T("GLAPP start initializing"));
		m_Mutex = CreateMutex(NULL, FALSE, _T("GLAPP"));
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(m_Mutex);
			nat_Throw(natException, _T("GLAPP is already running"));
		}
		if (!m_Mutex)
		{
			nat_Throw(natException, _T("Cannot create mutex"));
		}

		n2dWindow* window = m_pEngine->GetWindow();

		if (!window->GetFullScreen())
		{
			SetWindowLong(window->GetWnd(), GWL_STYLE, GetWindowLong(window->GetWnd(), GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX);
			HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
			SendMessage(window->GetWnd(), WM_SETICON, TRUE, LPARAM(hIcon));
			SendMessage(window->GetWnd(), WM_SETICON, FALSE, LPARAM(hIcon));
		}

		SetActiveWindow(window->GetWnd());

		if (!inited)
		{
			inited = true;

			// 关闭垂直同步
			renderdevice->SetSwapInterval(0u);

			//renderdevice->DisableCapability(n2dRenderDevice::Capability::Blend);
			renderdevice->EnableCapability(n2dRenderDevice::Capability::Blend);
			renderdevice->SetBlendMode(n2dRenderDevice::BlendFactor::SrcAlpha, n2dRenderDevice::BlendFactor::OneMinusSrcAlpha);

			sw = renderdevice->GetShaderWrapper();
			sw->CreateProgram(&sp);
			n2dShader* pShader[2];
			natStream* pStream = new natFileStream(_T("VertexShader.glsl"), true, false);
			sw->CreateShaderFromStream(pStream, n2dShader::ShaderType::Vertex, false, &pShader[0]);
			if (!pShader[0]->Compiled())
			{
				nat_Throw(natException, natUtil::FormatString(_T("Compile Shader0 Failed, Log: %s"), pShader[0]->GetInfoLog()).c_str());
			}

			ncTStr pLog = pShader[0]->GetInfoLog();
			if (_tcslen(pLog) > 1)
			{
				m_pEngine->GetLogger().LogWarn(natUtil::FormatString(_T("Compile0Log: %s"), pLog).c_str());
			}

			SafeRelease(pStream);
			pStream = new natFileStream(_T("FragmentShader.glsl"), true, false);
			sw->CreateShaderFromStream(pStream, n2dShader::ShaderType::Fragment, false, &pShader[1]);
			if (!pShader[1]->Compiled())
			{
				nat_Throw(natException, natUtil::FormatString(_T("Compile Shader1 Failed, Log: %s"), pShader[1]->GetInfoLog()).c_str());
			}

			pLog = pShader[1]->GetInfoLog();
			if (_tcslen(pLog) > 1)
			{
				m_pEngine->GetLogger().LogWarn(natUtil::FormatString(_T("Compile1Log: %s"), pLog).c_str());
			}

			SafeRelease(pStream);
			sp->AttachShader(pShader[0]);
			sp->AttachShader(pShader[1]);

			sp->Link();
			if (!sp->IsLinked())
			{
				nat_Throw(natException, natUtil::FormatString(_T("Link ShaderProgram Failed, Log: %s"), sp->GetInfoLog()).c_str());
			}

			pLog = sp->GetInfoLog();
			if (_tcslen(pLog) > 1)
			{
				m_pEngine->GetLogger().LogWarn(natUtil::FormatString(_T("LinkLog: %s"), pLog).c_str());
			}

			sp->Use();

			sp->DetachShader(pShader[0]);
			sp->DetachShader(pShader[1]);

			SafeRelease(pShader[0]);
			SafeRelease(pShader[1]);

			/*posID = glGetSubroutineUniformLocation(programID, n2dShaderWrapperImpl::Vertex, "positionShader");
			if (posID >= 0)
			{
			GLuint def = glGetSubroutineIndex(programID, n2dShaderWrapperImpl::Vertex, "DefaultPositionFunc");
			if (def == GL_INVALID_INDEX)
			{
			nat_Throw(natException, _T("Cannot get default position subroutine"));
			}
			GLsizei n;
			glGetIntegerv(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &n);
			GLuint* indices = new GLuint[n];
			memset(indices, 0, n * sizeof(GLuint));
			indices[posID] = def;
			glUniformSubroutinesuiv(n2dShaderWrapperImpl::Vertex, n, indices);
			SafeDelArr(indices);
			}*/

			ViewMatrix = sp->GetUniformReference(_T("V"));
			ModelMatrix = sp->GetUniformReference(_T("M"));
			Light = sp->GetUniformReference(_T("LightPosition_worldspace"));

			m_pEngine->AddMessageHandler(WndEventHandler);

			renderdevice->SubmitProjMat(natTransform::perspective(45.0f, 4.0f / 3.0f, 0.1f));
			renderdevice->SubmitViewMat(natTransform::lookAt(
				natVec3<>(0, 100, 200),
				natVec3<>(0, 80, 0),
				natVec3<>(0, 1, 0)
			));
			renderdevice->SubmitModelMat(natMat4<>(1.0f));

			renderdevice->SetMaxLights(10u);
			n2dLightController* pLight = renderdevice->GetLightController(0u);
			auto prop = pLight->GetProperties();
			prop.isEnabled = true;
			prop.ambient = natVec3<>(0.0f, 0.0f, 0.0f);
			prop.color = natVec3<>(1.0f, 1.0f, 1.0f);
			prop.position = natVec3<>(100.f, 200.f, 0.f);
			pLight->SetProperties(prop);

			m_DebugBuffer->AllocData(sizeof(DebugBuffer), nullptr, n2dBuffer::BufferUsage::DynamicRead);
			m_DebugBuffer->BindBase(3u);

			//f->InitFont(renderdevice, _T("fz.ttf"), 32, 32);
			//f->InitText(_T("23"), 2);
			/*if (!m_texture->LoadTexture(_T("texture.dds")))
			{
			nat_Throw(natException, _T("Unable to load texture"));
			}
			if (!m_texture2->LoadTexture(_T("table.dds")))
			{
			nat_Throw(natException, _T("Unable to load texture"));
			}

			if (!m_texture3->LoadTexture(_T("ch.png")))
			{
			nat_Throw(natException, _T("Unable to load texture"));
			}*/

			n2dSoundSys* pSound = m_pEngine->GetSoundSys();
			pStream = new natFileStream(_T("Lamb.wav"), true, false);
			pSound->CreateWaveSoundBufferFromStream(pStream, &m_soundbuf);
			SafeRelease(pStream);
			pSound->CreateSoundSource(&m_soundsrc);
			m_soundsrc->BindBuffer(m_soundbuf);
			m_soundsrc->SetLooping(false);

			//MessageBox(window->GetWnd(), _T("即将开始加载模型，请等待\n按上下左右进行旋转，点击鼠标左键开始移动，滚动鼠标滚轮进行缩放"), _T("提示"), MB_OK | MB_ICONINFORMATION);
			m_pEngine->GetLogger().LogMsg(_T("Loading model"));
			//m_modelloader->SetDefaultTexture(m_texture);
			//if (NATFAIL(m_modelloader->CreateStaticModelFromFile(_T("comb.obj"), &m_model2)))
			if (NATFAIL(m_modelloader->CreateDynamicModelFromFile(_T("cirno.pmd"), &m_model2)))
			{
				nat_Throw(natException, _T("Unable to load model"));
			}

			/*m_modelloader->SetDefaultTexture(m_texture2);
			if (NATFAIL(m_modelloader->CreateStaticModelFromFile(_T("table.obj"), &m_model)))
			{
			nat_Throw(natException, _T("Unable to load model"));
			}*/

			if (NATFAIL(m_MotionManager->LoadMotionFromFile(_T("lamb"), _T("Lamb.vmd"))))
			{
				nat_Throw(natException, _T("Unable to load motion"));
			}

			if (NATFAIL(m_MotionManager->ApplyToModel(_T("lamb"), m_model2)))
			{
				nat_Throw(natException, _T("Unable to apply motion to model"));
			}

			//m_model2->SetZoom(0.1f);

			m_pEngine->GetLogger().LogMsg(_T("Loaded model successfully"));
			m_pEngine->GetLogger().LogMsg(_T("GLAPP initialized successfully"));
			m_soundsrc->Play();
		}
		
		return true;
	}

	void WindowUninit() override
	{
		CloseHandle(m_Mutex);
		m_pEngine->GetLogger().LogMsg(_T("GLAPP exit"));
	}

	void Update(nDouble, n2dFPSController* /*pFPSController*/) override
	{
		if (m_pEngine->IsKeyPressed(VK_SPACE))
		{
			// Load default matrix
			m_pEngine->GetRenderDevice()->SubmitViewMat(natTransform::lookAt(
				natVec3<>(0, 100, 200),
				natVec3<>(0, 80, 0),
				natVec3<>(0, 1, 0)
				));
		}

		m_RotateL = m_pEngine->IsKeyPressed(VK_LEFT) ? -1.0f : m_pEngine->IsKeyPressed(VK_RIGHT) ? 1.0f : 0.0f;
		m_RotateH = m_pEngine->IsKeyPressed(VK_UP) ? -1.0f : m_pEngine->IsKeyPressed(VK_DOWN) ? 1.0f : 0.0f;
	}

	void Render(nDouble, n2dFPSController* pFPSController, n2dRenderDevice* pRenderDevice) override
	{
		if (!m_pEngine->GetWindow()->GetFullScreen())
		{
			SetWindowText(m_pEngine->GetWindow()->GetWnd(), n2dUtil::FormatString(_T("夏之幻想 | FPS={0}"), pFPSController->GetFPS()).c_str());
		}

		pRenderDevice->Clear(n2dRenderDevice::ClearBit_Color | n2dRenderDevice::ClearBit_Depth);

		if (NATFAIL(m_pGraphics->Begin()) || NATFAIL(m_pGraphics3D->Begin()))
		{
			nat_Throw(natException, _T("Failed to prepare graphic renderer"));
		}
		
		//f.PrintFont(m_pGraphics, _T("2333"), 0.f, 0.f, 10.f);

		pRenderDevice->SubmitViewMat(natTransform::move(
			natTransform::rotate(
				natTransform::rotate(
					pRenderDevice->GetCurViewMat(),
					natTransform::DegtoRad(m_RotateL),
					natVec3<>(0.0f, 1.0f, 0.0f)
					),
				natTransform::DegtoRad(m_RotateH),
				natVec3<>(1.0f, 0.0f, 0.0f)
				),
			m_Speed));

		ModelMatrix->SetValue(1, &pRenderDevice->GetCurModelMat()[0][0]);
		ViewMatrix->SetValue(1, &pRenderDevice->GetCurViewMat()[0][0]);

		//m_pGraphics3D->RenderModel(m_model);
		m_model2->Update(static_cast<nuInt>(m_soundsrc->GetSecOffset() * 30u));
		m_pGraphics3D->RenderModel(m_model2);

		sp->Use();

		m_pGraphics3D->End();

		/*m_pGraphics->DrawQuad(m_texture3,
			n2dGraphics2DVertex{ natVec3<>(-100, 0, 200), 0, natVec2<>(0, 0) },
			n2dGraphics2DVertex{ natVec3<>(100, 0, 200), 0, natVec2<>(1, 0) },
			n2dGraphics2DVertex{ natVec3<>(100, 200, 200), 0, natVec2<>(1, 1) },
			n2dGraphics2DVertex{ natVec3<>(-100, 200, 200), 0, natVec2<>(0, 1) });*/

		n2dShaderProgram* pd = sw->GetDefaultProgram();
		pd->Use();

		m_pGraphics->End();

		//DebugBuffer db;
		//m_DebugBuffer->GetSubData(0u, sizeof(DebugBuffer), reinterpret_cast<nData>(&db));

		m_pEngine->GetWindow()->SwapBuffers();
	}

private:
	natRefPointer<n2dEngine>		m_pEngine;
	natRefPointer<n2dGraphics2D>	m_pGraphics;
	natRefPointer<n2dGraphics3D>	m_pGraphics3D;

	natVec3<>						m_PosCamera;
	natVec3<>						m_Speed;
	nBool							m_bShouldControl;
	nFloat							m_RotateL;
	nFloat							m_RotateH;

	natRefPointer<n2dFont>			f;
	nUnsafePtr<n2dShaderWrapper>	sw;
	natRefPointer<n2dShaderProgram>	sp;

	nUnsafePtr<n2dShaderProgram::UniformReference> ViewMatrix;
	nUnsafePtr<n2dShaderProgram::UniformReference> ModelMatrix;
	nUnsafePtr<n2dShaderProgram::UniformReference> Light;

	HANDLE							m_Mutex;
	//natRefPointer<n2dTexture2D>		m_texture;
	//natRefPointer<n2dTexture2D>		m_texture2;
	//natRefPointer<n2dTexture2D>		m_texture3;

	natRefPointer<n2dModelLoader>	m_modelloader;
	natRefPointer<n2dModelData>		m_model, m_model2, m_model3;

	natRefPointer<n2dSoundBuffer>	m_soundbuf;
	natRefPointer<n2dSoundSource>	m_soundsrc;

	natRefPointer<n2dBuffer>		m_DebugBuffer;

	natRefPointer<n2dMotionManager> m_MotionManager;

	Sqrat::Function					m_Init, m_Uninit;
};

int main()
{
	try
	{
		//std::locale::global(std::locale(std::locale(), "", LC_CTYPE));
		auto loc = std::locale("", LC_CTYPE);
		Global::g_LogFile.open(_T("Log.log"));
		if (!Global::g_LogFile.is_open())
		{
			exit(EXIT_FAILURE);
		}

		std::locale::global(loc);
		std::wcout.imbue(loc);
		std::wclog.imbue(loc);
		std::wcerr.imbue(loc);
		_set_invalid_parameter_handler(custom_invalid_parameter);

		test (GetModuleHandle(NULL));
	}
	catch (natException& ex)
	{
		if (Global::g_pEngine)
		{
			n2dGlobal::natExceptionEvent event(ex);
			Global::g_pEngine->GetEventBus().Post<n2dGlobal::natExceptionEvent>(event);
		}
		else
		{
			throw;
		}
	}
	catch (...)
	{
		if (Global::g_pEngine)
		{
			Global::g_pEngine->GetLogger().LogErr(_T("Unknown exception"));
		}
		MessageBox(NULL, _T("Unknown exception"), _T("Uncaught exception"), MB_OK | MB_ICONERROR);
	}
	
	_CrtDumpMemoryLeaks();
	return 0;
}

void custom_invalid_parameter(ncTStr expression, ncTStr function, ncTStr file, nuInt line, uintptr_t /*pReserved*/)
{
	std::basic_stringstream<nTChar> ss;

	ss << "In " << file << ", line " << line << ": " << expression;

	natException ex(function, file, line, ss.str().c_str());
	n2dGlobal::natExceptionEvent event(ex);
	if (Global::g_pEngine)
	{
		Global::g_pEngine->GetEventBus().Post<n2dGlobal::natExceptionEvent>(event);
	}
}