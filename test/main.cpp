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
#include <n2dModel.h>

// NatsuLib头文件
#include <natException.h>
#include <natStream.h>
#include <natLog.h>
#include <natMisc.h>

#pragma endregion

namespace Global
{
	n2dEngine* g_pEngine;
	natConsole g_Console;
	std::ofstream g_LogFile;
}

void custom_invalid_parameter(LPCTSTR expression, LPCTSTR function, LPCTSTR file, nuInt line, uintptr_t pReserved);

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
		Global::g_pEngine->GetLogger().LogMsg(nString{ WideStringView{ buf.data() } });
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
		Global::g_pEngine->GetLogger().LogErr(nString{ WideStringView{ buf.data() } });
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
	void WndEventHandler(natEventBase& e)
	{
		n2dEngine::WndMsgEvent& wndEvent = dynamic_cast<n2dEngine::WndMsgEvent&>(e);

		switch (wndEvent.GetMsg().uMsg)
		{
		case WM_LBUTTONDOWN:
			MouseClick(wndEvent);
			break;
		case WM_MOUSEMOVE:
			MouseMove(wndEvent);
			break;
		case WM_MOUSEWHEEL:
			MouseWheel(wndEvent);
			break;
		case WM_KEYDOWN:
			KeyDown(wndEvent);
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

	void MouseWheel(n2dEngine::WndMsgEvent& e) const
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

	void KeyDown(n2dEngine::WndMsgEvent& e) const
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
		: m_Speed(0.0f), m_bShouldControl(false), m_RotateL(0.0f), m_RotateH(0.0f)
	{
		nat_ThrowIfFailed(CreateN2DEngine(0, "GLAPP"_nv, 0u, 0u, 800u, 600u, 1280u, 720u, 32u, false, hInstance, n2dEngine::ThreadMode::MultiThread, this, &m_pEngine), "Failed to create Natsu2D engine"_nv);

		Global::g_pEngine = m_pEngine;

		m_pEngine->GetLogger().UseDefaultAction(Global::g_Console, Global::g_LogFile);

		m_pEngine->GetEventBus().RegisterEventListener<n2dGlobal::natExceptionEvent>([this](natEventBase& event) noexcept
		{
			auto scope = make_scope([]()
			{
				terminate();
			});

			std::basic_stringstream<nTChar> ss;
			auto pEvent = dynamic_cast<n2dGlobal::natExceptionEvent*>(&event);
			if (!pEvent)
			{
				return;
			}

			auto ex = &pEvent->GetData();
			if (ex != nullptr)
			{
				ss << "Unhandled exception: In "_nv << ex->GetSource() << " : "_nv << ex->GetDesc() << std::endl;
				if (typeid(*ex) == typeid(natWinException))
				{
					auto pWinException = static_cast<const natWinException*>(ex);
					ss << "LastErr = "_nv << pWinException->GetErrNo() << ", Description: "_nv << pWinException->GetErrMsg() << std::endl;
				}
				else if (typeid(*ex) == typeid(natErrException))
				{
					auto pnatErrException = static_cast<const natErrException*>(ex);
					ss << "NatErr = "_nv << pnatErrException->GetErrNo() << ", Description: "_nv << pnatErrException->GetErrMsg() << std::endl;
				}
#ifdef EnableExceptionStackTrace
				ss << "Call stack:"_nv << std::endl;
				for (size_t i = 0; i < ex->GetStackWalker().GetFrameCount(); ++i)
				{
					auto&& symbol = ex->GetStackWalker().GetSymbol(i);
					ss << natUtil::FormatString("{3}: (0x%p) {4} at address 0x%p (file {5}:{6} at address 0x%p)"_nv, symbol.OriginalAddress, reinterpret_cast<LPCVOID>(symbol.SymbolAddress), reinterpret_cast<LPCVOID>(symbol.SourceFileAddress), i, symbol.SymbolName, symbol.SourceFileName, symbol.SourceFileLine) << std::endl;
				}
#endif

				nString data{ AnsiStringView{ ss.str().c_str() } };

				m_pEngine->GetLogger().LogErr(data);
				MessageBox(NULL, WideString{ data }.data(), _T("Unhandled exception"), MB_OK | MB_ICONERROR);
			}
		});

		m_pEngine->GetLogger().LogMsg(static_cast<nString>(L"中文测试"_wv));
		m_pEngine->MainLoop(static_cast<nString>(L"夏之幻想"_wv), 60u);
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
		m_Init = root.GetFunction(_SC("Init"));
		m_Uninit = root.GetFunction(_SC("Uninit"));

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
		m_pEngine->GetLogger().LogMsg("GLAPP quit"_nv);
	}

	nBool WindowInit() override
	{
		static auto inited = false;
		n2dRenderDevice* renderdevice = m_pEngine->GetRenderDevice();
		
		if (!inited)
		{
			// 创建二维图元渲染器
			nat_ThrowIfFailed(renderdevice->CreateGraphics2D(&m_pGraphics), "CreateGraphics2D failed."_nv);
			// 创建三维图元渲染器
			nat_ThrowIfFailed(renderdevice->CreateGraphics3D(&m_pGraphics3D), "CreateGraphics3D failed."_nv);
			// 纹理加载
			// TODO: 完成纹理管理器
			//renderdevice->CreateTexture(&m_texture);
			renderdevice->CreateTexture(&m_texture2);
			//renderdevice->CreateTexture(&m_texture3);

			// 创建模型加载器
			nat_ThrowIfFailed(renderdevice->CreateModelLoader(&m_modelloader), "CreateModelLoader failed."_nv);
			// 用于调试的缓存
			nat_ThrowIfFailed(renderdevice->CreateBuffer(n2dBuffer::BufferTarget::ShaderStorageBuffer, &m_DebugBuffer), "CreateBuffer failed."_nv);
			// 创建动作管理器
			nat_ThrowIfFailed(renderdevice->CreateMotionManager(&m_MotionManager), "CreateMotionManager failed."_nv);

			nat_ThrowIfFailed(renderdevice->CreateFontManager(&f), "CreateFontManager failed."_nv);
		}

		m_pEngine->GetLogger().LogMsg(natUtil::FormatString("GLAPP initialized as {0} thread mode"_nv, m_pEngine->GetThreadMode() == n2dEngine::ThreadMode::SingleThread ? "single"_nv : "multi"_nv));
		m_pEngine->GetLogger().LogMsg("GLAPP start initializing"_nv);
		m_Mutex = CreateMutex(NULL, FALSE, _T("GLAPP"));
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			nat_Throw(natException, "GLAPP is already running"_nv);
		}
		if (!m_Mutex)
		{
			nat_Throw(natException, "Cannot create mutex"_nv);
		}

		n2dWindow* window = m_pEngine->GetWindow();

		if (!window->GetFullScreen())
		{
			SetWindowLongPtr(window->GetWnd(), GWL_STYLE, GetWindowLongPtr(window->GetWnd(), GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX);
			HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
			SendMessage(window->GetWnd(), WM_SETICON, TRUE, LPARAM(hIcon));
			SendMessage(window->GetWnd(), WM_SETICON, FALSE, LPARAM(hIcon));
		}

		SetActiveWindow(window->GetWnd());

		if (!inited)
		{
			inited = true;

			// 测试虚拟文件系统
			auto& schemaFactory = m_pEngine->GetSchemaFactory();
			auto& vfs = m_pEngine->GetVirtualFileSystem();

			auto schema = schemaFactory.CreateLocalFilesystemSchema("local"_nv);
			vfs.RegisterSchema(schema);
			auto streamInfo = vfs.GetStreamInfoFromUri("local://script/boot.nut"_nv);
			nLen size = 0;
			nat_ThrowIfFailed(streamInfo->GetSize(size), "Cannot get size."_nv);
			std::chrono::system_clock::time_point time;
			nat_ThrowIfFailed(streamInfo->GetEditTime(time), "Cannot get time."_nv);
			m_pEngine->GetLogger().LogMsg("Path: {0}, Size: {1}"_nv, streamInfo->GetPath(), size);
			std::vector<nByte> data(static_cast<size_t>(size) + 1);
			streamInfo->OpenStream(true, false)->ReadBytes(data.data(), size);
			m_pEngine->GetLogger().LogMsg("{0}"_nv, AnsiStringView(reinterpret_cast<ncStr>(data.data())));

			// 关闭垂直同步
			renderdevice->SetSwapInterval(1u);

			//renderdevice->DisableCapability(n2dRenderDevice::Capability::Blend);
			renderdevice->EnableCapability(n2dRenderDevice::Capability::Blend);
			renderdevice->SetBlendMode(n2dRenderDevice::BlendFactor::SrcAlpha, n2dRenderDevice::BlendFactor::OneMinusSrcAlpha);

			sw = renderdevice->GetShaderWrapper();
			sw->CreateProgram(&sp);
			natRefPointer<n2dShader> pShader[2];
			auto pStream = vfs.GetStreamInfoFromUri("local://VertexShader.glsl"_nv)->OpenStream(true, false);
			sw->CreateShaderFromStream(pStream, n2dShader::ShaderType::Vertex, false, &pShader[0]);
			if (!pShader[0]->Compiled())
			{
				nat_Throw(natException, "Compile Shader0 Failed, Log: %s"_nv, pShader[0]->GetInfoLog());
			}

			auto pLog = pShader[0]->GetInfoLog();
			if (!pLog.empty())
			{
				m_pEngine->GetLogger().LogWarn("Compile0Log: %s"_nv, pLog);
			}

			pStream = vfs.GetStreamInfoFromUri("local://FragmentShader.glsl"_nv)->OpenStream(true, false);
			sw->CreateShaderFromStream(pStream, n2dShader::ShaderType::Fragment, false, &pShader[1]);
			if (!pShader[1]->Compiled())
			{
				nat_Throw(natException, "Compile Shader1 Failed, Log: %s"_nv, pShader[1]->GetInfoLog());
			}

			pLog = pShader[1]->GetInfoLog();
			if (!pLog.empty())
			{
				m_pEngine->GetLogger().LogWarn("Compile1Log: %s"_nv, pLog);
			}

			sp->AttachShader(pShader[0]);
			sp->AttachShader(pShader[1]);

			sp->Link();
			if (!sp->IsLinked())
			{
				nat_Throw(natException, "Link ShaderProgram Failed, Log: %s"_nv, sp->GetInfoLog());
			}

			pLog = sp->GetInfoLog();
			if (!pLog.empty())
			{
				m_pEngine->GetLogger().LogWarn("LinkLog: %s"_nv, pLog);
			}

			sp->Use();

			sp->DetachShader(pShader[0]);
			sp->DetachShader(pShader[1]);

			/*posID = glGetSubroutineUniformLocation(programID, n2dShaderWrapperImpl::Vertex, "positionShader");
			if (posID >= 0)
			{
			GLuint def = glGetSubroutineIndex(programID, n2dShaderWrapperImpl::Vertex, "DefaultPositionFunc");
			if (def == GL_INVALID_INDEX)
			{
			nat_Throw(natException, "Cannot get default position subroutine"_nv);
			}
			GLsizei n;
			glGetIntegerv(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &n);
			GLuint* indices = new GLuint[n];
			memset(indices, 0, n * sizeof(GLuint));
			indices[posID] = def;
			glUniformSubroutinesuiv(n2dShaderWrapperImpl::Vertex, n, indices);
			SafeDelArr(indices);
			}*/

			ViewMatrix = sp->GetUniformReference("V"_nv);
			ModelMatrix = sp->GetUniformReference("M"_nv);
			Light = sp->GetUniformReference("LightPosition_worldspace"_nv);

			m_pEngine->AddMessageHandler(natEventBus::EventListenerDelegate{ &test::WndEventHandler, *this });

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

			f->InitFont(renderdevice, vfs.GetStreamInfoFromUri("local://fz.ttf"_nv)->OpenStream(true, false), 32, 32);
			f->InitText("23"_nv, 2);
			/*if (!m_texture->LoadTexture("texture.dds"_nv))
			{
			nat_Throw(natException, "Unable to load texture"_nv);
			}*/
			if (!m_texture2->LoadTexture("table.dds"_nv))
			{
				nat_Throw(natException, "Unable to load texture"_nv);
			}

			/*if (!m_texture3->LoadTexture("ch.png"_nv))
			{
			nat_Throw(natException, "Unable to load texture"_nv);
			}*/

			n2dSoundSys* pSound = m_pEngine->GetSoundSys();
			pStream = vfs.GetStreamInfoFromUri("local://Lamb.wav"_nv)->OpenStream(true, false);
			pSound->CreateWaveSoundBufferFromStream(pStream, &m_soundbuf);
			pSound->CreateSoundSource(&m_soundsrc);
			m_soundsrc->BindBuffer(m_soundbuf);
			m_soundsrc->SetLooping(false);

			//MessageBox(window->GetWnd(), "即将开始加载模型，请等待\n按上下左右进行旋转，点击鼠标左键开始移动，滚动鼠标滚轮进行缩放"_nv, "提示"_nv, MB_OK | MB_ICONINFORMATION);
			m_pEngine->GetLogger().LogMsg("Loading model"_nv);
			//m_modelloader->SetDefaultTexture(m_texture);
			//m_modelloader->CreateStaticModelFromFile("comb.obj"_nv, &m_model2))
			nat_ThrowIfFailed(m_modelloader->CreateDynamicModelFromFile("cirno.pmd"_nv, &m_model2), "Unable to load model"_nv);

			/*m_modelloader->SetDefaultTexture(m_texture2);
			if (NATFAIL(m_modelloader->CreateStaticModelFromFile("table.obj"_nv, &m_model)))
			{
			nat_Throw(natException, "Unable to load model"_nv);
			}*/

			nat_ThrowIfFailed(m_MotionManager->LoadMotionFromFile("lamb"_nv, "Lamb.vmd"_nv), "Unable to load motion"_nv);
			nat_ThrowIfFailed(m_MotionManager->ApplyToModel("lamb"_nv, m_model2), "Unable to apply motion to model"_nv);

			//m_model2->SetZoom(0.1f);

			m_pEngine->GetLogger().LogMsg("Loaded model successfully"_nv);
			m_pEngine->GetLogger().LogMsg("GLAPP initialized successfully"_nv);
			m_soundsrc->Play();
		}
		
		return true;
	}

	void WindowUninit() override
	{
		CloseHandle(m_Mutex);
		m_pEngine->GetLogger().LogMsg("GLAPP exit"_nv);
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
			SetWindowText(m_pEngine->GetWindow()->GetWnd(), WideString{ natUtil::FormatString("夏之幻想 | FPS={0}"_nv, pFPSController->GetFPS()) }.data());
		}

		pRenderDevice->Clear(n2dRenderDevice::ClearBit_Color | n2dRenderDevice::ClearBit_Depth);

		if (NATFAIL(m_pGraphics->Begin()) || NATFAIL(m_pGraphics3D->Begin()))
		{
			nat_Throw(natException, "Failed to prepare graphic renderer"_nv);
		}
		
		f->PrintFont(m_pGraphics, "琪露诺"_nv, 0.0f, 60.0f, 1.0f, natVec3<>(1.0f, 1.0f, 0.0f));
		f->PrintFont(m_pGraphics, "琪露诺"_nv, 0.0f, 30.0f, 1.0f, m_texture2);

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

		//n2dShaderProgram* pd = sw->GetDefaultProgram();
		//pd->Use();

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
	natRefPointer<n2dTexture2D>		m_texture2;
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
		Global::g_LogFile.open("Log.log");
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
			Global::g_pEngine->GetLogger().LogErr("Unknown exception"_nv);
		}
		MessageBox(NULL, _T("Unknown exception"), _T("Uncaught exception"), MB_OK | MB_ICONERROR);
	}
	
	//_CrtDumpMemoryLeaks();
	return 0;
}

void custom_invalid_parameter(LPCTSTR expression, LPCTSTR function, LPCTSTR file, nuInt line, uintptr_t /*pReserved*/)
{
	if (Global::g_pEngine)
	{
		std::basic_stringstream<nTChar> ss;

		ss << "In " << file << ", line " << line << ": " << expression;

		natException ex(nString{ WideStringView{ function } }, nString{ WideStringView{ file } }, line, ss.str().c_str());
		n2dGlobal::natExceptionEvent event(ex);
		Global::g_pEngine->GetEventBus().Post<n2dGlobal::natExceptionEvent>(event);
	}
}
