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
///	@brief	缓存
////////////////////////////////////////////////////////////////////////////////
struct n2dBuffer
	: n2dInterface
{
	///	@brief	缓存目标
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

	///	@brief	缓存用途
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

	///	@brief	缓存存取权限
	enum class BufferAccess
	{
		ReadOnly,
		WriteOnly,
		ReadWrite,
	};

	///	@brief	用于BindRange的缓存存取权限
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

	///	@brief	内部格式
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

	///	@brief	获得内部句柄
	virtual GLhandle GetBuffer() const = 0;
	///	@brief	设置目标
	///	@brief	创建后应避免使用
	virtual void SetTarget(BufferTarget Target) = 0;
	///	@brief	获得目标
	virtual BufferTarget GetTarget() const = 0;
	///	@brief	绑定缓存到目标
	virtual void Bind() const = 0;
	///	@brief	绑定缓存到目标对象
	virtual void BindBase() = 0;
	///	@brief	绑定缓存到目标对象
	///	@param[in]	BindingPoint	指定绑定点
	virtual void BindBase(nuInt BindingPoint) = 0;
	///	@brief	绑定缓存到目标对象
	///	@param[in]	Offset	偏移
	///	@param[in]	size	大小
	virtual void BindRange(nInt Offset, nuInt size) = 0;
	///	@brief	绑定缓存到目标对象
	///	@param[in]	BindingPoint	指定绑定点
	///	@param[in]	Offset			偏移
	///	@param[in]	size			大小
	virtual void BindRange(nuInt BindingPoint, nInt Offset, nuInt size) = 0;
	///	@brief	分配数据
	///	@param[in]	Size		大小
	///	@param[in]	pInitData	初始化数据，可以留空指针
	///	@param[in]	Usage		用途
	///	@note	隐含调用Bind
	virtual void AllocData(nuInt Size, ncData pInitData, BufferUsage Usage) = 0;
	///	@brief	分配子数据
	///	@param[in]	Offset	偏移
	///	@param[in]	Size	大小
	///	@param[in]	pData	数据，可以留空指针
	///	@note	隐含调用Bind
	virtual void AllocSubData(nuInt Offset, nuInt Size, ncData pData) = 0;

	// 废弃的方法
	//virtual void ClearData(InternalFormat internalFormat, GLenum Format, GLenum Type, ncData pData) = 0;
	//virtual void ClearSubData(InternalFormat internalFormat, nuInt Offset, nuInt Size, GLenum Format, GLenum Type, ncData pData) = 0;

	///	@brief	复制子数据到
	///	@param[in]	Target		目标缓存
	///	@param[in]	ReadOffset	读取偏移
	///	@param[in]	WriteOffset	写入偏移
	///	@param[in]	Size		大小
	///	@note	隐含调用Bind
	virtual void CopySubDataTo(BufferTarget Target, nuInt ReadOffset, nuInt WriteOffset, nuInt Size) = 0;

	///	@brief	获得缓存大小
	virtual nuInt GetBufferSize() const = 0;
	///	@brief	获得绑定点
	virtual nuInt GetBindingPoint() const = 0;

	///	@brief	获得子数据
	///	@param[in]	Offset	偏移
	///	@param[in]	Size	大小
	///	@param[out]	pData	数据
	///	@note	隐含调用Bind
	virtual void GetSubData(nuInt Offset, nuInt Size, nData pData) = 0;

	///	@brief	映射缓存
	///	@param[in]	Access	存取权限
	///	@return	映射的流
	///	@note	会使上次映射的流失效\n
	///			隐含调用Bind
	virtual natRefPointer<natStream> MapBuffer(BufferAccess Access) = 0;
	///	@brief	解除缓存映射
	///	@note	会使上次映射的流失效\n
	///			隐含调用Bind
	virtual void UnmapBuffer() = 0;

	///	@brief	映射部分缓存
	///	@param[in]	Offset	偏移
	///	@param[in]	Length	长度
	///	@param[in]	Access	存取权限
	///	@return	映射的流
	///	@note	会使上次映射的流失效\n
	///			隐含调用Bind
	virtual natRefPointer<natStream> MapBufferRange(nuInt Offset, nuInt Length, nuInt Access) = 0;
	///	@brief	刷新映射的部分缓存
	///	@param[in]	Offset	偏移
	///	@param[in]	Length	长度
	///	@note	会使上次映射的流失效\n
	///			隐含调用Bind
	virtual void FlushMappedBufferRange(nuInt Offset, nuInt Length) = 0;

	///	@brief	丢弃缓存数据
	///	@note	隐含调用Bind
	virtual void InvalidateData() = 0;
	///	@brief	丢弃缓存子数据
	///	@param[in]	Offset	偏移
	///	@param[in]	Length	长度
	///	@note	隐含调用Bind
	virtual void InvalidateSubData(nuInt Offset, nuInt Length) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	着色器
////////////////////////////////////////////////////////////////////////////////
struct n2dShader
	: n2dInterface
{
	///	@brief	着色器类型
	enum class ShaderType
	{
		Vertex,			///< @brief	顶点着色器
		Fragment,		///< @brief	片段着色器
		Geometry,		///< @brief	几何着色器
	};

	///	@brief	获得内部句柄
	///	@note	请勿手动删除
	virtual GLhandle GetHandle() const = 0;
	///	@brief	获得着色器类型
	virtual ShaderType GetType() const = 0;
	///	@brief	是否已设定删除标记
	virtual nBool Deleted() const = 0;
	///	@brief	是否已经编译
	virtual nBool Compiled() const = 0;
	///	@brief	获得编译日志
	virtual ncTStr GetInfoLog() const = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	着色器程序
////////////////////////////////////////////////////////////////////////////////
struct n2dShaderProgram
	: n2dInterface
{
	///	@brief	着色器变量类型
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

	///	@brief	Attribute的Size
	///	@note	可以为1, 2, 3, 4, AttribSize_BGRA
	enum AttribSize : nuInt
	{
		AttribSize_BGRA,
	};

	///	@brief	Attribute的内部类型
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

	///	@brief	Attribute引用
	///	@note	程序销毁时失效
	struct AttributeReference
		: n2dInterface
	{
		///	@brief	获得关联的程序
		///	@note	程序销毁后返回nullptr
		virtual n2dShaderProgram* GetProgram() const = 0;

		///	@brief	获得大小
		///	@note	GetSize获得的大小是元素个数，GetInternalSize获得的大小是实际占的字节数
		virtual nuInt GetSize() const = 0;
		///	@brief	获得内部大小
		///	@see	GetSize
		virtual nuInt GetInternalSize() const = 0;
		///	@brief	获得变量类型
		virtual VarType GetType() const = 0;

		///	@brief	启用顶点属性
		///	@param[in]	Value	是否启用
		virtual void EnablePointer(nBool Value = true) const = 0;
		///	@brief	顶点属性是否启用
		virtual nBool isEnable() const = 0;

		///	@brief	获得顶点属性绑定的属性值指针
		virtual const void* GetPointer() const = 0;
		///	@brief	设置顶点属性绑定的属性值指针
		///	@param[in]	Size		元素个数
		///	@param[in]	Type		Attribute内部类型
		///	@param[in]	Normalized	归一化
		///	@param[in]	Stride		元素间距
		///	@param[in]	pPointer	属性值指针
		///	@return	处理结果
		virtual nResult SetPointer(nuInt Size, AttribType Type, nBool Normalized, nuInt Stride, const void* pPointer) = 0;

		///	@brief	设置静态值
		///	@return	处理结果
		virtual nResult SetStaticValue(nBool Normalized, nBool Explicit, const void* pValue) = 0;
	};

	///	@brief	Uniform引用
	///	@note	程序销毁时失效
	struct UniformReference
		: n2dInterface
	{
		///	@brief	获得关联的程序
		///	@note	程序销毁后返回nullptr
		virtual n2dShaderProgram* GetProgram() const = 0;

		///	@brief	获得变量类型
		virtual VarType GetType() const = 0;
		///	@brief	获得大小
		virtual nuInt GetSize() const = 0;
		///	@brief	获得值
		///	@param[in]	Size	Value指向的内存大小
		///	@param[out]	Value	输出到的缓冲区
		virtual void GetValue(nuInt Size, void* Value) const = 0;
		///	@brief	设置值
		///	@param[in]	Count		元素个数
		///	@param[in]	Value		指向要设置的值的指针
		///	@param[in]	Transpose	当变量类型为矩阵时，是否自动转置矩阵
		///	@note	当变量类型并非矩阵时，Transpose被忽略
		virtual void SetValue(nuInt Count, const void* Value, nBool Transpose = false) = 0;
	};

	///	@brief	Uniform块引用
	///	@note	程序销毁时失效
	struct UniformBlockReference
		: n2dInterface
	{
		///	@brief	获得关联的程序
		///	@note	程序销毁后返回nullptr
		virtual n2dShaderProgram* GetProgram() const = 0;

		///	@brief	获得大小
		virtual nuInt GetSize() const = 0;
		///	@brief	获得有效的Uniform个数
		virtual nuInt ActiveUniformCount() const = 0;

		///	@brief	绑定到Buffer
		virtual void Bind(n2dBuffer* pBuffer) = 0;
		///	@brief	获得绑定点
		virtual nuInt GetBindingPoint() const = 0;
	};

	///	@brief	获得内部句柄
	///	@note	请勿手动删除
	virtual GLhandle GetHandle() const = 0;

	///	@brief	附加着色器
	virtual void AttachShader(n2dShader* pShader) = 0;
	///	@brief	分离着色器
	virtual void DetachShader(n2dShader* pShader) = 0;

	///	@brief	获得附加的着色器的个数
	virtual nuInt AttachedShaderCount() const = 0;

	///	@brief	使程序可以被获得二进制文件
	///	@note	仅在连接前设置有效
	virtual void SetBinaryRetrievable(nBool Value = true) = 0;
	///	@brief	使程序可以分离
	///	@note	仅在连接前设置有效
	virtual void SetSeparable(nBool Value = true) = 0;

	///	@brief	连接着色器
	virtual void Link() = 0;
	///	@brief	是否已连接
	virtual nBool IsLinked() const = 0;
	///	@brief	验证着色器
	///	@return	验证结果
	virtual nBool Validate() const = 0;
	///	@brief	获得信息日志
	virtual ncTStr GetInfoLog() const = 0;
	///	@brief	使用着色器
	virtual void Use() const = 0;
	///	@brief	该着色器是否正在使用
	virtual nBool IsUsing() const = 0;

	///	@brief	输出二进制文件
	///	@param[inout]	pStream	输出到的流
	///	@return	处理结果
	virtual nResult OutputBinary(natStream* pStream) const = 0;

	///	@brief	获得有效的Attribute个数
	virtual nuInt ActiveAttributeCount() const = 0;
	///	@brief	获得有效的Uniform个数
	virtual nuInt ActiveUniformCount() const = 0;
	///	@brief	获得有效的Uniform块个数
	virtual nuInt ActiveUniformBlockCount() const = 0;

	///	@brief	获得Attribute引用
	///	@param[in]	Location	位置
	virtual AttributeReference* GetAttributeReference(nuInt Location) = 0;
	///	@brief	获得Attribute引用
	///	@param[in]	Name	名称
	virtual AttributeReference* GetAttributeReference(ncTStr Name) = 0;

	///	@brief	获得Uniform引用
	///	@param[in]	Location	位置
	virtual UniformReference* GetUniformReference(nuInt Location) = 0;
	///	@brief	获得Uniform引用
	///	@param[in]	Name	名称
	virtual UniformReference* GetUniformReference(ncTStr Name) = 0;

	///	@brief	获得Uniform块引用
	///	@param[in]	Location	位置
	virtual UniformBlockReference* GetUniformBlockReference(nuInt Location) = 0;
	///	@brief	获得Uniform块引用
	///	@param[in]	Name	名称
	virtual UniformBlockReference* GetUniformBlockReference(ncTStr Name) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	程序管线
////////////////////////////////////////////////////////////////////////////////
struct n2dProgramPipeline
	: n2dInterface
{
	///	@brief	绑定到的地点
	enum Stages : nuInt
	{
		Vertex			= 0x00000001,	///< @brief	顶点处理
		Fragment		= 0x00000002,	///< @brief	片元处理
		Geometry		= 0x00000004,	///< @brief	几何处理
		TessControl		= 0x00000008,	///< @brief	细分控制
		TessEvaluation	= 0x00000010,	///< @brief	细分评估
		Compute			= 0x00000020,	///< @brief	计算
		All				= 0xFFFFFFFF,	///< @brief	全部
	};

	///	@brief	获得内部句柄
	virtual GLhandle GetHandle() const = 0;
	///	@brief	绑定当前程序管线
	virtual void Bind() const = 0;
	///	@brief	判断当前是否绑定
	virtual nBool IsBinding() const = 0;

	///	@brief	绑定程序到管线步骤
	///	@param[in]	stages		步骤
	///	@param[in]	pProgram	着色器程序
	virtual void UseProgramStages(nuInt stages, n2dShaderProgram* pProgram) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	着色器包装器
////////////////////////////////////////////////////////////////////////////////
struct n2dShaderWrapper
	: n2dInterface
{
	///	@brief	从流中创建着色器
	virtual nResult CreateShaderFromStream(natStream* pStream, n2dShader::ShaderType shaderType, nBool bIsBinary, n2dShader** pOut) = 0;
	///	@brief	创建着色器程序
	virtual nResult CreateProgram(n2dShaderProgram** pOut) = 0;
	///	@brief	从流中创建着色器程序
	///	@note	加载的必须是着色器程序的二进制编译
	virtual nResult CreateProgramFromStream(natStream* pStream, n2dShaderProgram** pOut) = 0;

	///	@brief		获得当前使用的着色器程序
	///	@warning	释放指针会导致着色器对象被删除
	virtual n2dShaderProgram* GetCurrentProgram() = 0;
	///	@brief		获得当前使用的程序管线
	///	@warning	释放指针会导致管线对象被删除
	virtual n2dProgramPipeline* GetCurrentProgramPipeline() = 0;

	///	@brief		获得默认程序
	///	@warning	请勿自行释放
	virtual n2dShaderProgram* GetDefaultProgram() const = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	渲染设备
////////////////////////////////////////////////////////////////////////////////
struct n2dRenderDevice
	: n2dInterface
{
	///	@brief	清除缓存
	enum ClearBit : nuInt
	{
		ClearBit_Color		= 0x00004000,	///< @brief	颜色缓存
		ClearBit_Depth		= 0x00000100,	///< @brief	深度缓存
		ClearBit_Stencil	= 0x00000400,	///< @brief	模板缓存
	};

	///	@brief	特性
	enum class Capability
	{
		Blend,						///< @brief	混合
		ColorLogicOp,				///< @brief	颜色逻辑操作
		CullFace,					///< @brief	剔除面
		DebugOutput,				///< @brief	调试输出
		DebugOutputSynchronous,		///< @brief	同步调试输出
		DepthClamp,					///< @brief	深度限定
		DepthTest,					///< @brief	深度测试
		Dither,						///< @brief	抖动
		Framebuffer_SRGB,			///< @brief	强制帧缓存使用SRGB
		LineSmooth,					///< @brief	线段反走样
		Multisample,				///< @brief	多重采样
		PolygonOffsetFill,			///< @brief	Fill模式多边形偏移
		PolygonOffsetLine,			///< @brief	Line模式多边形偏移
		PolygonOffsetPoint,			///< @brief	Point模式多边形偏移
		PolygonSmooth,				///< @brief	多边形反走样
		PrimitiveRestart,			///< @brief	图元重启动
		PrimitiveRestartFixedIndex,	///< @brief	使用固定的索引图元重启动
		RasterizerDiscard,			///< @brief	光栅化丢弃
		SampleAlphaToCoverage,		///< @brief	由采样Alpha值计算临时覆盖值
		SampleAlphaToOne,			///< @brief	采样Alpha值始终最大
		SampleCoverage,				///< @brief	片元覆盖值和临时覆盖值进行与操作
		SampleShading,				///< @brief	对每个覆盖采样执行一次片段着色器
		SampleMask,					///< @brief	采样掩码
		ScissorTest,				///< @brief	剪裁测试
		StencilTest,				///< @brief	模板测试
		TextureCubeMapSeamless,		///< @brief	无缝立方图纹理
		ProgramPointSize,			///< @brief	着色器程序中PointSize
	};

	///	@brief	使用索引的特性
	enum class CapabilityI
	{
		ClipDistance,				///< @brief	夹距
	};

	///	@brief	混合因子
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

	///	@brief	关联渲染上下文到此线程
	virtual void MakeCurrent() = 0;

	///	@brief	清除指定缓存
	///	@param	clearBit	指定清除的缓存（按位指定）
	virtual void Clear(nuInt clearBit) = 0;

	///	@brief	启用特性
	virtual void EnableCapability(Capability capability) = 0;
	///	@brief	禁用特性
	virtual void DisableCapability(Capability capability) = 0;
	///	@brief	按索引启用特性
	virtual void EnableCapabilityI(CapabilityI capability, nuInt Index) = 0;
	///	@brief	按索引禁用特性
	virtual void DisableCapabilityI(CapabilityI capability, nuInt Index) = 0;
	
	///	@brief	判断特性是否已启用
	virtual nBool IsCapabilityEnabled(Capability capability) const = 0;
	///	@brief	按索引判断特性是否已启用
	virtual nBool IsCapabilityIEnabled(CapabilityI capability, nuInt Index) const = 0;

	///	@brief	设置混合模式
	///	@param[in]	Source		源混合因子
	///	@param[in]	Destination	目标混合因子
	virtual void SetBlendMode(BlendFactor Source, BlendFactor Destination) = 0;
	///	@brief	为特定的DrawBuffer设置混合模式
	///	@param[in]	Source		源混合因子
	///	@param[in]	Destination	目标混合因子
	virtual void SetBlendModeI(n2dBuffer* Buf, BlendFactor Source, BlendFactor Destination) = 0;
	///	@brief	设置混合颜色
	virtual void SetBlendColor(natVec4<> const& Color) = 0;
	///	@brief	设置混合颜色
	virtual void SetBlendColor(nFloat r, nFloat g, nFloat b, nFloat a) = 0;

	///	@brief	设置同步频率
	///	@note	设置为0视为关闭垂直同步
	virtual void SetSwapInterval(nuInt Interval) = 0;

	///	@brief	交换缓冲区
	virtual void SwapBuffers() = 0;

	///	@brief	获得着色器包装器
	virtual n2dShaderWrapper* GetShaderWrapper() = 0;

	///	@brief	提交模型矩阵
	///	@param[in]	Mat	提交的矩阵
	virtual void SubmitModelMat(natMat4<> const& Mat) = 0;
	///	@brief	提交观察矩阵
	///	@param[in]	Mat	提交的矩阵
	virtual void SubmitViewMat(natMat4<> const& Mat) = 0;
	///	@brief	提交投影矩阵
	///	@param[in]	Mat	提交的矩阵
	virtual void SubmitProjMat(natMat4<> const& Mat) = 0;

	///	@brief	将当前模型矩阵压入栈
	///	@note	不会引发MVP矩阵更新
	virtual void PushModelMat() = 0;
	///	@brief	将当前观察矩阵压入栈
	///	@note	不会引发MVP矩阵更新
	virtual void PushViewMat() = 0;
	///	@brief	将当前投影矩阵压入栈
	///	@note	不会引发MVP矩阵更新
	virtual void PushProjMat() = 0;

	///	@brief	从栈中弹出1个模型矩阵
	///	@return	操作是否成功
	virtual nBool PopModelMat() = 0;
	///	@brief	从栈中弹出1个观察矩阵
	///	@return	操作是否成功
	virtual nBool PopViewMat() = 0;
	///	@brief	从栈中弹出1个投影矩阵
	///	@return	操作是否成功
	virtual nBool PopProjMat() = 0;

	///	@brief	将当前所有矩阵压入栈
	virtual void PushMVPMat() = 0;
	///	@brief	从栈中弹出1个所有矩阵
	///	@return	操作是否成功
	virtual nBool PopMVPMat() = 0;
	///	@brief	初始化MVP矩阵
	///	@note	仅覆盖当前栈顶矩阵
	virtual void InitMVPMat() = 0;

	///	@brief	获得当前模型矩阵
	virtual natMat4<> const& GetCurModelMat() const = 0;
	///	@brief	获得当前观察矩阵
	virtual natMat4<> const& GetCurViewMat() const = 0;
	///	@brief	获得当前投影矩阵
	virtual natMat4<> const& GetCurProjMat() const = 0;

	///	@brief	获得模型观察投影矩阵
	///	@return	当前的模型观察投影矩阵
	virtual natMat4<> const& GetMVPMat() = 0;

	///	@brief	获得关联的引擎
	virtual n2dEngine* GetEngine() = 0;

	///	@brief	创建缓存
	///	@param[in]	DefaultTarget	初始目标
	///	@param[out]	pOut			创建的缓存
	///	@return	处理结果
	virtual nResult CreateBuffer(n2dBuffer::BufferTarget DefaultTarget, n2dBuffer** pOut) = 0;

	///	@brief	创建二维图元渲染器
	///	@param[out]	pOut	创建的二维图元渲染器
	///	@return	处理结果
	virtual nResult CreateGraphics2D(n2dGraphics2D** pOut) = 0;
	///	@brief	创建三维图元渲染器
	///	@param[out]	pOut	创建的三维图元渲染器
	///	@return	处理结果
	virtual nResult CreateGraphics3D(n2dGraphics3D** pOut) = 0;

	///	@brief	创建纹理
	///	@param[out]	pOut	创建的纹理
	///	@return	处理结果
	virtual nResult CreateTexture(n2dTexture2D** pOut) = 0;
	///	@brief	从流中创建纹理
	///	@param[out]	pOut	创建的纹理
	///	@return	处理结果
	virtual nResult CreateTextureFromStream(natStream* pStream, DWORD dwFileType, n2dTexture2D** pOut) = 0;

	///	@brief	创建模型加载器
	///	@param[out]	pOut	创建的模型加载器
	///	@return	处理结果
	virtual nResult CreateModelLoader(n2dModelLoader** pOut) = 0;

	///	@brief	创建Obj模型加载器
	///	@param[out]	pOut	创建的Obj模型加载器
	///	@return	处理结果
	///	@deprecated	仅作者测试用
	virtual nResult CreateObjLoader(n2dModelLoader** pOut) = 0;
};