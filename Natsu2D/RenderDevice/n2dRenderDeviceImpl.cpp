#include "n2dRenderDeviceImpl.h"
#include "..\Engine\n2dEngineImpl.h"
#include "n2dGraphicsImpl.h"
#include <Windows.h>
#include "n2dTextureImpl.h"
#include "n2dModelLoaderImpl.h"
#include "n2dObjLoader.h"

n2dRenderDeviceImpl::n2dRenderDeviceImpl(n2dEngine* GLApp)
	: m_pEngine(GLApp),
	m_bUpdated(false)
{
	m_ModelMatStack.push(natMat4<>(1.0f));
	m_ViewMatStack.push(natMat4<>(1.0f));
	m_ProjMatStack.push(natMat4<>(1.0f));
	m_MVPMat = GetCurProjMat() * GetCurViewMat() * GetCurModelMat();
}

void n2dRenderDeviceImpl::MakeCurrent()
{
	wglMakeCurrent(m_pEngine->GetWindow()->GetDC(), m_pEngine->GetWindow()->GetRC());
}

void n2dRenderDeviceImpl::SwapBuffers()
{
	m_pEngine->SwapBuffers();
}

n2dShaderWrapper* n2dRenderDeviceImpl::GetShaderWrapper()
{
	return &m_Shader;
}

void n2dRenderDeviceImpl::SubmitModelMat(natMat4<> const& Mat)
{
	m_ModelMatStack.top() = Mat;
	m_bUpdated = false;
}

void n2dRenderDeviceImpl::SubmitViewMat(natMat4<> const& Mat)
{
	m_ViewMatStack.top() = Mat;
	m_bUpdated = false;
}

void n2dRenderDeviceImpl::SubmitProjMat(natMat4<> const& Mat)
{
	m_ProjMatStack.top() = Mat;
	m_bUpdated = false;
}

void n2dRenderDeviceImpl::PushModelMat()
{
	m_ModelMatStack.emplace(m_ModelMatStack.top());
}

void n2dRenderDeviceImpl::PushViewMat()
{
	m_ViewMatStack.emplace(m_ViewMatStack.top());
}

void n2dRenderDeviceImpl::PushProjMat()
{
	m_ProjMatStack.emplace(m_ProjMatStack.top());
}

nBool n2dRenderDeviceImpl::PopModelMat()
{
	if (m_ModelMatStack.size() > 1u)
	{
		m_ModelMatStack.pop();
		m_bUpdated = false;
		return true;
	}

	return false;
}

nBool n2dRenderDeviceImpl::PopViewMat()
{
	if (m_ViewMatStack.size() > 1u)
	{
		m_ViewMatStack.pop();
		m_bUpdated = false;
		return true;
	}

	return false;
}

nBool n2dRenderDeviceImpl::PopProjMat()
{
	if (m_ProjMatStack.size() > 1u)
	{
		m_ProjMatStack.pop();
		m_bUpdated = false;
		return true;
	}

	return false;
}

void n2dRenderDeviceImpl::PushMVPMat()
{
	PushModelMat();
	PushViewMat();
	PushProjMat();
}

nBool n2dRenderDeviceImpl::PopMVPMat()
{
	nBool bRet = PopModelMat();
	bRet &= PopViewMat();
	bRet &= PopProjMat();
	return bRet;
}

void n2dRenderDeviceImpl::InitMVPMat()
{
	SubmitModelMat(natMat4<>(1.0f));
	SubmitViewMat(natMat4<>(1.0f));
	SubmitProjMat(natMat4<>(1.0f));
}

natMat4<> n2dRenderDeviceImpl::GetCurModelMat() const
{
	return m_ModelMatStack.top();
}

natMat4<> n2dRenderDeviceImpl::GetCurViewMat() const
{
	return m_ViewMatStack.top();
}

natMat4<> n2dRenderDeviceImpl::GetCurProjMat() const
{
	return m_ProjMatStack.top();
}

natMat4<> const& n2dRenderDeviceImpl::GetMVPMat()
{
	if (!m_bUpdated)
	{
		updateMVP();
	}
	
	return m_MVPMat;
}

n2dEngine* n2dRenderDeviceImpl::GetEngine()
{
	return m_pEngine;
}

nResult n2dRenderDeviceImpl::CreateGraphics2D(n2dGraphics2D** pOut)
{
	if (pOut == nullptr)
	{
		return NatErr_InvalidArg;
	}

	try
	{
		*pOut = new n2dGraphics2DImpl(this);
	}
	catch (std::bad_alloc&)
	{
		natException e(_T("n2dRenderDeviceImpl::CreateGraphics2D"), _T("Failed to allocate memory"));
		global::EventException(&e);
	}
	catch (natException& e)
	{
		global::EventException(&e);
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
}

nResult n2dRenderDeviceImpl::CreateTexture(n2dTexture2D** pOut)
{
	if (pOut == nullptr)
	{
		return NatErr_InvalidArg;
	}

	try
	{
		*pOut = new n2dTexture2DImpl;
	}
	catch (std::bad_alloc&)
	{
		natException e(_T("n2dRenderDeviceImpl::CreateTexture"), _T("Failed to allocate memory"));
		global::EventException(&e);
	}
	catch (natException& e)
	{
		global::EventException(&e);
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
}

nResult n2dRenderDeviceImpl::CreateTextureFromStream(natStream* pStream, DWORD dwFileType, n2dTexture2D** pOut)
{
	if (pOut == nullptr)
	{
		return NatErr_InvalidArg;
	}

	try
	{
		nResult tRet;
		if (NATFAIL(tRet = CreateTexture(pOut)))
		{
			return tRet;
		}

		(*pOut)->LoadTexture(pStream, dwFileType);
	}
	catch (std::bad_alloc&)
	{
		natException e(_T("n2dRenderDeviceImpl::CreateTextureFromStream"), _T("Failed to allocate memory"));
		global::EventException(&e);
	}
	catch (natException& e)
	{
		global::EventException(&e);
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
}

nResult n2dRenderDeviceImpl::CreateModelLoader(n2dModelLoader** pOut)
{
	if (pOut == nullptr)
	{
		return NatErr_InvalidArg;
	}

	try
	{
		*pOut = new n2dModelLoaderImpl;
	}
	catch (std::bad_alloc&)
	{
		natException e(_T("n2dRenderDeviceImpl::CreateModelLoader"), _T("Failed to allocate memory"));
		global::EventException(&e);
	}
	catch (natException& e)
	{
		global::EventException(&e);
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
}

nResult n2dRenderDeviceImpl::CreateObjLoader(n2dModelLoader** pOut)
{
	if (pOut == nullptr)
	{
		return NatErr_InvalidArg;
	}

	try
	{
		*pOut = new n2dObjLoader;
	}
	catch (std::bad_alloc&)
	{
		natException e(_T("n2dRenderDeviceImpl::CreateObjLoader"), _T("Failed to allocate memory"));
		global::EventException(&e);
	}
	catch (natException& e)
	{
		global::EventException(&e);
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
}

void n2dRenderDeviceImpl::updateMVP()
{
	m_MVPMat = GetCurProjMat() * GetCurViewMat() * GetCurModelMat();
	m_bUpdated = true;
}