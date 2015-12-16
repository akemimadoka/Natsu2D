#pragma region 预编译

#include <Windows.h>
#include <iostream>
#include <sstream>

#include "resource.h"

//#define Natsu2DStatic 1
#include "..\Natsu2D\Natsu2D.h"

#ifdef _DEBUG
#	ifdef Natsu2DStatic
#		pragma comment (lib, "../Natsu2D/bin/Natsu2D_ds.lib")
#	else
#		pragma comment (lib, "../Natsu2D/bin/Natsu2D_d.lib")
#	endif
#	pragma comment (lib, "../NatsuLib/bin/NatsuLib_d.lib")
#	pragma comment (lib, "../Natsu2D/extern/glew/bin/Debug/glew.lib")
#else
#	ifdef Natsu2DStatic
#		pragma comment (lib, "../Natsu2D/bin/Natsu2D_s.lib")
#	else
#		pragma comment (lib, "../Natsu2D/bin/Natsu2D.lib")
#	endif
#	pragma comment (lib, "../NatsuLib/bin/NatsuLib.lib")
#	pragma comment (lib, "../Natsu2D/extern/glew/bin/Release/glew.lib")
#endif

#pragma comment (lib, "opengl32.lib")

#include <n2dFont.h>

#include <n2dCommon.h>
#include <n2dFPSController.h>
#include <n2dRenderDevice.h>
#include <n2dGraphics.h>
#include <n2dUtil.h>
#include <n2dTexture.h>
#include <n2dModel.h>

#include <natException.h>
#include <natMath.h>
#include <natLog.h>
#include <natStream.h>

#pragma endregion

void custom_invalid_parameter(ncTStr expression, ncTStr function, ncTStr file, nuInt line, uintptr_t pReserved);

///	@brief	测试用App类
///	@note	仅用于测试
class test final
	: public n2dEngineEventListener
{
public:
	static void MouseClick(n2dEngine::WndMsgEvent& e)
	{
		test* pTest = dynamic_cast<test*>(e.GetEngine()->GetListener());
		if (!pTest)
		{
			return;
		}

		pTest->m_bShouldControl = !pTest->m_bShouldControl;
		pTest->m_Speed = natVec3<nFloat>(0.0f);
	}

	static void MouseMove(n2dEngine::WndMsgEvent& e)
	{
		test* pTest = dynamic_cast<test*>(e.GetEngine()->GetListener());
		if (!pTest)
		{
			return;
		}

		if (pTest->m_bShouldControl)
		{
			POINTS mp = MAKEPOINTS(e.GetData().lParam);

			pTest->m_Speed.x = -10.0f + 20.0f * mp.x / pTest->m_pEngine->GetWindow()->Width;
			pTest->m_Speed.y = 10.0f - 20.0f * mp.y / pTest->m_pEngine->GetWindow()->Height;
		}
	}

	static void MouseWheel(n2dEngine::WndMsgEvent& e)
	{
		test* pTest = dynamic_cast<test*>(e.GetEngine()->GetListener());
		if (!pTest)
		{
			return;
		}

		auto Delta = static_cast<nShort>(HIWORD(e.GetData().wParam)) / WHEEL_DELTA;
		
		//pTest->m_Speed.z = 10.0f ;
		// 立即改变
		natMat4<> tmat(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, Delta * 10.f, 1.0f
			);
		pTest->m_pEngine->GetRenderDevice()->SubmitViewMat(pTest->m_pEngine->GetRenderDevice()->GetCurViewMat() * tmat);
	}

	static void KeyDown(n2dEngine::WndMsgEvent& e)
	{
		switch (e.Data.wParam)
		{
		case VK_F1:
			e.GetEngine()->ToggleFullscreen();
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
			throw natException(_T("test::test"), _T("Failed to create Natsu2D engine"));
		}

		m_pEngine->GetRenderDevice()->CreateGraphics2D(&m_pGraphics);
		m_pEngine->GetRenderDevice()->CreateTexture(&m_texture);
		m_pEngine->GetRenderDevice()->CreateTexture(&m_texture2);
		m_pEngine->GetRenderDevice()->CreateTexture(&m_texture3);
		m_pEngine->GetRenderDevice()->CreateModelLoader(&m_model);
		m_pEngine->GetRenderDevice()->CreateModelLoader(&m_model2);
		m_pEngine->GetRenderDevice()->CreateModelLoader(&m_model3);

		//n2dGlobal::pCurrentApp = m_pEngine;
		m_pEngine->MainLoop(_T("夏之幻想"), 6000u);
	}
	~test()
	{

	}

	nBool EngineInit() override
	{
		return true;
	}

	void EngineUninit() override
	{

	}

	struct Test
	{
		int first;
		n2dGraphics3DVertex vert;
	};

	nBool WindowInit() override
	{
		if (!m_pGraphics3D)
		{
			m_pEngine->GetRenderDevice()->CreateGraphics3D(&m_pGraphics3D);
		}

		natLog::GetInstance().LogMsg(n2dUtil::FormatString(TEXT("GLAPP initialized as %s thread mode"), (m_pEngine->GetThreadMode() == n2dEngine::ThreadMode::SingleThread ? TEXT("single") : TEXT("multi"))));
		natLog::GetInstance().LogMsg(_T("GLAPP start initializing"));
		m_Mutex = CreateMutex(NULL, FALSE, _T("GLAPP"));
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(m_Mutex);
			throw natException(_T("test::WindowInit"), _T("GLAPP is already running"));
		}
		if (!m_Mutex)
		{
			throw natException(_T("test::WindowInit"), _T("Cannot create mutex"));
		}

		n2dWindow* window = m_pEngine->GetWindow();
		n2dRenderDevice* renderdevice = m_pEngine->GetRenderDevice();

		if (!window->FullScreen)
		{
			SetWindowLong(window->GetWnd(), GWL_STYLE, GetWindowLong(window->GetWnd(), GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX);
			HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
			SendMessage(window->GetWnd(), WM_SETICON, TRUE, LPARAM(hIcon));
			SendMessage(window->GetWnd(), WM_SETICON, FALSE, LPARAM(hIcon));
		}

		SetActiveWindow(window->GetWnd());

		renderdevice->SetSwapInterval(1u);

		renderdevice->EnableCapability(n2dRenderDevice::Capability::Blend);
		renderdevice->SetBlendMode(n2dRenderDevice::BlendFactor::SrcAlpha, n2dRenderDevice::BlendFactor::OneMinusSrcAlpha);

		sw = renderdevice->GetShaderWrapper();
		sw->CreateProgram(&sp);
		n2dShader* pShader[2];
		natStream* pStream = new natFileStream(_T("VertexShader.glsl"), true, false);
		sw->CreateShaderFromStream(pStream, n2dShader::ShaderType::Vertex, false, &pShader[0]);
		if (!pShader[0]->Compiled())
		{
			throw natException(_T("test::WindowInit"), natUtil::FormatString(_T("Compile Shader0 Failed, Log: %s"), pShader[0]->GetInfoLog()));
		}

		SafeRelease(pStream);
		pStream = new natFileStream(_T("FragmentShader.glsl"), true, false);
		sw->CreateShaderFromStream(pStream, n2dShader::ShaderType::Fragment, false, &pShader[1]);
		if (!pShader[1]->Compiled())
		{
			throw natException(_T("test::WindowInit"), natUtil::FormatString(_T("Compile Shader1 Failed, Log: %s"), pShader[1]->GetInfoLog()));
		}

		SafeRelease(pStream);
		sp->AttachShader(pShader[0]);
		sp->AttachShader(pShader[1]);

		sp->Link();
		if (!sp->IsLinked())
		{
			throw natException(_T("test::WindowInit"), natUtil::FormatString(_T("Link ShaderProgram Failed, Log: %s"), sp->GetInfoLog()));
		}

		sp->Use();

		sp->DetachShader(pShader[0]);
		sp->DetachShader(pShader[1]);

		SafeRelease(pShader[0]);
		SafeRelease(pShader[1]);

		Matrix = sp->GetUniformReference(_T("MVP"));
		/*posID = glGetSubroutineUniformLocation(programID, n2dShaderWrapperImpl::Vertex, "positionShader");
		if (posID >= 0)
		{
			GLuint def = glGetSubroutineIndex(programID, n2dShaderWrapperImpl::Vertex, "DefaultPositionFunc");
			if (def == GL_INVALID_INDEX)
			{
				throw natException(_T("test::Initialize"), _T("Cannot get default position subroutine"));
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
		//TimeID = glGetUniformLocation(programID, "Time");

		n2dBuffer* pBuf = nullptr;
		renderdevice->CreateBuffer(n2dBuffer::BufferTarget::ShaderStorageBuffer, &pBuf);
		pBuf->AllocData(sizeof(Test), nullptr, n2dBuffer::BufferUsage::DynamicCopy);
		pBuf->BindBase(0u);
		/*glGenBuffers(1, &TestID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, TestID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Test), nullptr, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0u, TestID);*/

		/*GLsizei length;
		GLchar tmpName[10] = "";*/
		/*glGetActiveUniformBlockName(programID, 0, 10, &length, tmpName);
		GLuint Materid = glGetUniformBlockIndex(programID, "Mater");
		GLuint Lightid = glGetUniformBlockIndex(programID, "Light");*/
		
		struct LightProperties
		{
			nBool isEnabled;
			nBool isLocal;
			nBool isSpot;
			natVec3<> ambient;
			natVec3<> color;
			natVec3<> position;
			natVec3<> halfVector;
			natVec3<> coneDirection;
			nFloat spotCosCutoff;
			nFloat spotExponent;
			nFloat constantAttenuation;
			nFloat linearAttenuation;
			nFloat quadraticAttenuation;
		};

		struct MaterialProperties
		{
			natVec4<> Diffuse;
			natVec4<> Specular;
			natVec4<> Ambient;
			natVec4<> Emission;
			nFloat Shininess;
			nFloat Strength;
		};

		//glGetError();
		/*n2dBuffer* pBuf = nullptr;
		renderdevice->CreateBuffer(n2dBuffer::UniformBuffer, &pBuf);
		pBuf->AllocData(sizeof(MaterialProperties), nullptr, n2dBuffer::StaticDraw);
		glBindBufferBase(GL_UNIFORM_BUFFER, Materid, pBuf->GetBuffer());*/
		//auto e = glGetError();
		pStream = pBuf->MapBuffer(n2dBuffer::BufferAccess::WriteOnly);
		MaterialProperties tm = { natVec4<>(), natVec4<>() ,natVec4<>() ,natVec4<>() ,1.0f,1.0f };
		pStream->WriteBytes(reinterpret_cast<ncData>(&tm), sizeof(MaterialProperties));
		pStream = nullptr;
		pBuf->UnmapBuffer();
		
		m_pEngine->AddMessageHandler(MouseClick, WM_LBUTTONDOWN);
		m_pEngine->AddMessageHandler(MouseMove, WM_MOUSEMOVE);
		m_pEngine->AddMessageHandler(MouseWheel, WM_MOUSEWHEEL);
		m_pEngine->AddMessageHandler(KeyDown, WM_KEYDOWN);

		// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> infinite units
		renderdevice->SubmitProjMat(natTransform::perspective(45.0f, 4.0f / 3.0f, 0.1f));
		// Camera matrix
		renderdevice->SubmitViewMat(natTransform::lookAt(
			natVec3<>(0, 100, 500), // Camera
			natVec3<>(0, 0, 0), // looks at the origin
			natVec3<>(0, 1, 0)  // Head
			));
		// Model matrix : an identity matrix (model will be at the origin)
		renderdevice->SubmitModelMat(natMat4<>(1.0f));

		/*GLuint light = glGetUniformBlockIndex(programID, "Lights");
		GLuint lb = 0u;
		glGenBuffers(1, &lb);
		glBindBuffer(GL_UNIFORM_BUFFER, lb);*/


		//f->InitFont(renderdevice, _T("fz.ttf"), 32, 32);
		//f->InitText(_T("23"), 2);
		if (!m_texture->LoadTexture(_T("texture.dds")))
		{
			throw natException(_T("test::WindowInit"), _T("Unable to load texture"));
		}
		if (!m_texture2->LoadTexture(_T("table.dds")))
		{
			throw natException(_T("test::WindowInit"), _T("Unable to load texture"));
		}
		
		if (!m_texture3->LoadTexture(_T("ch.png")))
		{
			throw natException(_T("test::WindowInit"), _T("Unable to load texture"));
		}

		MessageBox(window->GetWnd(), _T("即将开始加载模型，请等待\n按上下左右进行旋转，点击鼠标左键开始移动，滚动鼠标滚轮进行缩放"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		natLog::GetInstance().LogMsg(_T("Loading model"));
		m_model2->SetDefaultTexture(m_texture);
		if (NATFAIL(m_model2->LoadFromFile(_T("comb.obj"))))
		{
			throw natException(_T("test::WindowInit"), _T("Unable to load obj model"));
		}

		m_model->SetDefaultTexture(m_texture2);
		if (NATFAIL(m_model->LoadFromFile(_T("table.obj"))))
		{
			throw natException(_T("test::WindowInit"), _T("Unable to load obj model"));
		}

		natLog::GetInstance().LogMsg(_T("Loaded model successfully"));

		natLog::GetInstance().LogMsg(_T("GLAPP initialized successfully"));
		return true;
	}

	void WindowUninit() override
	{
		CloseHandle(m_Mutex);
		natLog::GetInstance().LogMsg(_T("GLAPP exit"));
	}

	void Update(nDouble, n2dFPSController*) override
	{
		if (m_pEngine->IsKeyPressed(VK_SPACE))
		{
			// Load default mat
			m_pEngine->GetRenderDevice()->SubmitViewMat(natTransform::lookAt(
				natVec3<>(0, 100, 500),
				natVec3<>(0, 0, 0),
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
			SetWindowText(m_pEngine->GetWindow()->GetWnd(), n2dUtil::FormatString(_T("夏之幻想 | FPS=%f"), pFPSController->FPS).c_str());
		}

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pRenderDevice->Clear(n2dRenderDevice::ClearBit_Color | n2dRenderDevice::ClearBit_Depth);

		if (NATFAIL(m_pGraphics->Begin()) || NATFAIL(m_pGraphics3D->Begin()))
		{
			throw natException(_T("test::Render"), _T("Failed to prepare graphic renderer"));
		}

		//glColor3f(1.f, 0.5f, 0.f);
		//f.PrintFont(m_pGraphics, _T("2333"), 0.f, 0.f, 10.f);
		//glUseProgram(programID);

		//m_pGraphics->DrawRaw(m_texture, m_model2);
		//m_pGraphics->DrawRaw(m_texture2, m_model);

		natVec3<> lightPos = natVec3<>(100, 100, 200);
		//glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		Light->SetValue(1u, &lightPos[0]);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform

		/*natMat4<> tmatL(
			cos(natTransform::DegtoRad(m_RotateL)),0, -sin(natTransform::DegtoRad(m_RotateL)),	0,
			0,									1, 0,											0,
			sin(natTransform::DegtoRad(m_RotateL)),0, cos(natTransform::DegtoRad(m_RotateL)),	0,
			0,									0, 0,											1
			);

		natMat4<> tmatH(
			1	,0,										0,										0,
			0,	cos(natTransform::DegtoRad(m_RotateH)),	sin(natTransform::DegtoRad(m_RotateH)),	0,
			0,	-sin(natTransform::DegtoRad(m_RotateH)),cos(natTransform::DegtoRad(m_RotateH)),	0,
			0,	0,										0,										1
			);

		natMat4<> tmat2(
			1.0f,		0.0f,		0.0f,		0.0f,
			0.0f,		1.0f,		0.0f,		0.0f,
			0.0f,		0.0f,		1.0f,		0.0f,
			m_Speed.x,	m_Speed.y,	m_Speed.z,	1.0f);*/
		
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

		Matrix->SetValue(1, &pRenderDevice->GetMVPMat()[0][0]);
		ModelMatrix->SetValue(1, &pRenderDevice->GetCurModelMat()[0][0]);
		ViewMatrix->SetValue(1, &pRenderDevice->GetCurViewMat()[0][0]);
		//glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &pRenderDevice->GetMVPMat()[0][0]);
		//glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &pRenderDevice->GetCurModelMat()[0][0]);
		//glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &pRenderDevice->GetCurViewMat()[0][0]);
		//glUniform1f(TimeID, static_cast<GLfloat>(pFPSController->GetTotalFrame()));

		m_pGraphics3D->RenderModel(m_model->GetModel());
		m_pGraphics3D->RenderModel(m_model2->GetModel());

		sp->Use();

		m_pGraphics3D->End();

		m_pGraphics->DrawQuad(m_texture3,
			n2dGraphics2DVertex{ natVec3<>(-100, 0, 200), 0, natVec2<>(0, 0) },
			n2dGraphics2DVertex{ natVec3<>(100, 0, 200), 0, natVec2<>(1, 0) },
			n2dGraphics2DVertex{ natVec3<>(100, 200, 200), 0, natVec2<>(1, 1) },
			n2dGraphics2DVertex{ natVec3<>(-100, 200, 200), 0, natVec2<>(0, 1) });

		n2dShaderProgram* pd = sw->GetDefaultProgram();
		pd->Use();
		auto um = pd->GetUniformReference(_T("MVP"));
		natMat4<> t;
		um->SetValue(1, &t/*pRenderDevice->GetMVPMat()[0][0]*/);

		m_pGraphics->End();

		//Test data;

		//GLint b = 0;
		//glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &b);
		/*if (b > 0)
		{
			memcpy_s(&data, sizeof(data), glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY), b);
		}

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);*/

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
	n2dShaderWrapper*				sw;
	n2dShaderProgram*				sp;

	n2dShaderProgram::UniformReference* Matrix;
	n2dShaderProgram::UniformReference* ViewMatrix;
	n2dShaderProgram::UniformReference* ModelMatrix;
	n2dShaderProgram::UniformReference* Light;

	GLuint							pipelineID;
	GLuint							programID;
	GLuint							posID;
	//GLuint						MatrixID;
	//GLuint						ViewMatrixID;
	//GLuint						ModelMatrixID;
	//GLuint						LightID;
	GLuint							TimeID;

	GLuint							TestID;

	HANDLE							m_Mutex;
	natRefPointer<n2dTexture2D>		m_texture;
	natRefPointer<n2dTexture2D>		m_texture2;
	natRefPointer<n2dTexture2D>		m_texture3;
	natRefPointer<n2dModelLoader>	m_model;
	natRefPointer<n2dModelLoader>	m_model2;
	natRefPointer<n2dModelLoader>	m_model3;
};

void LogEvent(natEvent<nTString>& e)
{
	//OutputDebugString((e.GetData() + TEXT("\n")).c_str());

#ifdef UNICODE
	std::wcout
#else
	std::cout
#endif
		<< e.GetData() << std::endl;
}

void ExceptionHandler(natEvent<natException*>& e)
{
	std::basic_stringstream<nTChar, std::char_traits<nTChar>, std::allocator<nTChar> > ss;
	const natException* ex = e.GetData();
	if (ex != nullptr)
	{
		ss << _T("Uncaught exception: In ") << ex->GetSource() << _T(" : ") << ex->GetDesc() << std::endl;

		while (ex->GetCausedbyException() != nullptr)
		{
			ex = ex->GetCausedbyException();
			ss << _T("Caused by exception: In ") << ex->GetSource() << _T(" : ") << ex->GetDesc() << std::endl;
		}

		natLog::GetInstance().LogErr(ss.str());

		MessageBox(nullptr, ss.str().c_str(), _T("Uncaught exception"), MB_OK | MB_ICONERROR);
	}

	//if (n2dGlobal::pCurrentApp != nullptr)
	//	n2dGlobal::pCurrentApp->TerminateApplication();

	exit(EXIT_FAILURE);
}

int main()
{
	try
	{
		//std::locale::global(std::locale(std::locale(), "", LC_CTYPE));
		std::locale::global(std::locale("", LC_CTYPE));
		_set_invalid_parameter_handler(custom_invalid_parameter);
		natLog::GetInstance().RegisterLogUpdateEventFunc(LogEvent);
		n2dGlobal::EventException += ExceptionHandler;

		test (GetModuleHandle(NULL));
	}
	catch (natException& ex)
	{
		n2dGlobal::EventException(&ex);
	}
	catch (...)
	{
		natLog::GetInstance().LogErr(TEXT("Unknown exception"));
		MessageBox(nullptr, TEXT("Unknown exception"), TEXT("Uncaught exception"), MB_OK | MB_ICONERROR);
	}

	return 0;
}

void custom_invalid_parameter(ncTStr expression, ncTStr function, ncTStr file, nuInt line, uintptr_t /*pReserved*/)
{
	std::basic_stringstream<nTChar, std::char_traits<nTChar>, std::allocator<nTChar> > ss;

	ss << "In " << file << ", line " << line << ": " << expression;

	natException ex(function, ss.str());
	n2dGlobal::EventException(&ex);
}