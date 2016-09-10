////////////////////////////////////////////////////////////////////////////////
///	@file	n2dRenderDeviceImpl.h
///	@brief	渲染设备相关
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../n2dRenderDevice.h"
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
		case Capability::Blend:
			return GL_BLEND;
		case Capability::ClipDistance:
			return GL_CLIP_DISTANCE0;
		case Capability::ColorLogicOp:
			return GL_COLOR_LOGIC_OP;
		case Capability::CullFace:
			return GL_CULL_FACE;
		case Capability::DebugOutput:
			return GL_DEBUG_OUTPUT;
		case Capability::DebugOutputSynchronous:
			return GL_DEBUG_OUTPUT_SYNCHRONOUS;
		case Capability::DepthClamp:
			return GL_DEPTH_CLAMP;
		case Capability::DepthTest:
			return GL_DEPTH_TEST;
		case Capability::Dither:
			return GL_DITHER;
		case Capability::Framebuffer_SRGB:
			return GL_FRAMEBUFFER_SRGB;
		case Capability::LineSmooth:
			return GL_LINE_SMOOTH;
		case Capability::Multisample:
			return GL_MULTISAMPLE;
		case Capability::PolygonOffsetFill:
			return GL_POLYGON_OFFSET_FILL;
		case Capability::PolygonOffsetLine:
			return GL_POLYGON_OFFSET_LINE;
		case Capability::PolygonOffsetPoint:
			return GL_POLYGON_OFFSET_POINT;
		case Capability::PolygonSmooth:
			return GL_POLYGON_SMOOTH;
		case Capability::PrimitiveRestart:
			return GL_PRIMITIVE_RESTART;
		case Capability::PrimitiveRestartFixedIndex:
			return GL_PRIMITIVE_RESTART_FIXED_INDEX;
		case Capability::RasterizerDiscard:
			return GL_RASTERIZER_DISCARD;
		case Capability::SampleAlphaToCoverage:
			return GL_SAMPLE_ALPHA_TO_COVERAGE;
		case Capability::SampleAlphaToOne:
			return GL_SAMPLE_ALPHA_TO_ONE;
		case Capability::SampleCoverage:
			return GL_SAMPLE_COVERAGE;
		case Capability::SampleShading:
			return GL_SAMPLE_SHADING;
		case Capability::SampleMask:
			return GL_SAMPLE_MASK;
		case Capability::ScissorTest:
			return GL_SCISSOR_TEST;
		case Capability::StencilTest:
			return GL_STENCIL_TEST;
		case Capability::TextureCubeMapSeamless:
			return GL_TEXTURE_CUBE_MAP_SEAMLESS;
		case Capability::ProgramPointSize:
			return GL_PROGRAM_POINT_SIZE;
		default:
			nat_Throw(natException, _T("Not an available Capability enum"));
		}
	}

	static GLenum GetCapabilityIEnum(CapabilityI cp)
	{
		switch (cp)
		{
		case CapabilityI::Blend:
			return GL_BLEND;
		case CapabilityI::ScissorTest:
			return GL_SCISSOR_TEST;
		default:
			nat_Throw(natException, _T("Not an available CapabilityI enum"));
		}
	}

	static GLenum GetBlendFactorEnum(BlendFactor blendfactor)
	{
		switch (blendfactor)
		{
		case BlendFactor::Zero:
			return GL_ZERO;
		case BlendFactor::One:
			return GL_ONE;
		case BlendFactor::SrcColor:
			return GL_SRC_COLOR;
		case BlendFactor::OneMinusSrcColor:
			return GL_ONE_MINUS_SRC_COLOR;
		case BlendFactor::DstColor:
			return GL_DST_COLOR;
		case BlendFactor::OneMinusDstColor:
			return GL_ONE_MINUS_DST_COLOR;
		case BlendFactor::SrcAlpha:
			return GL_SRC_ALPHA;
		case BlendFactor::OneMinusSrcAlpha:
			return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFactor::DstAlpha:
			return GL_DST_ALPHA;
		case BlendFactor::OneMinusDstAlpha:
			return GL_ONE_MINUS_DST_ALPHA;
		case BlendFactor::ConstantColor:
			return GL_CONSTANT_COLOR;
		case BlendFactor::OneMinusConstantColor:
			return GL_ONE_MINUS_CONSTANT_COLOR;
		case BlendFactor::ConstantAlpha:
			return GL_CONSTANT_ALPHA;
		case BlendFactor::OneMinusConstantAlpha:
			return GL_ONE_MINUS_CONSTANT_ALPHA;
		case BlendFactor::SrcAlphaSaturate:
			return GL_SRC_ALPHA_SATURATE;
		case BlendFactor::Src1Color:
			return GL_SRC1_COLOR;
		case BlendFactor::OneMinusSrc1Color:
			return GL_ONE_MINUS_SRC1_COLOR;
		case BlendFactor::Src1Alpha:
			return GL_SRC1_ALPHA;
		case BlendFactor::OneMinusSrc1Alpha:
			return GL_ONE_MINUS_SRC1_ALPHA;
		default:
			nat_Throw(natException, _T("Not an available BlendFactor enum"));
		}
	}
public:
	///	@brief	渲染设备构造函数
	///	@param[in]	pEngine		要关联的引擎
	explicit n2dRenderDeviceImpl(n2dEngine* pEngine);
	~n2dRenderDeviceImpl();

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
	void EnableCapabilityI(CapabilityI capability, nuInt Index) override;
	///	@brief	按索引禁用特性
	void DisableCapabilityI(CapabilityI capability, nuInt Index) override;

	///	@brief	判断特性是否已启用
	nBool IsCapabilityEnabled(Capability capability) const override;
	///	@brief	按索引判断特性是否已启用
	nBool IsCapabilityIEnabled(CapabilityI capability, nuInt Index) const override;

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
	nResult CreateLayerMgr(n2dLayerMgr** pOut) override;
	nResult CreateGraphics2D(n2dGraphics2D** pOut) override;
	nResult CreateGraphics3D(n2dGraphics3D** pOut) override;
	nResult CreateTexture(n2dTexture2D** pOut) override;
	nResult CreateTextureFromStream(natStream* pStream, DWORD dwFileType, n2dTexture2D** pOut) override;
	nResult CreateModelLoader(n2dModelLoader** pOut) override;
	///	@deprecated		仅实验用
	nResult CreateObjLoader(n2dModelLoader** pOut) override;
	nResult CreateMotionManager(n2dMotionManager** pOut) override;
	nResult CreateFontManager(n2dFont** pOut) override;

	void updateMVP();
private:
	n2dEngine* m_pEngine;
	n2dShaderWrapperImpl* m_Shader;

	nBool m_bUpdated;

	std::stack<natMat4<>> m_ModelMatStack;
	std::stack<natMat4<>> m_ViewMatStack;
	std::stack<natMat4<>> m_ProjMatStack;

	natMat4<> m_MVPMat;

	natRefPointer<n2dBufferImpl> m_pMVPBuffer;

	nuInt m_MaxLights;
	std::vector<natRefPointer<n2dLightControllerImpl>> m_Lights;
	natRefPointer<n2dBufferImpl> m_pLightBuffer;
};