#include "n2dWindowImpl.h"
#include <natException.h>
#include "natLog.h"
#include "n2dEngineImpl.h"
#include "../RenderDevice/n2dRenderDeviceImpl.h"
#include <natTransform.h>

n2dWindowImpl::n2dWindowImpl(n2dEngineImpl* pEngine, nuInt X, nuInt Y, nuInt WindowWidth, nuInt WindowHeight, nuInt ScreenWidth, nuInt ScreenHeight, nuInt BitsPerPixel, nBool fullscreen)
	: m_hWnd(NULL),
	m_hDC(NULL),
	m_hRC(NULL),
	m_pEngine(pEngine),
	m_WindowPosX(X),
	m_WindowPosY(Y),
	m_WindowWidth(WindowWidth),
	m_WindowHeight(WindowHeight),
	m_ScreenWidth(ScreenWidth),
	m_ScreenHeight(ScreenHeight),
	m_BitsPerPixel(BitsPerPixel <= 16u ? 16u : 32u),
	m_IsFullScreen(fullscreen)
{
}

n2dWindowImpl::~n2dWindowImpl()
{
	n2dWindowImpl::Destroy();
}

nBool n2dWindowImpl::Create(nStrView title, nStrView classname, HINSTANCE hInstance, LPVOID lpParam)
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_STEREO | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		static_cast<BYTE>(m_BitsPerPixel),
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		32, // 深度缓存
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;

	if (m_IsFullScreen)
	{
		if (!ChangeScreenSetting())
		{
			m_IsFullScreen = false;
		}
		else
		{
			ShowCursor(FALSE);
			windowStyle = WS_POPUP;
			windowExtendedStyle |= WS_EX_TOPMOST;
		}
	}
	else
	{
		ChangeDisplaySettings(nullptr, 0);
		ShowCursor(TRUE);
	}

	RECT windowRECT = { static_cast<LONG>(RawGetPosX()), static_cast<LONG>(RawGetPosY()), static_cast<LONG>(RawGetPosX() + RawGetWidth()), static_cast<LONG>(RawGetPosY() + RawGetHeight()) };

	if (!m_IsFullScreen)
	{
		AdjustWindowRectEx(&windowRECT, windowStyle, false, windowExtendedStyle);

		if (windowRECT.left < 0)
		{
			windowRECT.right -= windowRECT.left;
			windowRECT.left = 0;
		}

		if (windowRECT.top < 0)
		{
			windowRECT.bottom -= windowRECT.top;
			windowRECT.top = 0;
		}
	}

	m_hWnd = CreateWindowEx(
		windowExtendedStyle,
		WideString{ classname }.data(),
		WideString{ title }.data(),
		windowStyle,
		windowRECT.left,
		windowRECT.top,
		windowRECT.right - windowRECT.left,
		windowRECT.bottom - windowRECT.top,
		HWND_DESKTOP,
		NULL,
		hInstance,
		lpParam);

	while (m_hWnd)
	{
		m_hDC = ::GetDC(m_hWnd);
		if (!m_hDC)
		{
			break;
		}

		auto PixelFormat = ChoosePixelFormat(m_hDC, &pfd);
		if (!PixelFormat)
		{
			break;
		}

		if (!SetPixelFormat(m_hDC, PixelFormat, &pfd))
		{
			break;
		}

		m_hRC = wglCreateContext(m_hDC);
		if (!m_hRC)
		{
			break;
		}

		if (!wglMakeCurrent(m_hDC, m_hRC))
		{
			break;
		}

		//ShowWindow(m_hWnd, SW_NORMAL);
		ReshapeGL();
		return true;
	}

	Destroy();
	return false;
}

void n2dWindowImpl::Show(nBool show)
{
	ShowWindow(m_hWnd, show ? SW_NORMAL : SW_HIDE);
	ReshapeGL();
}

void n2dWindowImpl::Destroy()
{
	if (m_IsFullScreen)
	{
		ChangeDisplaySettings(nullptr, 0);
		ShowCursor(TRUE);
	}

	if (m_hWnd != nullptr)
	{
		if (m_hDC != nullptr)
		{
			wglMakeCurrent(m_hDC, nullptr);
			if (m_hRC != nullptr)
			{
				wglDeleteContext(m_hRC);
				m_hRC = nullptr;
			}
			ReleaseDC(m_hWnd, m_hDC);
			m_hDC = nullptr;
		}
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}
}

nBool n2dWindowImpl::ChangeScreenSetting()
{
	DEVMODE dm = { 0 };
	dm.dmSize = sizeof(DEVMODE);
	dm.dmPelsWidth = GetWidth();
	dm.dmPelsHeight = GetHeight();
	dm.dmBitsPerPel = m_BitsPerPixel;
	dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	if (ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		m_pEngine->GetLogger().LogWarn("转换分辨率失败"_nv);
		return false;
	}

	return true;
}

void n2dWindowImpl::ReshapeGL()
{
	GLsizei width(GetWidth());
	GLsizei height(GetHeight());
	glViewport(0, 0, width, height);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	if (height > 0)
		m_pEngine->GetRenderDevice()->SubmitProjMat(natTransform::perspective(45.0f, float(width) / float(height), 0.1f, 100.0f));
	//gluPerspective(45., static_cast<double>(width) / static_cast<double>(height), 1., 100.0);
	//glMatrixMode(GL_MODELVIEW);
	
	//glLoadIdentity();
}

void n2dWindowImpl::SwapBuffers()
{
	::SwapBuffers(m_hDC);
}

void n2dWindowImpl::SetPosX(nuInt X)
{
	RawSetPosX(X);
	if (!m_IsFullScreen)
		ApplyPosition();
}

nuInt n2dWindowImpl::GetPosX()
{
	UpdatePosition();
	return RawGetPosX();
}

void n2dWindowImpl::SetPosY(nuInt Y)
{
	RawSetPosY(Y);
	if (!m_IsFullScreen)
		ApplyPosition();
}

nuInt n2dWindowImpl::GetPosY()
{
	UpdatePosition();
	return RawGetPosY();
}

void n2dWindowImpl::SetWidth(nuInt Width)
{
	RawSetWidth(Width);
	ApplyPosition();
}

nuInt n2dWindowImpl::GetWidth()
{
	UpdatePosition();
	return RawGetWidth();
}

void n2dWindowImpl::SetHeight(nuInt Height)
{
	RawSetHeight(Height);
	ApplyPosition();
}

nuInt n2dWindowImpl::GetHeight()
{
	UpdatePosition();
	return RawGetHeight();
}

void n2dWindowImpl::SetHiColor()
{
	m_BitsPerPixel = 16u;
}

void n2dWindowImpl::SetTrueColor()
{
	m_BitsPerPixel = 32u;
}

void n2dWindowImpl::SetFullScreen(nBool mode)
{
	m_IsFullScreen = mode;
}

nBool n2dWindowImpl::GetFullScreen()
{
	return m_IsFullScreen;
}

void n2dWindowImpl::SetColorType(colorType ct)
{
	switch (ct)
	{
	case colorType::HiColor:
		SetHiColor();
		break;
	case colorType::TrueColor:
		SetTrueColor();
		break;
	default:
		break;
	}
}

n2dWindowImpl::colorType n2dWindowImpl::GetColorType()
{
	switch (m_BitsPerPixel)
	{
	case 16u:
		return colorType::HiColor;
	case 32u:
		return colorType::TrueColor;
	default:
		nat_Throw(natException, "Not a enum of ColorType"_nv);
	}
}

HWND n2dWindowImpl::GetWnd()
{
	return m_hWnd;
}

HDC n2dWindowImpl::GetDC()
{
	return m_hDC;
}

HGLRC n2dWindowImpl::GetRC()
{
	return m_hRC;
}

n2dEngineImpl* n2dWindowImpl::GetEngine()
{
	return m_pEngine;
}

void n2dWindowImpl::RawSetPosX(nuInt value)
{
	if (!m_IsFullScreen)
		m_WindowPosX = value;
}

nuInt n2dWindowImpl::RawGetPosX() const
{
	return m_IsFullScreen ? 0 : m_WindowPosX;
}

void n2dWindowImpl::RawSetPosY(nuInt value)
{
	if (!m_IsFullScreen)
		m_WindowPosY = value;
}

nuInt n2dWindowImpl::RawGetPosY() const
{
	return m_IsFullScreen ? 0 : m_WindowPosY;
}

void n2dWindowImpl::RawSetWidth(nuInt value)
{
	if (m_IsFullScreen)
		m_ScreenWidth = value;
	else
		m_WindowWidth = value;
}

nuInt n2dWindowImpl::RawGetWidth() const
{
	return m_IsFullScreen ? m_ScreenWidth : m_WindowWidth;
}

void n2dWindowImpl::RawSetHeight(nuInt value)
{
	if (m_IsFullScreen)
		m_ScreenHeight = value;
	else
		m_WindowHeight = value;
}

nuInt n2dWindowImpl::RawGetHeight() const
{
	return m_IsFullScreen ? m_ScreenHeight : m_WindowHeight;
}

void n2dWindowImpl::UpdatePosition()
{
	RECT windowRect{ 0 };
	GetWindowRect(m_hWnd, &windowRect);

	RawSetPosX(static_cast<nuInt>(windowRect.left));
	RawSetPosY(static_cast<nuInt>(windowRect.top));
	RawSetWidth(static_cast<nuInt>(windowRect.right - windowRect.left));
	RawSetHeight(static_cast<nuInt>(windowRect.bottom - windowRect.top));
}

void n2dWindowImpl::ApplyPosition()
{
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;

	if (m_IsFullScreen)
	{
		if (!ChangeScreenSetting())
		{
			m_IsFullScreen = false;
		}
		else
		{
			ShowCursor(FALSE);
			windowStyle = WS_POPUP;
			windowExtendedStyle |= WS_EX_TOPMOST;
		}
	}
	else
	{
		ChangeDisplaySettings(nullptr, 0);
		ShowCursor(TRUE);
	}

	RECT windowRECT = { static_cast<LONG>(RawGetPosX()), static_cast<LONG>(RawGetPosY()), static_cast<LONG>(RawGetPosX() + RawGetWidth()), static_cast<LONG>(RawGetPosY() + RawGetHeight()) };

	if (!m_IsFullScreen)
	{
		if (!AdjustWindowRectEx(&windowRECT, windowStyle, false, windowExtendedStyle))
		{
			nat_Throw(natWinException, "AdjustWindowRectEx failed."_nv);
		}

		if (windowRECT.left < 0)
		{
			windowRECT.right -= windowRECT.left;
			windowRECT.left = 0;
		}

		if (windowRECT.top < 0)
		{
			windowRECT.bottom -= windowRECT.top;
			windowRECT.top = 0;
		}
	}

	if (!SetWindowPos(m_hWnd, NULL, windowRECT.left, windowRECT.top, windowRECT.right - windowRECT.left, windowRECT.bottom - windowRECT.top, SWP_NOACTIVATE | SWP_NOZORDER))
	{
		nat_Throw(natWinException, "SetWindowPos failed."_nv);
	}
}
