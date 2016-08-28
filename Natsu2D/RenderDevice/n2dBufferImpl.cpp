#include "n2dBufferImpl.h"
#include "n2dShaderWrapperImpl.h"
#include <natStream.h>

#ifdef max
#	undef max
#endif

n2dBufferImpl::n2dBufferImpl(BufferTarget DefaultTarget, n2dShaderWrapperImpl* pShaderWrapper)
	:	m_Buffer(0u),
		m_Target(GetBufferTargetEnum(DefaultTarget)),
	  	m_Size(0),
		m_BindPoint(std::numeric_limits<nuInt>::max()),
		m_pMappedBuffer(nullptr),
		m_pShaderWrapper(pShaderWrapper)
{
	glGenBuffers(1, &m_Buffer);
}

n2dBufferImpl::~n2dBufferImpl()
{
	m_pShaderWrapper->ReleaseBindingPoint(m_BindPoint);
	glDeleteBuffers(1, &m_Buffer);
}

GLhandle n2dBufferImpl::GetBuffer() const
{
	return m_Buffer;
}

void n2dBufferImpl::SetTarget(BufferTarget Target)
{
	m_Target = GetBufferTargetEnum(Target);
}

n2dBuffer::BufferTarget n2dBufferImpl::GetTarget() const
{
	return GetBufferTarget(m_Target);
}

void n2dBufferImpl::Bind() const
{
	glBindBuffer(m_Target, m_Buffer);
}

void n2dBufferImpl::BindBase()
{
	glBindBufferBase(m_Target, GetBindingPoint(), m_Buffer);
}

void n2dBufferImpl::BindBase(nuInt BindingPoint)
{
	if (m_BindPoint != std::numeric_limits<GLuint>::max())
	{
		m_pShaderWrapper->ReleaseBindingPoint(m_BindPoint);
	}

	m_BindPoint = BindingPoint;
	m_pShaderWrapper->UseBindingPoint(BindingPoint);
	BindBase();
}

void n2dBufferImpl::BindRange(nInt Offset, nuInt size)
{
	glBindBufferRange(m_Target, GetBindingPoint(), m_Buffer, Offset, size);
}

void n2dBufferImpl::BindRange(nuInt BindingPoint, nInt Offset, nuInt size)
{
	if (m_BindPoint != std::numeric_limits<GLuint>::max())
	{
		m_pShaderWrapper->ReleaseBindingPoint(m_BindPoint);
	}

	m_BindPoint = BindingPoint;
	BindRange(Offset, size);
}

void n2dBufferImpl::AllocData(nuInt Size, ncData pInitData, BufferUsage Usage)
{
	Bind();
	glBufferData(m_Target, Size, pInitData, GetBufferUsage(Usage));
	glGetBufferParameteriv(m_Target, GL_BUFFER_SIZE, reinterpret_cast<GLint*>(&m_Size));
}

void n2dBufferImpl::AllocSubData(nuInt Offset, nuInt Size, ncData pData)
{
	Bind();
	glBufferSubData(m_Target, Offset, Size, pData);
	glGetBufferParameteriv(m_Target, GL_BUFFER_SIZE, reinterpret_cast<GLint*>(&m_Size));
}

void n2dBufferImpl::ClearData(InternalFormat internalFormat, GLenum Format, GLenum Type, ncData pData)
{
	Bind();
	glClearBufferData(m_Target, GetInternalFormat(internalFormat), Format, Type, pData);
	glGetBufferParameteriv(m_Target, GL_BUFFER_SIZE, reinterpret_cast<GLint*>(&m_Size));
}

void n2dBufferImpl::ClearSubData(InternalFormat internalFormat, nuInt Offset, nuInt Size, GLenum Format, GLenum Type, ncData pData)
{
	Bind();
	glClearBufferSubData(m_Target, GetInternalFormat(internalFormat), Offset, Size, Format, Type, pData);
	glGetBufferParameteriv(m_Target, GL_BUFFER_SIZE, reinterpret_cast<GLint*>(&m_Size));
}

void n2dBufferImpl::CopySubDataTo(BufferTarget Target, nuInt ReadOffset, nuInt WriteOffset, nuInt Size)
{
	Bind();
	glCopyBufferSubData(m_Target, GetBufferTargetEnum(Target), ReadOffset, WriteOffset, Size);
}

nuInt n2dBufferImpl::GetBufferSize() const
{
	return m_Size;
}

nuInt n2dBufferImpl::GetBindingPoint() const
{
	if (m_BindPoint == std::numeric_limits<nuInt>::max())
	{
		m_BindPoint = m_pShaderWrapper->GetAvailableBindPoint();
	}

	return m_BindPoint;
}

void n2dBufferImpl::GetSubData(nuInt Offset, nuInt Size, nData pData)
{
	Bind();
	glGetBufferSubData(m_Target, Offset, Size, pData);
}

natStream* n2dBufferImpl::MapBuffer(BufferAccess Access)
{
	SafeRelease(m_pMappedBuffer);

	Bind();
	auto pData = static_cast<nData>(glMapBuffer(m_Target, GetBufferAccess(Access)));
	if (!pData)
	{
		return nullptr;
	}

	return natMemoryStream::CreateFromExternMemory(pData, GetBufferSize(), Access != BufferAccess::WriteOnly, Access != BufferAccess::ReadOnly);
}

void n2dBufferImpl::UnmapBuffer()
{
	SafeRelease(m_pMappedBuffer);

	Bind();
	glUnmapBuffer(m_Target);
}

natStream* n2dBufferImpl::MapBufferRange(nuInt Offset, nuInt Length, nuInt Access)
{
	SafeRelease(m_pMappedBuffer);

	Bind();
	auto pData = static_cast<nData>(glMapBufferRange(m_Target, Offset, Length, Access));
	if (!pData)
	{
		return nullptr;
	}

	return natMemoryStream::CreateFromExternMemory(pData, GetBufferSize(), (Access & Read) != NULL, (Access & Write) != NULL);
}

void n2dBufferImpl::FlushMappedBufferRange(nuInt Offset, nuInt Length)
{
	SafeRelease(m_pMappedBuffer);

	Bind();
	glFlushMappedBufferRange(m_Target, Offset, Length);
}

void n2dBufferImpl::InvalidateData()
{
	SafeRelease(m_pMappedBuffer);

	Bind();
	glInvalidateBufferData(m_Buffer);
}

void n2dBufferImpl::InvalidateSubData(nuInt Offset, nuInt Length)
{
	SafeRelease(m_pMappedBuffer);

	Bind();
	glInvalidateBufferSubData(m_Buffer, Offset, Length);
}
