////////////////////////////////////////////////////////////////////////////////
///	@file	n2dRenderDeviceImpl.h
///	@brief	渲染设备相关
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\n2dRenderDevice.h"
#include "n2dShaderWrapperImpl.h"
#include <natType.h>
#include <natMath.h>
#include <stack>

struct n2dGraphics3D;
class n2dBufferImpl;
class n2dLightControllerImpl;

////////////////////////////////////////////////////////////////////////////////
///	@brief	渲染设备实现
////////////////////////////////////////////////////////////////////////////////
class n2dRenderDeviceImpl final
	: public natRefObjImpl<n2dRenderDevice>
{
	static GLenum GetCapabilityEnum(Capability cp)
	{
		switch (cp)
		{
		case n2dRenderDevice::Capability::Blend:
			return GL_BLEND;
		case n2dRenderDevice::Capability::ColorLogicOp:
			return GL_COLOR_LOGIC_OP;
		case n2dRenderDevice::Capability::CullFace:
			return GL_CULL_FACE;
		case n2dRenderDevice::Capability::DebugOutput:
			return GL_DEBUG_OUTPUT;
		case n2dRenderDevice::Capability::DebugOutputSynchronous:
			return GL_DEBUG_OUTPUT_SYNCHRONOUS;
		case n2dRenderDevice::Capability::DepthClamp:
			return GL_DEPTH_CLAMP;
		case n2dRenderDevice::Capability::DepthTest:
			return GL_DEPTH_TEST;
		case n2dRenderDevice::Capability::Dither:
			return GL_DITHER;
		case n2dRenderDevice::Capability::Framebuffer_SRGB:
			return GL_FRAMEBUFFER_SRGB;
		case n2dRenderDevice::Capability::LineSmooth:
			return GL_LINE_SMOOTH;
		case n2dRenderDevice::Capability::Multisample:
			return GL_MULTISAMPLE;
		case n2dRenderDevice::Capability::PolygonOffsetFill:
			return GL_POLYGON_OFFSET_FILL;
		case n2dRenderDevice::Capability::PolygonOffsetLine:
			return GL_POLYGON_OFFSET_LINE;
		case n2dRenderDevice::Capability::PolygonOffsetPoint:
			return GL_POLYGON_OFFSET_POINT;
		case n2dRenderDevice::Capability::PolygonSmooth:
			return GL_POLYGON_SMOOTH;
		case n2dRenderDevice::Capability::PrimitiveRestart:
			return GL_PRIMITIVE_RESTART;
		case n2dRenderDevice::Capability::PrimitiveRestartFixedIndex:
			return GL_PRIMITIVE_RESTART_FIXED_INDEX;
		case n2dRenderDevice::Capability::RasterizerDiscard:
			return GL_RASTERIZER_DISCARD;
		case n2dRenderDevice::Capability::SampleAlphaToCoverage:
			return GL_SAMPLE_ALPHA_TO_COVERAGE;
		case n2dRenderDevice::Capability::SampleAlphaToOne:
			return GL_SAMPLE_ALPHA_TO_ONE;
		case n2dRenderDevice::Capability::SampleCoverage:
			return GL_SAMPLE_COVERAGE;
		case n2dRenderDevice::Capability::SampleShading:
			return GL_SAMPLE_SHADING;
		case n2dRenderDevice::Capability::SampleMask:
			return GL_SAMPLE_MASK;
		case n2dRenderDevice::Capability::ScissorTest:
			return GL_SCISSOR_TEST;
		case n2dRenderDevice::Capability::StencilTest:
			return GL_STENCIL_TEST;
		case n2dRenderDevice::Capability::TextureCubeMapSeamless:
			return GL_TEXTURE_CUBE_MAP_SEAMLESS;
		case n2dRenderDevice::Capability::ProgramPointSize:
			return GL_PROGRAM_POINT_SIZE;
		default:
			throw natException(_T("n2dRenderDeviceImpl::GetCapabilityEnum"), _T("Not an available Capability enum"));
		}
	}

	/*static GLenum GetCapabilityIEnum(CapabilityI cp)
	{
		switch (cp)
		{
		case n2dRenderDevice::CapabilityI::ClipDistance:
			return GL_CLIP_DISTANCE0;
		default:
			throw natException(_T("n2dRenderDeviceImpl::GetCapabilityIEnum"), _T("Not an available CapabilityI enum"));
		}
	}*/

	static GLenum GetBlendFactorEnum(BlendFactor blendfactor)
	{
		switch (blendfactor)
		{
		case n2dRenderDevice::BlendFactor::Zero:
			return GL_ZERO;
		case n2dRenderDevice::BlendFactor::One:
			return GL_ONE;
		case n2dRenderDevice::BlendFactor::SrcColor:
			return GL_SRC_COLOR;
		case n2dRenderDevice::BlendFactor::OneMinusSrcColor:
			return GL_ONE_MINUS_SRC_COLOR;
		case n2dRenderDevice::BlendFactor::DstColor:
			return GL_DST_COLOR;
		case n2dRenderDevice::BlendFactor::OneMinusDstColor:
			return GL_ONE_MINUS_DST_COLOR;
		case n2dRenderDevice::BlendFactor::SrcAlpha:
			return GL_SRC_ALPHA;
		case n2dRenderDevice::BlendFactor::OneMinusSrcAlpha:
			return GL_ONE_MINUS_SRC_ALPHA;
		case n2dRenderDevice::BlendFactor::DstAlpha:
			return GL_DST_ALPHA;
		case n2dRenderDevice::BlendFactor::OneMinusDstAlpha:
			return GL_ONE_MINUS_DST_ALPHA;
		case n2dRenderDevice::BlendFactor::ConstantColor:
			return GL_CONSTANT_COLOR;
		case n2dRenderDevice::BlendFactor::OneMinusConstantColor:
			return GL_ONE_MINUS_CONSTANT_COLOR;
		case n2dRenderDevice::BlendFactor::ConstantAlpha:
			return GL_CONSTANT_ALPHA;
		case n2dRenderDevice::BlendFactor::OneMinusConstantAlpha:
			return GL_ONE_MINUS_CONSTANT_ALPHA;
		case n2dRenderDevice::BlendFactor::SrcAlphaSaturate:
			return GL_SRC_ALPHA_SATURATE;
		case n2dRenderDevice::BlendFactor::Src1Color:
			return GL_SRC1_COLOR;
		case n2dRenderDevice::BlendFactor::OneMinusSrc1Color:
			return GL_ONE_MINUS_SRC1_COLOR;
		case n2dRenderDevice::BlendFactor::Src1Alpha:
			return GL_SRC1_ALPHA;
		case n2dRenderDevice::BlendFactor::OneMinusSrc1Alpha:
			return GL_ONE_MINUS_SRC1_ALPHA;
		default:
			throw natException(_T("n2dRenderDeviceImpl::GetBlendFactorEnum"), _T("Not an available BlendFactor enum"));
		}
	}
public:
	///	@brief	渲染设备构造函数
	///	@param[in]	pEngine		要关联的引擎
	explicit n2dRenderDeviceImpl(n2dEngine* pEngine);
	~n2dRenderDeviceImpl() = default;

	///	@brief	关联渲染上下文到此线程
	void MakeCurrent() override;

	///	@brief	清除指定缓存
	///	@param	clearBit	指定清除的缓存（按位指定）
	void Clear(nuInt clearBit) override;

	///	@brief	启用特性
	void EnableCapability(Capability capability) override;
	///	@brief	禁用特性
	void DisableCapability(Capability capability) override;
	///	@brief	按索引启用特性
	//void EnableCapabilityI(CapabilityI capability, nuInt Index) override;
	///	@brief	按索引禁用特性
	//void DisableCapabilityI(CapabilityI capability, nuInt Index) override;

	///	@brief	判断特性是否已启用
	nBool IsCapabilityEnabled(Capability capability) const override;
	///	@brief	按索引判断特性是否已启用
	//nBool IsCapabilityIEnabled(CapabilityI capability, nuInt Index) const override;

	void SetBlendMode(BlendFactor Source, BlendFactor Destination) override;
	void SetBlendModeI(n2dBuffer* Buf, BlendFactor Source, BlendFactor Destination) override;

	void SetBlendColor(natVec4<> const& Color) override;
	void SetBlendColor(nFloat r, nFloat g, nFloat b, nFloat a) override;

	void SetSwapInterval(nuInt Interval) override;

	///	@brief	交换缓冲区
	void SwapBuffers() override;

	///	@brief	获得着色器包装器
	n2dShaderWrapper* GetShaderWrapper() override;

	///	@brief	提交模型矩阵
	///	@param[in]	Mat	提交的矩阵
	void SubmitModelMat(natMat4<> const& Mat) override;
	///	@brief	提交观察矩阵
	///	@param[in]	Mat	提交的矩阵
	void SubmitViewMat(natMat4<> const& Mat) override;
	///	@brief	提交投影矩阵
	///	@param[in]	Mat	提交的矩阵
	void SubmitProjMat(natMat4<> const& Mat) override;

	///	@brief	将当前模型矩阵压入栈
	///	@note	不会引发MVP矩阵更新
	void PushModelMat() override;
	///	@brief	将当前观察矩阵压入栈
	///	@note	不会引发MVP矩阵更新
	void PushViewMat() override;
	///	@brief	将当前投影矩阵压入栈
	///	@note	不会引发MVP矩阵更新
	void PushProjMat() override;

	///	@brief	从栈中弹出1个模型矩阵
	///	@return	操作是否成功
	nBool PopModelMat() override;
	///	@brief	从栈中弹出1个观察矩阵
	///	@return	操作是否成功
	nBool PopViewMat() override;
	///	@brief	从栈中弹出1个投影矩阵
	///	@return	操作是否成功
	nBool PopProjMat() override;

	///	@brief	将当前所有矩阵压入栈
	void PushMVPMat() override;
	///	@brief	从栈中弹出1个所有矩阵
	///	@return	操作是否成功
	nBool PopMVPMat() override;
	///	@brief	初始化MVP矩阵
	///	@note	仅覆盖当前栈顶矩阵
	void InitMVPMat() override;

	///	@brief	获得当前模型矩阵
	natMat4<> const& GetCurModelMat() const override;
	///	@brief	获得当前观察矩阵
	natMat4<> const& GetCurViewMat() const override;
	///	@brief	获得当前投影矩阵
	natMat4<> const& GetCurProjMat() const override;

	///	@brief	获得模型观察投影矩阵
	///	@return	当前的模型观察投影矩阵
	natMat4<> const& GetMVPMat() override;

	///	@brief	获得关联的引擎
	n2dEngine* GetEngine() override;

	nuInt GetMaxLight() const override;
	void SetMaxLights(nuInt value) override;
	n2dLightController* GetLightController(nuInt Index) override;

	nResult CreateBuffer(n2dBuffer::BufferTarget DefaultTarget, n2dBuffer** pOut) override;

	///	@brief	创建二维图元渲染器
	///	@param[out]		pOut	创建的二维图元渲染器
	///	@return	结果
	nResult CreateGraphics2D(n2dGraphics2D** pOut) override;

	nResult CreateGraphics3D(n2dGraphics3D** pOut) override;

	///	@brief	创建纹理
	///	@param[out]		pOut	创建的纹理
	///	@return	结果
	nResult CreateTexture(n2dTexture2D** pOut) override;

	///	@brief	从流中创建纹理
	///	@param[in]		pStream		流
	///	@param[in]		dwFileType	文件类型
	///	@param[out]		pOut		创建的纹理
	///	@return	结果
	nResult CreateTextureFromStream(natStream* pStream, DWORD dwFileType, n2dTexture2D** pOut) override;

	nResult CreateModelLoader(n2dModelLoader** pOut) override;

	///	@deprecated		仅实验用
	nResult CreateObjLoader(n2dModelLoader** pOut) override;

	nResult CreateMotionManager(n2dMotionManager** pOut) override;

	void updateMVP();
private:
	n2dEngine* m_pEngine;
	n2dShaderWrapperImpl* m_Shader;

	nBool m_bUpdated;

	std::stack<natMat4<>> m_ModelMatStack;
	std::stack<natMat4<>> m_ViewMatStack;
	std::stack<natMat4<>> m_ProjMatStack;

	natMat4<> m_MVPMat;

	n2dBufferImpl* m_pMVPBuffer;

	nuInt m_MaxLights;
	std::vector<natRefPointer<n2dLightControllerImpl>> m_Lights;
	n2dBufferImpl* m_pLightBuffer;
};