#include "n2dShaderWrapperImpl.h"
#include <natUtil.h>
#include <natException.h>
#include <natStream.h>
#include <string>
#include <natLog.h>

#ifdef max
#	undef max
#endif

n2dShaderWrapperImpl::n2dShaderWrapperImpl(n2dRenderDevice* pRenderDevice)
	: m_pRenderDevice(pRenderDevice),
	m_DefaultProgram(nullptr)
{
	GLint tmax = 0;
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &tmax);
	m_AvailableBindPoint.reserve(tmax);
	for (int i = 0; i < tmax; ++i)
	{
		m_AvailableBindPoint.insert(i);
	}
}

n2dShaderWrapperImpl::~n2dShaderWrapperImpl()
{
	SafeRelease(m_DefaultProgram);
}

nResult n2dShaderWrapperImpl::CreateShaderFromStream(natStream* pStream, n2dShader::ShaderType shaderType, nBool bIsBinary, n2dShader** pOut)
{
	if (pOut == nullptr)
	{
		return NatErr_InvalidArg;
	}

	try
	{
		n2dShaderImpl* pTmp = new n2dShaderImpl(shaderType);
		pTmp->CompileFromStream(pStream, bIsBinary);
		*pOut = pTmp;
	}
	catch (std::bad_alloc&)
	{
		throw natException(_T("n2dShaderWrapperImpl::CreateProgram"), _T("Failed to allocate memory"));
	}
	catch (natException& ex)
	{
		natException e(_T("n2dShaderWrapperImpl::CreateProgram"), _T("CreateProgram failed"), &ex);
		throw e;
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
}

nResult n2dShaderWrapperImpl::CreateProgram(n2dShaderProgram** pOut)
{
	if (pOut == nullptr)
	{
		return NatErr_InvalidArg;
	}

	try
	{
		*pOut = new n2dShaderProgramImpl;
	}
	catch (std::bad_alloc&)
	{
		throw natException(_T("n2dShaderWrapperImpl::CreateProgram"), _T("Failed to allocate memory"));
	}
	catch (natException& ex)
	{
		natException e(_T("n2dShaderWrapperImpl::CreateProgram"), _T("CreateProgram failed"), &ex);
		throw e;
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
}

nResult n2dShaderWrapperImpl::CreateProgramFromStream(natStream* pStream, n2dShaderProgram** pOut)
{
	if (pOut == nullptr)
	{
		return NatErr_InvalidArg;
	}

	try
	{
		*pOut = n2dShaderProgramImpl::CreateFromStream(pStream);
	}
	catch (std::bad_alloc&)
	{
		throw natException(_T("n2dShaderWrapperImpl::CreateProgramFromStream"), _T("Failed to allocate memory"));
	}
	catch (natException& ex)
	{
		natException e(_T("n2dShaderWrapperImpl::CreateProgramFromStream"), _T("CreateProgramFromStream failed"), &ex);
		throw e;
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
}

GLuint n2dShaderWrapperImpl::GetAvailableBindPoint()
{
	auto tItea = m_AvailableBindPoint.begin();
	auto tRet = *tItea;
	m_AvailableBindPoint.erase(tItea);
	return tRet;
}

void n2dShaderWrapperImpl::ReleaseBindPoint(GLuint BindPoint)
{
	if (m_AvailableBindPoint.find(BindPoint) != m_AvailableBindPoint.end())
	{
		natLog::GetInstance().LogWarn(natUtil::FormatString(_T("Try to release a bind point %u which is unused, continue anyway"), BindPoint));
		return;
	}

	m_AvailableBindPoint.insert(BindPoint);
}

n2dShaderProgram* n2dShaderWrapperImpl::GetCurrentProgram()
{
	GLint tUsingProgram = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &tUsingProgram);
	return new n2dShaderProgramImpl(tUsingProgram);
}

n2dProgramPipeline* n2dShaderWrapperImpl::GetCurrentProgramPipeline()
{
	GLint tBindingPipeline = 0;
	glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &tBindingPipeline);
	return new n2dProgramPipelineImpl(tBindingPipeline);
}

n2dShaderProgram* n2dShaderWrapperImpl::GetDefaultProgram() const
{
	return m_DefaultProgram;
}

n2dShaderImpl::n2dShaderImpl(ShaderType shaderType)
	: m_ShaderType(shaderType),
	m_Shader(0u)
{
	m_Shader = glCreateShader(GetShaderType(shaderType));
}

n2dShaderImpl::n2dShaderImpl(ShaderType shaderType, GLhandle shader)
	: m_ShaderType(shaderType),
	m_Shader(shader)
{
}

n2dShaderImpl::~n2dShaderImpl()
{
	glDeleteShader(m_Shader);
}

GLhandle n2dShaderImpl::GetHandle() const
{
	return m_Shader;
}

n2dShader::ShaderType n2dShaderImpl::GetType() const
{
	return m_ShaderType;
}

nBool n2dShaderImpl::Deleted() const
{
	GLint tDeleted = GL_FALSE;
	glGetShaderiv(m_Shader, GL_DELETE_STATUS, &tDeleted);
	return tDeleted == GL_TRUE;
}

nBool n2dShaderImpl::Compiled() const
{
	GLint tCompiled = GL_FALSE;
	glGetShaderiv(m_Shader, GL_COMPILE_STATUS, &tCompiled);
	return tCompiled == GL_TRUE;
}

ncTStr n2dShaderImpl::GetInfoLog() const
{
	GLint tLogLength = 0;
	glGetShaderiv(m_Shader, GL_INFO_LOG_LENGTH, &tLogLength);
	if (tLogLength <= 1)
	{
		return _T("");
	}

	nStr tLog = new nChar[tLogLength + 1];
	glGetShaderInfoLog(m_Shader, tLogLength, nullptr, tLog);
	m_Log.assign(tLog, tLog + tLogLength);
	SafeDelArr(tLog);

	return m_Log.c_str();
}

void n2dShaderImpl::CompileFromStream(natStream* pStream, nBool bIsBinary)
{
	if (!pStream)
	{
		return;
	}

	nuInt tLen = static_cast<nuInt>(pStream->GetSize() - pStream->GetPosition());
	nData tBuf = new nByte[tLen + 1u];
	tBuf[tLen] = 0;
	pStream->ReadBytes(tBuf, tLen);
	if (bIsBinary)
	{
		// Œ¥—È÷§
		glShaderBinary(1, &m_Shader, GetShaderType(m_ShaderType), tBuf, tLen);
	}
	else
	{
		glShaderSource(m_Shader, 1, reinterpret_cast<const GLchar* const*>(&tBuf), nullptr);
		glCompileShader(m_Shader);
	}
}

n2dShaderProgramImpl::AttributeReferenceImpl::AttributeReferenceImpl(n2dShaderProgramImpl* pProgram, GLhandle Location)
	: m_pProgram(pProgram),
	m_Location(Location)
{
	if (m_pProgram == nullptr)
	{
		throw natException(_T("n2dShaderProgramImpl::AttributeReferenceImpl::AttributeReferenceImpl"), _T("Cannot refer to a valid program"));
	}

	glGetVertexAttribiv(m_Location, GL_VERTEX_ATTRIB_ARRAY_SIZE, &m_Size);
	glGetVertexAttribiv(m_Location, GL_VERTEX_ATTRIB_ARRAY_TYPE, reinterpret_cast<GLint*>(&m_Type));
	glGetActiveAttrib(m_pProgram->GetHandle(), m_Location, 0, nullptr, &m_RealSize, &m_ExType, nullptr);
}

n2dShaderProgram* n2dShaderProgramImpl::AttributeReferenceImpl::GetProgram() const
{
	return m_pProgram;
}

nuInt n2dShaderProgramImpl::AttributeReferenceImpl::GetSize() const
{
	return m_Size;
}

nuInt n2dShaderProgramImpl::AttributeReferenceImpl::GetInternalSize() const
{
	return m_RealSize;
}

n2dShaderProgram::VarType n2dShaderProgramImpl::AttributeReferenceImpl::GetType() const
{
	return GetVarType(m_ExType);
}

void n2dShaderProgramImpl::AttributeReferenceImpl::EnablePointer(nBool Value) const
{
	(Value ? glEnableVertexAttribArray : glDisableVertexAttribArray)(m_Location);
}

nBool n2dShaderProgramImpl::AttributeReferenceImpl::isEnable() const
{
	GLint tRet = GL_FALSE;
	glGetVertexAttribiv(m_Location, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &tRet);

	return tRet == GL_TRUE;
}

const void* n2dShaderProgramImpl::AttributeReferenceImpl::GetPointer() const
{
	void* pRet = nullptr;
	glGetVertexAttribPointerv(m_Location, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pRet);

	return pRet;
}

nResult n2dShaderProgramImpl::AttributeReferenceImpl::SetPointer(nuInt Size, AttribType Type, nBool Normalized, nuInt Stride, const void* pPointer)
{
	switch (Type)
	{
	case n2dShaderProgram::AttribType::Byte:
	case n2dShaderProgram::AttribType::UnsignedByte:
	case n2dShaderProgram::AttribType::Short:
	case n2dShaderProgram::AttribType::UnsignedShort:
	case n2dShaderProgram::AttribType::Int:
	case n2dShaderProgram::AttribType::UnsignedInt:
		switch (m_Type)
		{
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		case GL_INT:
		case GL_UNSIGNED_INT:
			if (Size < 1 || Size > 4)
				return NatErr_InvalidArg;
			glVertexAttribIPointer(m_Location, Size, GetAttribTypeEnum(Type), Stride, pPointer);
			break;
		default:
			if (Size < 0 || Size > 4)
				return NatErr_InvalidArg;
			glVertexAttribPointer(m_Location, Size == 0 ? GL_BGRA : Size, GetAttribTypeEnum(Type), Normalized ? GL_TRUE : GL_FALSE, Stride, pPointer);
			break;
		}
		break;
	case n2dShaderProgram::AttribType::HalfFloat:
	case n2dShaderProgram::AttribType::Float:
	case n2dShaderProgram::AttribType::Fixed:
	case n2dShaderProgram::AttribType::Int_2_10_10_10_Rev:
	case n2dShaderProgram::AttribType::UnsignedInt_2_10_10_10_Rev:
	case n2dShaderProgram::AttribType::UnsignedInt_10F_11F_11F_Rev:
		if (Size < 0 || Size > 4)
			return NatErr_InvalidArg;
		glVertexAttribPointer(m_Location, Size == 0 ? GL_BGRA : Size, GetAttribTypeEnum(Type), Normalized ? GL_TRUE : GL_FALSE, Stride, pPointer);
		break;

	case n2dShaderProgram::AttribType::Double:
		if (m_Type == GL_DOUBLE)
		{
			if (Size < 1 || Size > 4)
				return NatErr_InvalidArg;
			glVertexAttribLPointer(m_Location, Size, GL_DOUBLE, Stride, pPointer);
		}
		else
		{
			if (Size < 0 || Size > 4)
				return NatErr_InvalidArg;
			glVertexAttribPointer(m_Location, Size == 0 ? GL_BGRA : Size, GetAttribTypeEnum(Type), Normalized ? GL_TRUE : GL_FALSE, Stride, pPointer);
		}
		break;
	default:
		break;
	}
	
	return NatErr_OK;
}

nResult n2dShaderProgramImpl::AttributeReferenceImpl::SetStaticValue(nBool Normalized, nBool Explicit, const void* pValue)
{
	switch (m_Type)
	{
	case GL_BYTE:
		if (m_Size == 4)
		{
			(Explicit ? glVertexAttrib4bv : (Normalized ? glVertexAttrib4Nbv : glVertexAttribI4bv))(m_Location, static_cast<const GLbyte*>(pValue));
			break;
		}
		return NatErr_IllegalState;
	case GL_SHORT:
		switch (m_Size)
		{
		case 1:
			glVertexAttrib1sv(m_Location, static_cast<const GLshort*>(pValue));
			break;
		case 2:
			glVertexAttrib2sv(m_Location, static_cast<const GLshort*>(pValue));
			break;
		case 3:
			glVertexAttrib3sv(m_Location, static_cast<const GLshort*>(pValue));
			break;
		case 4:
			(Explicit ? glVertexAttrib4sv : (Normalized ? glVertexAttrib4Nsv : glVertexAttribI4sv))(m_Location, static_cast<const GLshort*>(pValue));
			break;
		default:
			return NatErr_IllegalState;
		}
		break;
	case GL_INT:
	case GL_FIXED:
		switch (m_Size)
		{
		case 1:
			glVertexAttribI1iv(m_Location, static_cast<const GLint*>(pValue));
			break;
		case 2:
			glVertexAttribI2iv(m_Location, static_cast<const GLint*>(pValue));
			break;
		case 3:
			glVertexAttribI3iv(m_Location, static_cast<const GLint*>(pValue));
			break;
		case 4:
			(Explicit ? glVertexAttrib4iv : (Normalized ? glVertexAttrib4Niv : glVertexAttribI4iv))(m_Location, static_cast<const GLint*>(pValue));
			break;
		default:
			return NatErr_IllegalState;
		}
		break;
	case GL_UNSIGNED_BYTE:
		if (m_Size == 4)
		{
			(Explicit ? glVertexAttrib4ubv : (Normalized ? glVertexAttrib4Nubv : glVertexAttribI4ubv))(m_Location, static_cast<const GLubyte*>(pValue));
			break;
		}
		return NatErr_IllegalState;
	case GL_UNSIGNED_SHORT:
	case GL_HALF_FLOAT:
		if (m_Size == 4)
		{
			(Explicit ? glVertexAttrib4usv : (Normalized ? glVertexAttrib4Nusv : glVertexAttribI4usv))(m_Location, static_cast<const GLushort*>(pValue));
			break;
		}
		return NatErr_IllegalState;
	case GL_UNSIGNED_INT:
		if (m_Size == 4)
		{
			(Explicit ? glVertexAttrib4uiv : (Normalized ? glVertexAttrib4Nuiv : glVertexAttribI4uiv))(m_Location, static_cast<const GLuint*>(pValue));
			break;
		}
		return NatErr_IllegalState;
	case GL_FLOAT:
		switch (m_Size)
		{
		case 1:
			glVertexAttrib1fv(m_Location, static_cast<const GLfloat*>(pValue));
			break;
		case 2:
			glVertexAttrib2fv(m_Location, static_cast<const GLfloat*>(pValue));
			break;
		case 3:
			glVertexAttrib3fv(m_Location, static_cast<const GLfloat*>(pValue));
			break;
		case 4:
			glVertexAttrib4fv(m_Location, static_cast<const GLfloat*>(pValue));
			break;
		default:
			return NatErr_IllegalState;
		}
		break;
	case GL_INT_2_10_10_10_REV:
	case GL_UNSIGNED_INT_2_10_10_10_REV:
	case GL_UNSIGNED_INT_10F_11F_11F_REV:
		switch (m_Size)
		{
		case 1:
			glVertexAttribP1uiv(m_Location, m_Type, Normalized ? GL_TRUE : GL_FALSE, static_cast<const GLuint*>(pValue));
			break;
		case 2:
			glVertexAttribP2uiv(m_Location, m_Type, Normalized ? GL_TRUE : GL_FALSE, static_cast<const GLuint*>(pValue));
			break;
		case 3:
			glVertexAttribP3uiv(m_Location, m_Type, Normalized ? GL_TRUE : GL_FALSE, static_cast<const GLuint*>(pValue));
			break;
		case 4:
			glVertexAttribP4uiv(m_Location, m_Type, Normalized ? GL_TRUE : GL_FALSE, static_cast<const GLuint*>(pValue));
			break;
		default:
			return NatErr_IllegalState;
		}
		break;
	case GL_DOUBLE:
		switch (m_Size)
		{
		case 1:
			(Explicit ? glVertexAttrib1dv : glVertexAttribL1dv)(m_Location, static_cast<const GLdouble*>(pValue));
			break;
		case 2:
			(Explicit ? glVertexAttrib2dv : glVertexAttribL2dv)(m_Location, static_cast<const GLdouble*>(pValue));
			break;
		case 3:
			(Explicit ? glVertexAttrib3dv : glVertexAttribL3dv)(m_Location, static_cast<const GLdouble*>(pValue));
			break;
		case 4:
			(Explicit ? glVertexAttrib4dv : glVertexAttribL4dv)(m_Location, static_cast<const GLdouble*>(pValue));
			break;
		default:
			return NatErr_IllegalState;
		}
		break;
	default:
		return NatErr_IllegalState;
	}

	return NatErr_OK;
}

void n2dShaderProgramImpl::AttributeReferenceImpl::onProgramDestroy()
{
	m_pProgram = nullptr;
}

n2dShaderProgramImpl::UniformReferenceImpl::UniformReferenceImpl(n2dShaderProgramImpl* pProgram, GLhandle Location)
	: m_pProgram(pProgram),
	m_Location(Location)
{
	if (m_pProgram == nullptr)
	{
		throw natException(_T("n2dShaderProgramImpl::UniformReferenceImpl::UniformReferenceImpl"), _T("Cannot refer to a valid program"));
	}

	glGetActiveUniform(m_pProgram->GetHandle(), m_Location, 0, nullptr, &m_Size, &m_Type, nullptr);
}

n2dShaderProgram* n2dShaderProgramImpl::UniformReferenceImpl::GetProgram() const
{
	return m_pProgram;
}

n2dShaderProgram::VarType n2dShaderProgramImpl::UniformReferenceImpl::GetType() const
{
	return GetVarType(m_Type);
}

nuInt n2dShaderProgramImpl::UniformReferenceImpl::GetSize() const
{
	return m_Size;
}

void n2dShaderProgramImpl::UniformReferenceImpl::GetValue(nuInt Size, void* value) const
{
	if (!m_pProgram)
	{
		return;
	}

	switch (m_Type)
	{
	case GL_FLOAT:
	case GL_FLOAT_VEC2:
	case GL_FLOAT_VEC3:
	case GL_FLOAT_VEC4:
	case GL_FLOAT_MAT2:
	case GL_FLOAT_MAT3:
	case GL_FLOAT_MAT4:
	case GL_FLOAT_MAT2x3:
	case GL_FLOAT_MAT2x4:
	case GL_FLOAT_MAT3x2:
	case GL_FLOAT_MAT3x4:
	case GL_FLOAT_MAT4x2:
	case GL_FLOAT_MAT4x3:
		glGetnUniformfv(m_pProgram->m_Program, m_Location, Size, static_cast<GLfloat*>(value));
		break;

	case GL_DOUBLE:
	case GL_DOUBLE_VEC2:
	case GL_DOUBLE_VEC3:
	case GL_DOUBLE_VEC4:
	case GL_DOUBLE_MAT2:
	case GL_DOUBLE_MAT3:
	case GL_DOUBLE_MAT4:
	case GL_DOUBLE_MAT2x3:
	case GL_DOUBLE_MAT2x4:
	case GL_DOUBLE_MAT3x2:
	case GL_DOUBLE_MAT3x4:
	case GL_DOUBLE_MAT4x2:
	case GL_DOUBLE_MAT4x3:
		glGetnUniformdv(m_pProgram->m_Program, m_Location, Size, static_cast<GLdouble*>(value));
		break;

	case GL_INT:
	case GL_INT_VEC2:
	case GL_INT_VEC3:
	case GL_INT_VEC4:

	case GL_BOOL:
	case GL_BOOL_VEC2:
	case GL_BOOL_VEC3:
	case GL_BOOL_VEC4:
		glGetnUniformiv(m_pProgram->m_Program, m_Location, Size, static_cast<GLint*>(value));
		break;

	case GL_UNSIGNED_INT:
	case GL_UNSIGNED_INT_VEC2:
	case GL_UNSIGNED_INT_VEC3:
	case GL_UNSIGNED_INT_VEC4:
		glGetnUniformuiv(m_pProgram->m_Program, m_Location, Size, static_cast<GLuint*>(value));
		break;

	default:
		throw natException(_T("n2dShaderProgramImpl::UniformReferenceImpl::GetValue"), natUtil::FormatString(_T("Unknown type : %u"), m_Type));
	}

	if (glGetError() == GL_INVALID_OPERATION)
	{
		throw natException(_T("n2dShaderProgramImpl::UniformReferenceImpl::GetValue"), _T("Cannot get value, maybe size is too small"));
	}
}

void n2dShaderProgramImpl::UniformReferenceImpl::SetValue(nuInt count, const void* value, nBool transpose)
{
	if (!m_pProgram)
	{
		return;
	}

	m_pProgram->Use();
	switch (m_Type)
	{
	case GL_FLOAT:
		glUniform1fv(m_Location, count, static_cast<const GLfloat*>(value));
		break;
	case GL_FLOAT_VEC2:
		glUniform2fv(m_Location, count, static_cast<const GLfloat*>(value));
		break;
	case GL_FLOAT_VEC3:
		glUniform3fv(m_Location, count, static_cast<const GLfloat*>(value));
		break;
	case GL_FLOAT_VEC4:
		glUniform4fv(m_Location, count, static_cast<const GLfloat*>(value));
		break;
	case GL_FLOAT_MAT2:
		glUniformMatrix2fv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLfloat*>(value));
		break;
	case GL_FLOAT_MAT3:
		glUniformMatrix3fv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLfloat*>(value));
		break;
	case GL_FLOAT_MAT4:
		glUniformMatrix4fv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLfloat*>(value));
		break;
	case GL_FLOAT_MAT2x3:
		glUniformMatrix2x3fv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLfloat*>(value));
		break;
	case GL_FLOAT_MAT2x4:
		glUniformMatrix2x4fv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLfloat*>(value));
		break;
	case GL_FLOAT_MAT3x2:
		glUniformMatrix3x2fv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLfloat*>(value));
		break;
	case GL_FLOAT_MAT3x4:
		glUniformMatrix3x4fv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLfloat*>(value));
		break;
	case GL_FLOAT_MAT4x2:
		glUniformMatrix4x2fv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLfloat*>(value));
		break;
	case GL_FLOAT_MAT4x3:
		glUniformMatrix4x3fv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLfloat*>(value));
		break;

	case GL_DOUBLE:
		glUniform1dv(m_Location, count, static_cast<const GLdouble*>(value));
		break;
	case GL_DOUBLE_VEC2:
		glUniform2dv(m_Location, count, static_cast<const GLdouble*>(value));
		break;
	case GL_DOUBLE_VEC3:
		glUniform3dv(m_Location, count, static_cast<const GLdouble*>(value));
		break;
	case GL_DOUBLE_VEC4:
		glUniform4dv(m_Location, count, static_cast<const GLdouble*>(value));
		break;
	case GL_DOUBLE_MAT2:
		glUniformMatrix2dv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLdouble*>(value));
		break;
	case GL_DOUBLE_MAT3:
		glUniformMatrix3dv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLdouble*>(value));
		break;
	case GL_DOUBLE_MAT4:
		glUniformMatrix4dv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLdouble*>(value));
		break;
	case GL_DOUBLE_MAT2x3:
		glUniformMatrix2x3dv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLdouble*>(value));
		break;
	case GL_DOUBLE_MAT2x4:
		glUniformMatrix2x4dv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLdouble*>(value));
		break;
	case GL_DOUBLE_MAT3x2:
		glUniformMatrix3x2dv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLdouble*>(value));
		break;
	case GL_DOUBLE_MAT3x4:
		glUniformMatrix3x4dv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLdouble*>(value));
		break;
	case GL_DOUBLE_MAT4x2:
		glUniformMatrix4x2dv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLdouble*>(value));
		break;
	case GL_DOUBLE_MAT4x3:
		glUniformMatrix4x3dv(m_Location, count, transpose ? GL_TRUE : GL_FALSE, static_cast<const GLdouble*>(value));
		break;

	case GL_INT:
	case GL_BOOL:
		glUniform1iv(m_Location, count, static_cast<const GLint*>(value));
		break;
	case GL_INT_VEC2:
	case GL_BOOL_VEC2:
		glUniform2iv(m_Location, count, static_cast<const GLint*>(value));
		break;
	case GL_INT_VEC3:
	case GL_BOOL_VEC3:
		glUniform3iv(m_Location, count, static_cast<const GLint*>(value));
		break;
	case GL_INT_VEC4:
	case GL_BOOL_VEC4:
		glUniform4iv(m_Location, count, static_cast<const GLint*>(value));
		break;

	case GL_UNSIGNED_INT:
		glUniform1uiv(m_Location, count, static_cast<const GLuint*>(value));
		break;
	case GL_UNSIGNED_INT_VEC2:
		glUniform2uiv(m_Location, count, static_cast<const GLuint*>(value));
		break;
	case GL_UNSIGNED_INT_VEC3:
		glUniform3uiv(m_Location, count, static_cast<const GLuint*>(value));
		break;
	case GL_UNSIGNED_INT_VEC4:
		glUniform4uiv(m_Location, count, static_cast<const GLuint*>(value));
		break;

	default:
		throw natException(_T("n2dShaderProgramImpl::UniformReferenceImpl::SetValue"), natUtil::FormatString(_T("Unknown type : %u"), m_Type));
	}

	/*if (glGetError() != GL_NO_ERROR)
	{
		throw natException(_T("n2dShaderProgramImpl::UniformReferenceImpl::SetValue"), _T("Set uniform failed"));
	}*/
}

void n2dShaderProgramImpl::UniformReferenceImpl::onProgramDestroy()
{
	m_pProgram = nullptr;
}

n2dShaderProgramImpl::UniformBlockReferenceImpl::UniformBlockReferenceImpl(n2dShaderProgramImpl * pProgram, GLhandle Location)
	: m_pProgram(pProgram),
	m_Location(Location)
{
	if (m_pProgram == nullptr)
	{
		throw natException(_T("n2dShaderProgramImpl::UniformBlockReferenceImpl::UniformBlockReferenceImpl"), _T("Cannot refer to a valid program"));
	}

	glGetActiveUniformBlockiv(m_pProgram->GetHandle(), m_Location, GL_UNIFORM_BLOCK_DATA_SIZE, &m_Size);
	glGetActiveUniformBlockiv(m_pProgram->GetHandle(), m_Location, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &m_ActiveUniformCount);
}

n2dShaderProgram* n2dShaderProgramImpl::UniformBlockReferenceImpl::GetProgram() const
{
	return m_pProgram;
}

nuInt n2dShaderProgramImpl::UniformBlockReferenceImpl::GetSize() const
{
	return m_Size;
}

nuInt n2dShaderProgramImpl::UniformBlockReferenceImpl::ActiveUniformCount() const
{
	return m_ActiveUniformCount;
}

void n2dShaderProgramImpl::UniformBlockReferenceImpl::Bind(n2dBuffer* pBuffer)
{
	glUniformBlockBinding(m_pProgram->GetHandle(), m_Location, pBuffer->GetBindingPoint());
	pBuffer->BindBase();
}

nuInt n2dShaderProgramImpl::UniformBlockReferenceImpl::GetBindingPoint() const
{
	GLint tRet = 0;
	glGetActiveUniformBlockiv(m_pProgram->GetHandle(), m_Location, GL_UNIFORM_BLOCK_BINDING, &tRet);
	return tRet;
}

void n2dShaderProgramImpl::UniformBlockReferenceImpl::onProgramDestroy()
{
	m_pProgram = nullptr;
}

n2dShaderProgramImpl::n2dShaderProgramImpl(GLhandle ProgramID)
	: m_Program(ProgramID)
{
	if (0u == m_Program)
	{
		m_Program = glCreateProgram();
	}
}

n2dShaderProgramImpl::n2dShaderProgramImpl(n2dShaderImpl* pVertexShader, n2dShaderImpl* pFragmentShader, n2dShaderImpl* pGeometryShader)
	: n2dShaderProgramImpl(0u)
{
	AttachShader(pVertexShader);
	AttachShader(pFragmentShader);
	AttachShader(pGeometryShader);
}

n2dShaderProgramImpl::~n2dShaderProgramImpl()
{
	for (auto& i : m_AttribMap)
	{
		i.second->onProgramDestroy();
		SafeRelease(i.second);
	}

	for (auto& i : m_UniformMap)
	{
		i.second->onProgramDestroy();
		SafeRelease(i.second);
	}

	for (auto& i : m_UniformBlockMap)
	{
		i.second->onProgramDestroy();
		SafeRelease(i.second);
	}

	glDeleteProgram(m_Program);
}

GLhandle n2dShaderProgramImpl::GetHandle() const
{
	return m_Program;
}

void n2dShaderProgramImpl::AttachShader(n2dShader* pShader)
{
	if (pShader)
	{
		glAttachShader(m_Program, pShader->GetHandle());
	}
}

void n2dShaderProgramImpl::DetachShader(n2dShader* pShader)
{
	glDetachShader(m_Program, pShader->GetHandle());
}

nuInt n2dShaderProgramImpl::AttachedShaderCount() const
{
	return GetParameter(GL_ATTACHED_SHADERS);
}

void n2dShaderProgramImpl::SetBinaryRetrievable(nBool Value)
{
	glProgramParameteri(m_Program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, Value ? GL_TRUE : GL_FALSE);
}

void n2dShaderProgramImpl::SetSeparable(nBool Value)
{
	glProgramParameteri(m_Program, GL_PROGRAM_SEPARABLE, Value ? GL_TRUE : GL_FALSE);
}

void n2dShaderProgramImpl::Link()
{
	glLinkProgram(m_Program);
}

nBool n2dShaderProgramImpl::IsLinked() const
{
	return GetParameter(GL_LINK_STATUS) == GL_TRUE;
}

nBool n2dShaderProgramImpl::Validate() const
{
	glValidateProgram(m_Program);

	return GetParameter(GL_VALIDATE_STATUS) == GL_TRUE;
}

ncTStr n2dShaderProgramImpl::GetInfoLog() const
{
	GLint tLogLength = GetParameter(GL_INFO_LOG_LENGTH);
	if (tLogLength <= 1)
	{
		return _T("");
	}
	
	nStr tLog = new nChar[tLogLength + 1];
	//tLog[tLogLength] = '\0';
	glGetProgramInfoLog(m_Program, tLogLength, nullptr, tLog);
	m_Log.assign(tLog, tLog + tLogLength);
	return m_Log.c_str();
}

void n2dShaderProgramImpl::Use() const
{
	glUseProgram(m_Program);
}

nBool n2dShaderProgramImpl::IsUsing() const
{
	return GetParameter(GL_CURRENT_PROGRAM) == m_Program;
}

nResult n2dShaderProgramImpl::OutputBinary(natStream* pStream) const
{
	if (!pStream || !pStream->CanWrite())
	{
		return NatErr_InvalidArg;
	}

	nuInt tLength = GetParameter(GL_PROGRAM_BINARY_LENGTH);
	if (tLength <= 0)
	{
		return NatErr_InternalErr;
	}

	GLenum tBinaryFormat = 0u;

	nData pBuf = new nByte[tLength];
	glGetProgramBinary(m_Program, tLength, nullptr, &tBinaryFormat, pBuf);

	NatErr tErrCode;

	pStream->WriteBytes(reinterpret_cast<ncData>(&tBinaryFormat), sizeof(tBinaryFormat));
	if ((tErrCode = pStream->GetLastErr()) != NatErr_OK)
	{
		return tErrCode;
	}

	pStream->WriteBytes(reinterpret_cast<ncData>(&tLength), sizeof(tLength));
	if ((tErrCode = pStream->GetLastErr()) != NatErr_OK)
	{
		return tErrCode;
	}

	pStream->WriteBytes(pBuf, tLength);
	if ((tErrCode = pStream->GetLastErr()) != NatErr_OK)
	{
		return tErrCode;
	}

	return NatErr_OK;
}

nuInt n2dShaderProgramImpl::ActiveAttributeCount() const
{
	return GetParameter(GL_ACTIVE_ATTRIBUTES);
}

nuInt n2dShaderProgramImpl::ActiveUniformCount() const
{
	return GetParameter(GL_ACTIVE_UNIFORMS);
}

nuInt n2dShaderProgramImpl::ActiveUniformBlockCount() const
{
	return GetParameter(GL_ACTIVE_UNIFORM_BLOCKS);
}

n2dShaderProgram::AttributeReference* n2dShaderProgramImpl::GetAttributeReference(nuInt location)
{
	if (location >= ActiveAttributeCount())
	{
		return nullptr;
	}

	auto itea = m_AttribMap.find(location);
	if (itea != m_AttribMap.end())
	{
		return itea->second;
	}

	AttributeReferenceImpl* pRet = new AttributeReferenceImpl(this, location);
	m_AttribMap[location] = pRet;

	return pRet;
}

n2dShaderProgram::AttributeReference* n2dShaderProgramImpl::GetAttributeReference(ncTStr Name)
{
	GLhandle tLocation = glGetAttribLocation(m_Program,
#ifdef _UNICODE
		natUtil::W2Cstr(Name).c_str()
#else
		Name
#endif
		);

	return tLocation == GLhandle(-1) ? nullptr : GetAttributeReference(tLocation);
}

n2dShaderProgram::UniformReference* n2dShaderProgramImpl::GetUniformReference(nuInt location)
{
	if (location >= ActiveUniformCount())
	{
		return nullptr;
	}

	auto itea = m_UniformMap.find(location);
	if (itea != m_UniformMap.end())
	{
		return itea->second;
	}
	
	UniformReferenceImpl* pRet = new UniformReferenceImpl(this, location);
	m_UniformMap[location] = pRet;

	return pRet;
}

n2dShaderProgram::UniformReference* n2dShaderProgramImpl::GetUniformReference(ncTStr Name)
{
	GLhandle tLocation = glGetUniformLocation(m_Program,
#ifdef _UNICODE
		natUtil::W2Cstr(Name).c_str()
#else
		Name
#endif
		);

	return tLocation == GLhandle(-1) ? nullptr : GetUniformReference(tLocation);
}

n2dShaderProgram::UniformBlockReference* n2dShaderProgramImpl::GetUniformBlockReference(nuInt Location)
{
	if (Location >= ActiveUniformBlockCount())
	{
		return nullptr;
	}

	auto itea = m_UniformBlockMap.find(Location);
	if (itea != m_UniformBlockMap.end())
	{
		return itea->second;
	}

	UniformBlockReferenceImpl* pRet = new UniformBlockReferenceImpl(this, Location);
	m_UniformBlockMap[Location] = pRet;

	return pRet;
}

n2dShaderProgram::UniformBlockReference* n2dShaderProgramImpl::GetUniformBlockReference(ncTStr Name)
{
	GLhandle tLocation = glGetUniformBlockIndex(m_Program,
#ifdef _UNICODE
		natUtil::W2Cstr(Name).c_str()
#else
		Name
#endif
		);

	return tLocation == GLhandle(-1) ? nullptr : GetUniformBlockReference(tLocation);
}

n2dShaderProgramImpl* n2dShaderProgramImpl::CreateFromStream(natStream* pStream)
{
	if (!pStream || !pStream->CanRead())
	{
		return nullptr;
	}

	n2dShaderProgramImpl* pRet = new n2dShaderProgramImpl;
	
	GLenum tBinaryFormat = 0u;
	pStream->ReadBytes(reinterpret_cast<nData>(&tBinaryFormat), sizeof(tBinaryFormat));
	if (pStream->GetLastErr() != NatErr_OK)
	{
		return nullptr;
	}

	nuInt tLen = 0u;
	pStream->ReadBytes(reinterpret_cast<nData>(&tLen), sizeof(tLen));
	if (pStream->GetLastErr() != NatErr_OK)
	{
		return nullptr;
	}

	nData pBuf = new nByte[tLen];
	pStream->ReadBytes(pBuf, tLen);
	if (pStream->GetLastErr() != NatErr_OK)
	{
		return nullptr;
	}

	glProgramBinary(pRet->m_Program, tBinaryFormat, pBuf, tLen);

	return pRet;
}

n2dProgramPipelineImpl::n2dProgramPipelineImpl(GLhandle PipelineID)
	: m_Pipeline(PipelineID)
{
	if (0u == m_Pipeline)
	{
		glGenProgramPipelines(1, &m_Pipeline);
	}
}

n2dProgramPipelineImpl::~n2dProgramPipelineImpl()
{
	glDeleteProgramPipelines(1, &m_Pipeline);
}

GLhandle n2dProgramPipelineImpl::GetHandle() const
{
	return m_Pipeline;
}

void n2dProgramPipelineImpl::Bind() const
{
	glBindProgramPipeline(m_Pipeline);
}

nBool n2dProgramPipelineImpl::IsBinding() const
{
	GLint tResult = 0;
	glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &tResult);
	return tResult == m_Pipeline;
}

void n2dProgramPipelineImpl::UseProgramStages(nuInt stages, n2dShaderProgram* pProgram)
{
	glUseProgramStages(m_Pipeline, stages, pProgram->GetHandle());
}
