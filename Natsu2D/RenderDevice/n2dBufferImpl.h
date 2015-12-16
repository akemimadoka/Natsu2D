#pragma once
#include "..\n2dRenderDevice.h"
#include "OpenGL.h"

class n2dShaderWrapperImpl;

class n2dBufferImpl
	: public natRefObjImpl<n2dBuffer>
{
	static GLenum GetBufferTargetEnum(BufferTarget bufferTarget)
	{
		switch (bufferTarget)
		{
		case BufferTarget::ArrayBuffer:
			return GL_ARRAY_BUFFER;
		case BufferTarget::AtomicCounterBuffer:
			return GL_ATOMIC_COUNTER_BUFFER;
		case BufferTarget::CopyReadBuffer:
			return GL_COPY_READ_BUFFER;
		case BufferTarget::CopyWriteBuffer:
			return GL_COPY_WRITE_BUFFER;
		case BufferTarget::DispatchIndirectBuffer:
			return GL_DISPATCH_INDIRECT_BUFFER;
		case BufferTarget::DrawIndirectBuffer:
			return GL_DRAW_INDIRECT_BUFFER;
		case BufferTarget::ElementArrayBuffer:
			return GL_ELEMENT_ARRAY_BUFFER;
		case BufferTarget::PixelPackBuffer:
			return GL_PIXEL_PACK_BUFFER;
		case BufferTarget::PixelUnpackBuffer:
			return GL_PIXEL_UNPACK_BUFFER;
		case BufferTarget::QueryBuffer:
			return GL_QUERY_BUFFER;
		case BufferTarget::ShaderStorageBuffer:
			return GL_SHADER_STORAGE_BUFFER;
		case BufferTarget::TextureBuffer:
			return GL_TEXTURE_BUFFER;
		case BufferTarget::TransformFeedbackBuffer:
			return GL_TRANSFORM_FEEDBACK_BUFFER;
		case BufferTarget::UniformBuffer:
			return GL_UNIFORM_BUFFER;
		default:
			throw natException(_T("n2dBufferImpl::GetBufferTargetEnum"), _T("Not an available BufferTarget"));
		}
	}

	static BufferTarget GetBufferTarget(GLenum target)
	{
		switch (target)
		{
		case GL_ARRAY_BUFFER:
			return BufferTarget::ArrayBuffer;
		case GL_ATOMIC_COUNTER_BUFFER:
			return BufferTarget::AtomicCounterBuffer;
		case GL_COPY_READ_BUFFER:
			return BufferTarget::CopyReadBuffer;
		case GL_COPY_WRITE_BUFFER:
			return BufferTarget::CopyWriteBuffer;
		case GL_DISPATCH_INDIRECT_BUFFER:
			return BufferTarget::DispatchIndirectBuffer;
		case GL_DRAW_INDIRECT_BUFFER:
			return BufferTarget::DrawIndirectBuffer;
		case GL_ELEMENT_ARRAY_BUFFER:
			return BufferTarget::ElementArrayBuffer;
		case GL_PIXEL_PACK_BUFFER:
			return BufferTarget::PixelPackBuffer;
		case GL_PIXEL_UNPACK_BUFFER:
			return BufferTarget::PixelUnpackBuffer;
		case GL_QUERY_BUFFER:
			return BufferTarget::QueryBuffer;
		case GL_SHADER_STORAGE_BUFFER:
			return BufferTarget::ShaderStorageBuffer;
		case GL_TEXTURE_BUFFER:
			return BufferTarget::TextureBuffer;
		case GL_TRANSFORM_FEEDBACK_BUFFER:
			return BufferTarget::TransformFeedbackBuffer;
		case GL_UNIFORM_BUFFER:
			return BufferTarget::UniformBuffer;
		default:
			throw natException(_T("n2dBufferImpl::GetBufferTarget"), _T("Not an available target"));
		}
	}

	static GLenum GetBufferUsage(BufferUsage bufferUsage)
	{
		switch (bufferUsage)
		{
		case BufferUsage::StaticDraw:
			return GL_STATIC_DRAW;
		case BufferUsage::StaticRead:
			return GL_STATIC_READ;
		case BufferUsage::StaticCopy:
			return GL_STATIC_COPY;
		case BufferUsage::DynamicDraw:
			return GL_DYNAMIC_DRAW;
		case BufferUsage::DynamicRead:
			return GL_DYNAMIC_READ;
		case BufferUsage::DynamicCopy:
			return GL_DYNAMIC_COPY;
		case BufferUsage::StreamDraw:
			return GL_STREAM_DRAW;
		case BufferUsage::StreamRead:
			return GL_STREAM_READ;
		case BufferUsage::StreamCopy:
			return GL_STREAM_COPY;
		default:
			throw natException(_T("n2dBufferImpl::GetBufferUsage"), _T("Not an available BufferUsage"));
		}
	}

	static GLenum GetBufferAccess(BufferAccess bufferAccess)
	{
		switch (bufferAccess)
		{
		case BufferAccess::ReadOnly:
			return GL_READ_ONLY;
		case BufferAccess::WriteOnly:
			return GL_WRITE_ONLY;
		case BufferAccess::ReadWrite:
			return GL_READ_WRITE;
		default:
			throw natException(_T("n2dBufferImpl::GetBufferAccess"), _T("Not an available BufferAccess"));
		}
	}

	static GLenum GetInternalFormat(InternalFormat internalFormat)
	{
		switch (internalFormat)
		{
		case n2dBuffer::InternalFormat::R8:
			return GL_R8;
		case n2dBuffer::InternalFormat::R16:
			return GL_R16;
		case n2dBuffer::InternalFormat::R16F:
			return GL_R16F;
		case n2dBuffer::InternalFormat::R32F:
			return GL_R32F;
		case n2dBuffer::InternalFormat::R8I:
			return GL_R8I;
		case n2dBuffer::InternalFormat::R16I:
			return GL_R16I;
		case n2dBuffer::InternalFormat::R32I:
			return GL_R32I;
		case n2dBuffer::InternalFormat::R8UI:
			return GL_R8UI;
		case n2dBuffer::InternalFormat::R16UI:
			return GL_R16UI;
		case n2dBuffer::InternalFormat::R32UI:
			return GL_R32UI;
		case n2dBuffer::InternalFormat::RG8:
			return GL_RG8;
		case n2dBuffer::InternalFormat::RG16:
			return GL_RG16;
		case n2dBuffer::InternalFormat::RG16F:
			return GL_RG16F;
		case n2dBuffer::InternalFormat::RG32F:
			return GL_RG32F;
		case n2dBuffer::InternalFormat::RG8I:
			return GL_RG8I;
		case n2dBuffer::InternalFormat::RG16I:
			return GL_RG16I;
		case n2dBuffer::InternalFormat::RG32I:
			return GL_RG32I;
		case n2dBuffer::InternalFormat::RG8UI:
			return GL_RG8UI;
		case n2dBuffer::InternalFormat::RG16UI:
			return GL_RG16UI;
		case n2dBuffer::InternalFormat::RG32UI:
			return GL_RG32UI;
		case n2dBuffer::InternalFormat::RGB32F:
			return GL_RGB32F;
		case n2dBuffer::InternalFormat::RGB32I:
			return GL_RGB32I;
		case n2dBuffer::InternalFormat::RGB32UI:
			return GL_RGB32UI;
		case n2dBuffer::InternalFormat::RGBA8:
			return GL_RGBA8;
		case n2dBuffer::InternalFormat::RGBA16:
			return GL_RGBA16;
		case n2dBuffer::InternalFormat::RGBA16F:
			return GL_RGBA16F;
		case n2dBuffer::InternalFormat::RGBA32F:
			return GL_RGBA32F;
		case n2dBuffer::InternalFormat::RGBA8I:
			return GL_RGBA8I;
		case n2dBuffer::InternalFormat::RGBA16UI:
			return GL_RGBA16UI;
		case n2dBuffer::InternalFormat::RGBA32UI:
			return GL_RGBA32UI;
		default:
			throw natException(_T("n2dBufferImpl::GetInternalFormat"), _T("Not an available InternalFormat"));
		}
	}
public:
	explicit n2dBufferImpl(BufferTarget DefaultTarget, n2dShaderWrapperImpl* pShaderWrapper);
	~n2dBufferImpl();

	GLhandle GetBuffer() const override;
	void SetTarget(BufferTarget Target) override;
	BufferTarget GetTarget() const override;

	///	@brief	将Buffer绑定到当前Target
	///	@note	为了兼容性不使用OpenGL4.5的DSA
	void Bind() const override;
	void BindBase() override;
	void BindBase(nuInt BindingPoint) override;
	void BindRange(nInt Offset, nuInt size) override;
	void BindRange(nuInt BindingPoint, nInt Offset, nuInt size) override;
	///	@brief	申请Buffer空间
	///	@note	隐含调用Bind
	void AllocData(nuInt Size, ncData pInitData, BufferUsage Usage) override;
	///	@brief	申请Buffer子空间
	///	@note	隐含调用Bind
	void AllocSubData(nuInt Offset, nuInt Size, ncData pData) override;

	///	@brief	清除Buffer空间
	///	@note	隐含调用Bind
	void ClearData(InternalFormat internalFormat, GLenum Format, GLenum Type, ncData pData);// override;
	///	@brief	清除Buffer子空间
	///	@note	隐含调用Bind
	void ClearSubData(InternalFormat internalFormat, nuInt Offset, nuInt Size, GLenum Format, GLenum Type, ncData pData);// override;

	///	@brief	拷贝Buffer子数据到另一个Buffer
	///	@note	隐含调用Bind
	void CopySubDataTo(BufferTarget Target, nuInt ReadOffset, nuInt WriteOffset, nuInt Size) override;

	nuInt GetBufferSize() const override;
	nuInt GetBindingPoint() const override;

	///	@note	隐含调用Bind
	void GetSubData(nuInt Offset, nuInt Size, nData pData) override;

	///	@note	隐含调用Bind
	natStream* MapBuffer(BufferAccess Access) override;
	///	@note	隐含调用Bind
	void UnmapBuffer() override;

	///	@note	隐含调用Bind
	natStream* MapBufferRange(nuInt Offset, nuInt Length, nuInt Access) override;
	///	@note	隐含调用Bind
	void FlushMappedBufferRange(nuInt Offset, nuInt Length) override;

	///	@note	隐含调用Bind
	void InvalidateData() override;
	///	@note	隐含调用Bind
	void InvalidateSubData(nuInt Offset, nuInt Length) override;

private:
	GLhandle m_Buffer;
	GLenum m_Target;
	GLuint m_Size;
	mutable GLuint m_BindPoint;

	natStream* m_pMappedBuffer;

	n2dShaderWrapperImpl* m_pShaderWrapper;
};