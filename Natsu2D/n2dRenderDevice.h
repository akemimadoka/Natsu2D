#pragma once
#include "n2dInterface.h"
#include "n2dCommon.h"
#include <natMat.h>

struct n2dShaderWrapper;
struct n2dEngine;
struct n2dGraphics2D;
struct n2dGraphics3D;
struct n2dTexture2D;
struct n2dModelLoader;

struct natStream;

////////////////////////////////////////////////////////////////////////////////
///	@brief	����
////////////////////////////////////////////////////////////////////////////////
struct n2dBuffer
	: n2dInterface
{
	///	@brief	����Ŀ��
	enum class BufferTarget
	{
		ArrayBuffer,
		AtomicCounterBuffer,
		CopyReadBuffer,
		CopyWriteBuffer,
		DispatchIndirectBuffer,
		DrawIndirectBuffer,
		ElementArrayBuffer,
		PixelPackBuffer,
		PixelUnpackBuffer,
		QueryBuffer,
		ShaderStorageBuffer,
		TextureBuffer,
		TransformFeedbackBuffer,
		UniformBuffer,
	};

	///	@brief	������;
	enum class BufferUsage
	{
		StaticDraw,
		StaticRead,
		StaticCopy,
		DynamicDraw,
		DynamicRead,
		DynamicCopy,
		StreamDraw,
		StreamRead ,
		StreamCopy,
	};

	///	@brief	�����ȡȨ��
	enum class BufferAccess
	{
		ReadOnly,
		WriteOnly,
		ReadWrite,
	};

	///	@brief	����BindRange�Ļ����ȡȨ��
	///	@see	BindRange
	enum BufferRangeAccess : nuInt
	{
		Read				= 0x0001,
		Write				= 0x0002,
		Persistent			= 0x0040,
		Coherent			= 0x0080,

		InvalidateRange		= 0x0004,
		InvalidateBuffer	= 0x0008,
		FlushExplicit		= 0x0010,
		Unsynchronized		= 0x0020,
	};

	///	@brief	�ڲ���ʽ
	enum class InternalFormat
	{
		R8,
		R16,
		R16F,
		R32F,
		R8I,
		R16I,
		R32I,
		R8UI,
		R16UI,
		R32UI,
		RG8,
		RG16,
		RG16F,
		RG32F,
		RG8I,
		RG16I,
		RG32I,
		RG8UI,
		RG16UI,
		RG32UI,
		RGB32F,
		RGB32I,
		RGB32UI,
		RGBA8,
		RGBA16,
		RGBA16F,
		RGBA32F,
		RGBA8I,
		RGBA16UI,
		RGBA32UI,
	};

	///	@brief	����ڲ����
	virtual GLhandle GetBuffer() const = 0;
	///	@brief	����Ŀ��
	///	@brief	������Ӧ����ʹ��
	virtual void SetTarget(BufferTarget Target) = 0;
	///	@brief	���Ŀ��
	virtual BufferTarget GetTarget() const = 0;
	///	@brief	�󶨻��浽Ŀ��
	virtual void Bind() const = 0;
	///	@brief	�󶨻��浽Ŀ�����
	virtual void BindBase() = 0;
	///	@brief	�󶨻��浽Ŀ�����
	///	@param[in]	BindingPoint	ָ���󶨵�
	virtual void BindBase(nuInt BindingPoint) = 0;
	///	@brief	�󶨻��浽Ŀ�����
	///	@param[in]	Offset	ƫ��
	///	@param[in]	size	��С
	virtual void BindRange(nInt Offset, nuInt size) = 0;
	///	@brief	�󶨻��浽Ŀ�����
	///	@param[in]	BindingPoint	ָ���󶨵�
	///	@param[in]	Offset			ƫ��
	///	@param[in]	size			��С
	virtual void BindRange(nuInt BindingPoint, nInt Offset, nuInt size) = 0;
	///	@brief	��������
	///	@param[in]	Size		��С
	///	@param[in]	pInitData	��ʼ�����ݣ���������ָ��
	///	@param[in]	Usage		��;
	///	@note	��������Bind
	virtual void AllocData(nuInt Size, ncData pInitData, BufferUsage Usage) = 0;
	///	@brief	����������
	///	@param[in]	Offset	ƫ��
	///	@param[in]	Size	��С
	///	@param[in]	pData	���ݣ���������ָ��
	///	@note	��������Bind
	virtual void AllocSubData(nuInt Offset, nuInt Size, ncData pData) = 0;

	// �����ķ���
	//virtual void ClearData(InternalFormat internalFormat, GLenum Format, GLenum Type, ncData pData) = 0;
	//virtual void ClearSubData(InternalFormat internalFormat, nuInt Offset, nuInt Size, GLenum Format, GLenum Type, ncData pData) = 0;

	///	@brief	���������ݵ�
	///	@param[in]	Target		Ŀ�껺��
	///	@param[in]	ReadOffset	��ȡƫ��
	///	@param[in]	WriteOffset	д��ƫ��
	///	@param[in]	Size		��С
	///	@note	��������Bind
	virtual void CopySubDataTo(BufferTarget Target, nuInt ReadOffset, nuInt WriteOffset, nuInt Size) = 0;

	///	@brief	��û����С
	virtual nuInt GetBufferSize() const = 0;
	///	@brief	��ð󶨵�
	virtual nuInt GetBindingPoint() const = 0;

	///	@brief	���������
	///	@param[in]	Offset	ƫ��
	///	@param[in]	Size	��С
	///	@param[out]	pData	����
	///	@note	��������Bind
	virtual void GetSubData(nuInt Offset, nuInt Size, nData pData) = 0;

	///	@brief	ӳ�仺��
	///	@param[in]	Access	��ȡȨ��
	///	@return	ӳ�����
	///	@note	��ʹ�ϴ�ӳ�����ʧЧ\n
	///			��������Bind
	virtual natRefPointer<natStream> MapBuffer(BufferAccess Access) = 0;
	///	@brief	�������ӳ��
	///	@note	��ʹ�ϴ�ӳ�����ʧЧ\n
	///			��������Bind
	virtual void UnmapBuffer() = 0;

	///	@brief	ӳ�䲿�ֻ���
	///	@param[in]	Offset	ƫ��
	///	@param[in]	Length	����
	///	@param[in]	Access	��ȡȨ��
	///	@return	ӳ�����
	///	@note	��ʹ�ϴ�ӳ�����ʧЧ\n
	///			��������Bind
	virtual natRefPointer<natStream> MapBufferRange(nuInt Offset, nuInt Length, nuInt Access) = 0;
	///	@brief	ˢ��ӳ��Ĳ��ֻ���
	///	@param[in]	Offset	ƫ��
	///	@param[in]	Length	����
	///	@note	��ʹ�ϴ�ӳ�����ʧЧ\n
	///			��������Bind
	virtual void FlushMappedBufferRange(nuInt Offset, nuInt Length) = 0;

	///	@brief	������������
	///	@note	��������Bind
	virtual void InvalidateData() = 0;
	///	@brief	��������������
	///	@param[in]	Offset	ƫ��
	///	@param[in]	Length	����
	///	@note	��������Bind
	virtual void InvalidateSubData(nuInt Offset, nuInt Length) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	��ɫ��
////////////////////////////////////////////////////////////////////////////////
struct n2dShader
	: n2dInterface
{
	///	@brief	��ɫ������
	enum class ShaderType
	{
		Vertex,			///< @brief	������ɫ��
		Fragment,		///< @brief	Ƭ����ɫ��
		Geometry,		///< @brief	������ɫ��
	};

	///	@brief	����ڲ����
	///	@note	�����ֶ�ɾ��
	virtual GLhandle GetHandle() const = 0;
	///	@brief	�����ɫ������
	virtual ShaderType GetType() const = 0;
	///	@brief	�Ƿ����趨ɾ�����
	virtual nBool Deleted() const = 0;
	///	@brief	�Ƿ��Ѿ�����
	virtual nBool Compiled() const = 0;
	///	@brief	��ñ�����־
	virtual ncTStr GetInfoLog() const = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	��ɫ������
////////////////////////////////////////////////////////////////////////////////
struct n2dShaderProgram
	: n2dInterface
{
	///	@brief	��ɫ����������
	enum class VarType
	{
		Float,
		vec2,
		vec3,
		vec4,
		Double,
		dvec2,
		dvec3,
		dvec4,
		Int,
		ivec2,
		ivec3,
		ivec4,
		Uint,
		uvec2,
		uvec3,
		uvec4,
		Bool,
		bvec2,
		bvec3,
		bvec4,
		mat2,
		mat3,
		mat4,
		mat2x3,
		mat2x4,
		mat3x2,
		mat3x4,
		mat4x2,
		mat4x3,
		dmat2,
		dmat3,
		dmat4,
		dmat2x3,
		dmat2x4,
		dmat3x2,
		dmat3x4,
		dmat4x2,
		dmat4x3,
		sampler1D,
		sampler2D,
		sampler3D,
		samplerCube,
		sampler1DShadow,
		sampler2DShadow,
		sampler1DArray,
		sampler2DArray,
		sampler1DArrayShadow,
		sampler2DArrayShadow,
		sampler2DMS,
		sampler2DMSArray,
		samplerCubeShadow,
		samplerBuffer,
		sampler2DRect,
		sampler2DRectShadow,
		isampler1D,
		isampler2D,
		isampler3D,
		isamplerCube,
		isampler1DArray,
		isampler2DArray,
		isampler2DMS,
		isampler2DMSArray,
		isamplerBuffer,
		isampler2DRect,
		usampler1D,
		usampler2D,
		usampler3D,
		usamplerCube,
		usampler2DArray,
		usampler2DMS,
		usampler2DMSArray,
		usamplerBuffer,
		usampler2DRect,
		image1D,
		image2D,
		image3D,
		image2DRect,
		imageCube,
		imageBuffer,
		image1DArray,
		image2DArray,
		image2DMS,
		image2DMSArray,
		iimage1D,
		iimage2D,
		iimage3D,
		iimage2DRect,
		iimageCube,
		iimageBuffer,
		iimage1DArray,
		iimage2DArray,
		iimage2DMS,
		iimage2DMSArray,
		uimage1D,
		uimage2D,
		uimage3D,
		uimage2DRect,
		uimageCube,
		uimageBuffer,
		uimage1DArray,
		uimage2DArray,
		uimage2DMS,
		uimage2DMSArray,
		atomic_uint,
	};

	///	@brief	Attribute��Size
	///	@note	����Ϊ1, 2, 3, 4, AttribSize_BGRA
	enum AttribSize : nuInt
	{
		AttribSize_BGRA,
	};

	///	@brief	Attribute���ڲ�����
	enum class AttribType
	{
		Byte,
		UnsignedByte,
		Short,
		UnsignedShort,
		Int,
		UnsignedInt,

		HalfFloat,
		Float,
		Fixed,
		Int_2_10_10_10_Rev,
		UnsignedInt_2_10_10_10_Rev,
		UnsignedInt_10F_11F_11F_Rev,

		Double,
	};

	///	@brief	Attribute����
	///	@note	��������ʱʧЧ
	struct AttributeReference
		: n2dInterface
	{
		///	@brief	��ù����ĳ���
		///	@note	�������ٺ󷵻�nullptr
		virtual n2dShaderProgram* GetProgram() const = 0;

		///	@brief	��ô�С
		///	@note	GetSize��õĴ�С��Ԫ�ظ�����GetInternalSize��õĴ�С��ʵ��ռ���ֽ���
		virtual nuInt GetSize() const = 0;
		///	@brief	����ڲ���С
		///	@see	GetSize
		virtual nuInt GetInternalSize() const = 0;
		///	@brief	��ñ�������
		virtual VarType GetType() const = 0;

		///	@brief	���ö�������
		///	@param[in]	Value	�Ƿ�����
		virtual void EnablePointer(nBool Value = true) const = 0;
		///	@brief	���������Ƿ�����
		virtual nBool isEnable() const = 0;

		///	@brief	��ö������԰󶨵�����ֵָ��
		virtual const void* GetPointer() const = 0;
		///	@brief	���ö������԰󶨵�����ֵָ��
		///	@param[in]	Size		Ԫ�ظ���
		///	@param[in]	Type		Attribute�ڲ�����
		///	@param[in]	Normalized	��һ��
		///	@param[in]	Stride		Ԫ�ؼ��
		///	@param[in]	pPointer	����ֵָ��
		///	@return	������
		virtual nResult SetPointer(nuInt Size, AttribType Type, nBool Normalized, nuInt Stride, const void* pPointer) = 0;

		///	@brief	���þ�ֵ̬
		///	@return	������
		virtual nResult SetStaticValue(nBool Normalized, nBool Explicit, const void* pValue) = 0;
	};

	///	@brief	Uniform����
	///	@note	��������ʱʧЧ
	struct UniformReference
		: n2dInterface
	{
		///	@brief	��ù����ĳ���
		///	@note	�������ٺ󷵻�nullptr
		virtual n2dShaderProgram* GetProgram() const = 0;

		///	@brief	��ñ�������
		virtual VarType GetType() const = 0;
		///	@brief	��ô�С
		virtual nuInt GetSize() const = 0;
		///	@brief	���ֵ
		///	@param[in]	Size	Valueָ����ڴ��С
		///	@param[out]	Value	������Ļ�����
		virtual void GetValue(nuInt Size, void* Value) const = 0;
		///	@brief	����ֵ
		///	@param[in]	Count		Ԫ�ظ���
		///	@param[in]	Value		ָ��Ҫ���õ�ֵ��ָ��
		///	@param[in]	Transpose	����������Ϊ����ʱ���Ƿ��Զ�ת�þ���
		///	@note	���������Ͳ��Ǿ���ʱ��Transpose������
		virtual void SetValue(nuInt Count, const void* Value, nBool Transpose = false) = 0;
	};

	///	@brief	Uniform������
	///	@note	��������ʱʧЧ
	struct UniformBlockReference
		: n2dInterface
	{
		///	@brief	��ù����ĳ���
		///	@note	�������ٺ󷵻�nullptr
		virtual n2dShaderProgram* GetProgram() const = 0;

		///	@brief	��ô�С
		virtual nuInt GetSize() const = 0;
		///	@brief	�����Ч��Uniform����
		virtual nuInt ActiveUniformCount() const = 0;

		///	@brief	�󶨵�Buffer
		virtual void Bind(n2dBuffer* pBuffer) = 0;
		///	@brief	��ð󶨵�
		virtual nuInt GetBindingPoint() const = 0;
	};

	///	@brief	����ڲ����
	///	@note	�����ֶ�ɾ��
	virtual GLhandle GetHandle() const = 0;

	///	@brief	������ɫ��
	virtual void AttachShader(n2dShader* pShader) = 0;
	///	@brief	������ɫ��
	virtual void DetachShader(n2dShader* pShader) = 0;

	///	@brief	��ø��ӵ���ɫ���ĸ���
	virtual nuInt AttachedShaderCount() const = 0;

	///	@brief	ʹ������Ա���ö������ļ�
	///	@note	��������ǰ������Ч
	virtual void SetBinaryRetrievable(nBool Value = true) = 0;
	///	@brief	ʹ������Է���
	///	@note	��������ǰ������Ч
	virtual void SetSeparable(nBool Value = true) = 0;

	///	@brief	������ɫ��
	virtual void Link() = 0;
	///	@brief	�Ƿ�������
	virtual nBool IsLinked() const = 0;
	///	@brief	��֤��ɫ��
	///	@return	��֤���
	virtual nBool Validate() const = 0;
	///	@brief	�����Ϣ��־
	virtual ncTStr GetInfoLog() const = 0;
	///	@brief	ʹ����ɫ��
	virtual void Use() const = 0;
	///	@brief	����ɫ���Ƿ�����ʹ��
	virtual nBool IsUsing() const = 0;

	///	@brief	����������ļ�
	///	@param[inout]	pStream	���������
	///	@return	������
	virtual nResult OutputBinary(natStream* pStream) const = 0;

	///	@brief	�����Ч��Attribute����
	virtual nuInt ActiveAttributeCount() const = 0;
	///	@brief	�����Ч��Uniform����
	virtual nuInt ActiveUniformCount() const = 0;
	///	@brief	�����Ч��Uniform�����
	virtual nuInt ActiveUniformBlockCount() const = 0;

	///	@brief	���Attribute����
	///	@param[in]	Location	λ��
	virtual AttributeReference* GetAttributeReference(nuInt Location) = 0;
	///	@brief	���Attribute����
	///	@param[in]	Name	����
	virtual AttributeReference* GetAttributeReference(ncTStr Name) = 0;

	///	@brief	���Uniform����
	///	@param[in]	Location	λ��
	virtual UniformReference* GetUniformReference(nuInt Location) = 0;
	///	@brief	���Uniform����
	///	@param[in]	Name	����
	virtual UniformReference* GetUniformReference(ncTStr Name) = 0;

	///	@brief	���Uniform������
	///	@param[in]	Location	λ��
	virtual UniformBlockReference* GetUniformBlockReference(nuInt Location) = 0;
	///	@brief	���Uniform������
	///	@param[in]	Name	����
	virtual UniformBlockReference* GetUniformBlockReference(ncTStr Name) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	�������
////////////////////////////////////////////////////////////////////////////////
struct n2dProgramPipeline
	: n2dInterface
{
	///	@brief	�󶨵��ĵص�
	enum Stages : nuInt
	{
		Vertex			= 0x00000001,	///< @brief	���㴦��
		Fragment		= 0x00000002,	///< @brief	ƬԪ����
		Geometry		= 0x00000004,	///< @brief	���δ���
		TessControl		= 0x00000008,	///< @brief	ϸ�ֿ���
		TessEvaluation	= 0x00000010,	///< @brief	ϸ������
		Compute			= 0x00000020,	///< @brief	����
		All				= 0xFFFFFFFF,	///< @brief	ȫ��
	};

	///	@brief	����ڲ����
	virtual GLhandle GetHandle() const = 0;
	///	@brief	�󶨵�ǰ�������
	virtual void Bind() const = 0;
	///	@brief	�жϵ�ǰ�Ƿ��
	virtual nBool IsBinding() const = 0;

	///	@brief	�󶨳��򵽹��߲���
	///	@param[in]	stages		����
	///	@param[in]	pProgram	��ɫ������
	virtual void UseProgramStages(nuInt stages, n2dShaderProgram* pProgram) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	��ɫ����װ��
////////////////////////////////////////////////////////////////////////////////
struct n2dShaderWrapper
	: n2dInterface
{
	///	@brief	�����д�����ɫ��
	virtual nResult CreateShaderFromStream(natStream* pStream, n2dShader::ShaderType shaderType, nBool bIsBinary, n2dShader** pOut) = 0;
	///	@brief	������ɫ������
	virtual nResult CreateProgram(n2dShaderProgram** pOut) = 0;
	///	@brief	�����д�����ɫ������
	///	@note	���صı�������ɫ������Ķ����Ʊ���
	virtual nResult CreateProgramFromStream(natStream* pStream, n2dShaderProgram** pOut) = 0;

	///	@brief		��õ�ǰʹ�õ���ɫ������
	///	@warning	�ͷ�ָ��ᵼ����ɫ������ɾ��
	virtual n2dShaderProgram* GetCurrentProgram() = 0;
	///	@brief		��õ�ǰʹ�õĳ������
	///	@warning	�ͷ�ָ��ᵼ�¹��߶���ɾ��
	virtual n2dProgramPipeline* GetCurrentProgramPipeline() = 0;

	///	@brief		���Ĭ�ϳ���
	///	@warning	���������ͷ�
	virtual n2dShaderProgram* GetDefaultProgram() const = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	��Ⱦ�豸
////////////////////////////////////////////////////////////////////////////////
struct n2dRenderDevice
	: n2dInterface
{
	///	@brief	�������
	enum ClearBit : nuInt
	{
		ClearBit_Color		= 0x00004000,	///< @brief	��ɫ����
		ClearBit_Depth		= 0x00000100,	///< @brief	��Ȼ���
		ClearBit_Stencil	= 0x00000400,	///< @brief	ģ�建��
	};

	///	@brief	����
	enum class Capability
	{
		Blend,						///< @brief	���
		ColorLogicOp,				///< @brief	��ɫ�߼�����
		CullFace,					///< @brief	�޳���
		DebugOutput,				///< @brief	�������
		DebugOutputSynchronous,		///< @brief	ͬ���������
		DepthClamp,					///< @brief	����޶�
		DepthTest,					///< @brief	��Ȳ���
		Dither,						///< @brief	����
		Framebuffer_SRGB,			///< @brief	ǿ��֡����ʹ��SRGB
		LineSmooth,					///< @brief	�߶η�����
		Multisample,				///< @brief	���ز���
		PolygonOffsetFill,			///< @brief	Fillģʽ�����ƫ��
		PolygonOffsetLine,			///< @brief	Lineģʽ�����ƫ��
		PolygonOffsetPoint,			///< @brief	Pointģʽ�����ƫ��
		PolygonSmooth,				///< @brief	����η�����
		PrimitiveRestart,			///< @brief	ͼԪ������
		PrimitiveRestartFixedIndex,	///< @brief	ʹ�ù̶�������ͼԪ������
		RasterizerDiscard,			///< @brief	��դ������
		SampleAlphaToCoverage,		///< @brief	�ɲ���Alphaֵ������ʱ����ֵ
		SampleAlphaToOne,			///< @brief	����Alphaֵʼ�����
		SampleCoverage,				///< @brief	ƬԪ����ֵ����ʱ����ֵ���������
		SampleShading,				///< @brief	��ÿ�����ǲ���ִ��һ��Ƭ����ɫ��
		SampleMask,					///< @brief	��������
		ScissorTest,				///< @brief	���ò���
		StencilTest,				///< @brief	ģ�����
		TextureCubeMapSeamless,		///< @brief	�޷�����ͼ����
		ProgramPointSize,			///< @brief	��ɫ��������PointSize
	};

	///	@brief	ʹ������������
	enum class CapabilityI
	{
		ClipDistance,				///< @brief	�о�
	};

	///	@brief	�������
	enum class BlendFactor
	{
		Zero,
		One,
		SrcColor,
		OneMinusSrcColor,
		DstColor,
		OneMinusDstColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha,
		ConstantColor,
		OneMinusConstantColor,
		ConstantAlpha,
		OneMinusConstantAlpha,
		SrcAlphaSaturate,
		Src1Color,
		OneMinusSrc1Color,
		Src1Alpha,
		OneMinusSrc1Alpha,
	};

	///	@brief	������Ⱦ�����ĵ����߳�
	virtual void MakeCurrent() = 0;

	///	@brief	���ָ������
	///	@param	clearBit	ָ������Ļ��棨��λָ����
	virtual void Clear(nuInt clearBit) = 0;

	///	@brief	��������
	virtual void EnableCapability(Capability capability) = 0;
	///	@brief	��������
	virtual void DisableCapability(Capability capability) = 0;
	///	@brief	��������������
	virtual void EnableCapabilityI(CapabilityI capability, nuInt Index) = 0;
	///	@brief	��������������
	virtual void DisableCapabilityI(CapabilityI capability, nuInt Index) = 0;
	
	///	@brief	�ж������Ƿ�������
	virtual nBool IsCapabilityEnabled(Capability capability) const = 0;
	///	@brief	�������ж������Ƿ�������
	virtual nBool IsCapabilityIEnabled(CapabilityI capability, nuInt Index) const = 0;

	///	@brief	���û��ģʽ
	///	@param[in]	Source		Դ�������
	///	@param[in]	Destination	Ŀ��������
	virtual void SetBlendMode(BlendFactor Source, BlendFactor Destination) = 0;
	///	@brief	Ϊ�ض���DrawBuffer���û��ģʽ
	///	@param[in]	Source		Դ�������
	///	@param[in]	Destination	Ŀ��������
	virtual void SetBlendModeI(n2dBuffer* Buf, BlendFactor Source, BlendFactor Destination) = 0;
	///	@brief	���û����ɫ
	virtual void SetBlendColor(natVec4<> const& Color) = 0;
	///	@brief	���û����ɫ
	virtual void SetBlendColor(nFloat r, nFloat g, nFloat b, nFloat a) = 0;

	///	@brief	����ͬ��Ƶ��
	///	@note	����Ϊ0��Ϊ�رմ�ֱͬ��
	virtual void SetSwapInterval(nuInt Interval) = 0;

	///	@brief	����������
	virtual void SwapBuffers() = 0;

	///	@brief	�����ɫ����װ��
	virtual n2dShaderWrapper* GetShaderWrapper() = 0;

	///	@brief	�ύģ�;���
	///	@param[in]	Mat	�ύ�ľ���
	virtual void SubmitModelMat(natMat4<> const& Mat) = 0;
	///	@brief	�ύ�۲����
	///	@param[in]	Mat	�ύ�ľ���
	virtual void SubmitViewMat(natMat4<> const& Mat) = 0;
	///	@brief	�ύͶӰ����
	///	@param[in]	Mat	�ύ�ľ���
	virtual void SubmitProjMat(natMat4<> const& Mat) = 0;

	///	@brief	����ǰģ�;���ѹ��ջ
	///	@note	��������MVP�������
	virtual void PushModelMat() = 0;
	///	@brief	����ǰ�۲����ѹ��ջ
	///	@note	��������MVP�������
	virtual void PushViewMat() = 0;
	///	@brief	����ǰͶӰ����ѹ��ջ
	///	@note	��������MVP�������
	virtual void PushProjMat() = 0;

	///	@brief	��ջ�е���1��ģ�;���
	///	@return	�����Ƿ�ɹ�
	virtual nBool PopModelMat() = 0;
	///	@brief	��ջ�е���1���۲����
	///	@return	�����Ƿ�ɹ�
	virtual nBool PopViewMat() = 0;
	///	@brief	��ջ�е���1��ͶӰ����
	///	@return	�����Ƿ�ɹ�
	virtual nBool PopProjMat() = 0;

	///	@brief	����ǰ���о���ѹ��ջ
	virtual void PushMVPMat() = 0;
	///	@brief	��ջ�е���1�����о���
	///	@return	�����Ƿ�ɹ�
	virtual nBool PopMVPMat() = 0;
	///	@brief	��ʼ��MVP����
	///	@note	�����ǵ�ǰջ������
	virtual void InitMVPMat() = 0;

	///	@brief	��õ�ǰģ�;���
	virtual natMat4<> const& GetCurModelMat() const = 0;
	///	@brief	��õ�ǰ�۲����
	virtual natMat4<> const& GetCurViewMat() const = 0;
	///	@brief	��õ�ǰͶӰ����
	virtual natMat4<> const& GetCurProjMat() const = 0;

	///	@brief	���ģ�͹۲�ͶӰ����
	///	@return	��ǰ��ģ�͹۲�ͶӰ����
	virtual natMat4<> const& GetMVPMat() = 0;

	///	@brief	��ù���������
	virtual n2dEngine* GetEngine() = 0;

	///	@brief	��������
	///	@param[in]	DefaultTarget	��ʼĿ��
	///	@param[out]	pOut			�����Ļ���
	///	@return	������
	virtual nResult CreateBuffer(n2dBuffer::BufferTarget DefaultTarget, n2dBuffer** pOut) = 0;

	///	@brief	������άͼԪ��Ⱦ��
	///	@param[out]	pOut	�����Ķ�άͼԪ��Ⱦ��
	///	@return	������
	virtual nResult CreateGraphics2D(n2dGraphics2D** pOut) = 0;
	///	@brief	������άͼԪ��Ⱦ��
	///	@param[out]	pOut	��������άͼԪ��Ⱦ��
	///	@return	������
	virtual nResult CreateGraphics3D(n2dGraphics3D** pOut) = 0;

	///	@brief	��������
	///	@param[out]	pOut	����������
	///	@return	������
	virtual nResult CreateTexture(n2dTexture2D** pOut) = 0;
	///	@brief	�����д�������
	///	@param[out]	pOut	����������
	///	@return	������
	virtual nResult CreateTextureFromStream(natStream* pStream, DWORD dwFileType, n2dTexture2D** pOut) = 0;

	///	@brief	����ģ�ͼ�����
	///	@param[out]	pOut	������ģ�ͼ�����
	///	@return	������
	virtual nResult CreateModelLoader(n2dModelLoader** pOut) = 0;

	///	@brief	����Objģ�ͼ�����
	///	@param[out]	pOut	������Objģ�ͼ�����
	///	@return	������
	///	@deprecated	�����߲�����
	virtual nResult CreateObjLoader(n2dModelLoader** pOut) = 0;
};