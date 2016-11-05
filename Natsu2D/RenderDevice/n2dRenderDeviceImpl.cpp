#include "n2dRenderDeviceImpl.h"
#include "OpenGL.h"
#include "../Engine/n2dEngineImpl.h"
#include "n2dGraphicsImpl.h"
#include <Windows.h>
#include "n2dTextureImpl.h"
#include "n2dModelLoaderImpl.h"
#include "n2dObjLoader.h"
#include "n2dBufferImpl.h"
#include "n2dLightControllerImpl.h"
#include "n2dMotionManagerImpl.h"
#include "n2dLayerImpl.h"
#include "../Renderer/n2dFontImpl.h"

n2dRenderDeviceImpl::n2dRenderDeviceImpl(n2dEngine* GLApp)
	: m_pEngine(GLApp),
	m_Shader(nullptr),
	m_bUpdated(false),
	m_MaxLights(0u)
{
	m_ModelMatStack.push(natMat4<>(1.0f));
	m_ViewMatStack.push(natMat4<>(1.0f));
	m_ProjMatStack.push(natMat4<>(1.0f));
	m_MVPMat = GetCurProjMat() * GetCurViewMat() * GetCurModelMat();
}

n2dRenderDeviceImpl::~n2dRenderDeviceImpl()
{
}

void n2dRenderDeviceImpl::MakeCurrent()
{
	wglMakeCurrent(m_pEngine->GetWindow()->GetDC(), m_pEngine->GetWindow()->GetRC());
}

void n2dRenderDeviceImpl::Clear(nuInt clearBit)
{
	glClear(clearBit);
}

void n2dRenderDeviceImpl::EnableCapability(Capability capability)
{
	glEnable(GetCapabilityEnum(capability));
}

void n2dRenderDeviceImpl::DisableCapability(Capability capability)
{
	glDisable(GetCapabilityEnum(capability));
}

void n2dRenderDeviceImpl::EnableCapabilityI(CapabilityI capability, nuInt Index)
{
	glEnablei(GetCapabilityIEnum(capability), Index);
}

void n2dRenderDeviceImpl::DisableCapabilityI(CapabilityI capability, nuInt Index)
{
	glDisablei(GetCapabilityIEnum(capability), Index);
}

nBool n2dRenderDeviceImpl::IsCapabilityEnabled(Capability capability) const
{
	return glIsEnabled(GetCapabilityEnum(capability)) == GL_TRUE;
}

nBool n2dRenderDeviceImpl::IsCapabilityIEnabled(CapabilityI capability, nuInt Index) const
{
	return glIsEnabledi(GetCapabilityIEnum(capability), Index) == GL_TRUE;
}

void n2dRenderDeviceImpl::SetBlendMode(BlendFactor Source, BlendFactor Destination)
{
	glBlendFunc(GetBlendFactorEnum(Source), GetBlendFactorEnum(Destination));
}

void n2dRenderDeviceImpl::SetBlendModeI(n2dBuffer* Buf, BlendFactor Source, BlendFactor Destination)
{
	if (!Buf || Buf->GetTarget() != n2dBuffer::BufferTarget::DrawIndirectBuffer)
	{
		nat_Throw(natException, _T("Buf is not a draw buffer"));
	}

	glBlendFunci(Buf->GetBuffer(), GetBlendFactorEnum(Source), GetBlendFactorEnum(Destination));
}

void n2dRenderDeviceImpl::SetBlendColor(natVec4<> const& Color)
{
	SetBlendColor(Color.r, Color.g, Color.b, Color.a);
}

void n2dRenderDeviceImpl::SetBlendColor(nFloat r, nFloat g, nFloat b, nFloat a)
{
	glBlendColor(r, g, b, a);
}

void n2dRenderDeviceImpl::SetSwapInterval(nuInt Interval)
{
	wglSwapIntervalEXT(Interval);
}

void n2dRenderDeviceImpl::SwapBuffers()
{
	m_pEngine->SwapBuffers();
}

n2dShaderWrapper* n2dRenderDeviceImpl::GetShaderWrapper()
{
	if (!m_Shader)
	{
		m_Shader = make_ref<n2dShaderWrapperImpl>(this);
	}

	return m_Shader;
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

natMat4<> const& n2dRenderDeviceImpl::GetCurModelMat() const
{
	return m_ModelMatStack.top();
}

natMat4<> const& n2dRenderDeviceImpl::GetCurViewMat() const
{
	return m_ViewMatStack.top();
}

natMat4<> const& n2dRenderDeviceImpl::GetCurProjMat() const
{
	return m_ProjMatStack.top();
}

natMat4<> const& n2dRenderDeviceImpl::GetMVPMat()
{
	updateMVP();
	
	return m_MVPMat;
}

n2dEngine* n2dRenderDeviceImpl::GetEngine()
{
	return m_pEngine;
}

nuInt n2dRenderDeviceImpl::GetMaxLight() const
{
	return m_MaxLights;
}

void n2dRenderDeviceImpl::SetMaxLights(nuInt value)
{
	if (m_pLightBuffer || m_MaxLights > 0u)
	{
		return;
	}

	m_MaxLights = value;
	m_Lights.resize(m_MaxLights);
	m_pLightBuffer = make_ref<n2dBufferImpl>(n2dBuffer::BufferTarget::UniformBuffer, static_cast<n2dShaderWrapperImpl*>(GetShaderWrapper()));
	m_pLightBuffer->AllocData(sizeof(n2dLightController::LightProperties) * m_MaxLights, nullptr, n2dBuffer::BufferUsage::DynamicDraw);
	m_pLightBuffer->BindBase(1u);
}

n2dLightController* n2dRenderDeviceImpl::GetLightController(nuInt Index)
{
	if (m_MaxLights == 0u || !m_pLightBuffer || Index >= m_MaxLights)
	{
		return nullptr;
	}

	natRefPointer<n2dLightControllerImpl> pLight;

	pLight = m_Lights[Index];
	if (pLight)
		return pLight;
	
	return m_Lights[Index] = make_ref<n2dLightControllerImpl>(Index, m_pLightBuffer);
}

nResult n2dRenderDeviceImpl::CreateBuffer(n2dBuffer::BufferTarget DefaultTarget, n2dBuffer** pOut)
{
	if (pOut == nullptr)
	{
		return NatErr_InvalidArg;
	}

	try
	{
		*pOut = new n2dBufferImpl(DefaultTarget, m_Shader);
	}
	catch (std::bad_alloc&)
	{
		nat_Throw(natException, _T("Failed to allocate memory"));
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
}

nResult n2dRenderDeviceImpl::CreateLayerMgr(n2dLayerMgr** pOut)
{
	if (pOut == nullptr)
	{
		return NatErr_InvalidArg;
	}

	try
	{
		*pOut = new n2dLayerMgrImpl;
	}
	catch (std::bad_alloc&)
	{
		nat_Throw(natException, _T("Failed to allocate memory"));
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
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
		nat_Throw(natException, _T("Failed to allocate memory"));
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
}

nResult n2dRenderDeviceImpl::CreateGraphics3D(n2dGraphics3D** pOut)
{
	if (pOut == nullptr)
	{
		return NatErr_InvalidArg;
	}

	try
	{
		*pOut = new n2dGraphics3DImpl(this);
	}
	catch (std::bad_alloc&)
	{
		nat_Throw(natException, _T("Failed to allocate memory"));
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
		nat_Throw(natException, _T("Failed to allocate memory"));
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
		nat_Throw(natException, _T("Failed to allocate memory"));
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
		*pOut = new n2dModelLoaderImpl(this);
	}
	catch (std::bad_alloc&)
	{
		nat_Throw(natException, _T("Failed to allocate memory"));
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
		nat_Throw(natException, _T("Failed to allocate memory"));
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
}

nResult n2dRenderDeviceImpl::CreateMotionManager(n2dMotionManager ** pOut)
{
	if (pOut == nullptr)
	{
		return NatErr_InvalidArg;
	}

	try
	{
		*pOut = new n2dMotionManagerImpl;
	}
	catch (std::bad_alloc&)
	{
		nat_Throw(natException, _T("Failed to allocate memory"));
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
}

nResult n2dRenderDeviceImpl::CreateFontManager(n2dFont** pOut)
{
	if (pOut == nullptr)
	{
		return NatErr_InvalidArg;
	}

	try
	{
		*pOut = new n2dFontImpl(this);
	}
	catch (std::bad_alloc&)
	{
		nat_Throw(natException, _T("Failed to allocate memory"));
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
}

void n2dRenderDeviceImpl::updateMVP()
{
	if (m_bUpdated)
		return;
	
	m_MVPMat = GetCurProjMat() * GetCurViewMat() * GetCurModelMat();
	m_bUpdated = true;
	if (!m_pMVPBuffer)
	{
		m_pMVPBuffer = make_ref<n2dBufferImpl>(n2dBuffer::BufferTarget::UniformBuffer, static_cast<n2dShaderWrapperImpl*>(GetShaderWrapper()));
		m_pMVPBuffer->BindBase(0u);
	}

	m_pMVPBuffer->AllocData(sizeof(natMat4<>), reinterpret_cast<ncData>(&m_MVPMat), n2dBuffer::BufferUsage::DynamicDraw);
}
