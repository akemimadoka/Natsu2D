#include "n2dShaderWrapperImpl.h"
#include <natUtil.h>
#include <natException.h>
#include <natStream.h>
#include <string>

n2dShaderWrapperImpl::~n2dShaderWrapperImpl()
{
	for each (auto p in m_ProgramID)
	{
		glDeleteProgram(p);
	}

	glDeleteProgramPipelines(m_PipelineID.size(), m_PipelineID.data());
}

void n2dShaderWrapperImpl::AddShader(ShaderType shaderType, ncTStr filename, nBool bIsBinary)
{
	natFileStream* pStream = new natFileStream(filename, false);
	AddShader(shaderType, pStream, bIsBinary);
	pStream->Release();
}

void n2dShaderWrapperImpl::AddShader(ShaderType shaderType, natStream* pStream, nBool bIsBinary)
{
	nLen tLen = pStream->GetSize() - pStream->GetPosition();
	nData pBuf = new nByte[static_cast<nuInt>(tLen + 1)];
	// 记得字符串要以0结尾= =
	pBuf[tLen] = 0u;
	pStream->ReadBytes(pBuf, tLen);
	m_ShaderData[shaderType].emplace_back(std::make_pair(bIsBinary, std::vector<nByte>(pBuf, pBuf + tLen + 1)));
	SafeDelArr(pBuf);
}

GLuint n2dShaderWrapperImpl::CompileShader(ncData lpData, nLen lLen, ShaderType shaderType, nBool bIsBinary)
{
	GLuint tShader;
	if (bIsBinary)
	{
		glShaderBinary(1, &tShader, shaderType, lpData, static_cast<GLsizei>(lLen));
	}
	else
	{
		GLint tResult, tInfoLogLength;
		tShader = glCreateShader(shaderType);
		glShaderSource(tShader, 1, (const GLchar * const*)&lpData, NULL);
		glCompileShader(tShader);

		glGetShaderiv(tShader, GL_COMPILE_STATUS, &tResult);
		glGetShaderiv(tShader, GL_INFO_LOG_LENGTH, &tInfoLogLength);

		if (tInfoLogLength > 1)
		{
			char* log = new char[tInfoLogLength + 1];
			glGetShaderInfoLog(tShader, tInfoLogLength, NULL, log);
			nTString tDesc = natUtil::FormatString(_T("Compile Error:\r\n%s"), natUtil::C2Wstr(log).c_str());
			delete[] log;

			if (tResult == GL_FALSE)
			{
				throw natException(_T("n2dShaderWrapperImpl::CompileProgram"), tDesc);
			}
			
			MessageBox (NULL, _T("Warning"), (nTString(_T("In n2dShaderWrapperImpl::CompileShader: ")) + tDesc).c_str(), MB_OK);
		}
	}

	return tShader;
}

GLuint n2dShaderWrapperImpl::CompileProgram(ncTStr lpOutput)
{
	std::vector<GLuint> shadervec(m_ShaderData.size());
	GLuint ProgramID = glCreateProgram();

	GLint result;
	int InfoLogLength;

	for (nuInt i = 0u; i < 2u; ++i)
	{
		ShaderType shaderType = i == 0 ? Vertex : Fragment;

		for each (auto p in m_ShaderData[shaderType])
		{
			if (p.second.empty())
			{
				continue;
			}

			GLuint shader = CompileShader(p.second.data(), p.second.size(), shaderType, p.first);

			glAttachShader(ProgramID, shader);
			shadervec.push_back(shader);
		}
	}

	glProgramParameteri(ProgramID, GL_PROGRAM_SEPARABLE, GL_TRUE);

	glLinkProgram(ProgramID);
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 1)
	{
		char* log = new char[InfoLogLength + 1];
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, log);
		nTString tDesc = natUtil::FormatString(_T("Link Error:\r\n%s"), natUtil::C2Wstr(log).c_str());
		delete[] log;

		if (result == GL_FALSE)
		{
			throw natException(_T("n2dShaderWrapperImpl::CompileProgram"), tDesc);
		}
		else
		{
			MessageBox(NULL, _T("Warning"), (nTString(_T("In n2dShaderWrapperImpl::CompileProgram: ")) + tDesc).c_str(), MB_OK);
		}
	}

	glValidateProgram(ProgramID);
	glGetProgramiv(ProgramID, GL_VALIDATE_STATUS, &result);

	if (result != GL_TRUE)
	{
		throw natException(_T("n2dShaderWrapperImpl::CompileProgram"), _T("Program validating failed"));
	}

	for each (auto shader in shadervec)
	{
		glDetachShader(ProgramID, shader);
		glDeleteShader(shader);
	}

	m_ProgramID.push_back(ProgramID);
	m_ShaderData.clear();
	shadervec.clear();

	if (lpOutput)
	{
		GLsizei tSize, tSize2;
		GLenum tEnum;
		glGetProgramiv(ProgramID, GL_PROGRAM_BINARY_LENGTH, &tSize);
		if (tSize == 0)
		{
			return ProgramID;
		}
		nData pBuf = new nByte[tSize];
		glGetProgramBinary(ProgramID, tSize, &tSize2, &tEnum, pBuf);
		if (tSize != tSize2)
		{
			return ProgramID;
		}
		natFileStream* pStream = new natFileStream(lpOutput, true);
		pStream->WriteBytes(reinterpret_cast<nData>(&tEnum), sizeof(GLenum));
		pStream->WriteBytes(pBuf, tSize);
		pStream->Release();
	}

	return ProgramID;
}

GLuint n2dShaderWrapperImpl::LoadProgram(natStream* pStream)
{
	GLuint ProgramID = glCreateProgram();

	nLen tLen = pStream->GetSize() - pStream->GetPosition();
	nData pBuf = new nByte[static_cast<nuInt>(tLen - sizeof(GLenum))];
	GLenum tEnum;

	pStream->ReadBytes(reinterpret_cast<nData>(&tEnum), sizeof(GLenum));
	pStream->ReadBytes(pBuf, tLen - sizeof(GLenum));
	glProgramBinary(ProgramID, tEnum, pBuf, static_cast<GLsizei>(tLen));
	glLinkProgram(ProgramID);
	GLint result, InfoLogLength;
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 1)
	{
		char* log = new char[InfoLogLength + 1];
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, log);
		nTString tDesc = natUtil::FormatString(_T("Link Error:\r\n%s"), natUtil::C2Wstr(log).c_str());
		delete[] log;

		if (result == GL_FALSE)
		{
			throw natException(_T("n2dShaderWrapperImpl::CompileProgram"), tDesc);
		}
		else
		{
			MessageBox(NULL, _T("Warning"), (nTString(_T("In n2dShaderWrapperImpl::CompileProgram: ")) + tDesc).c_str(), MB_OK);
		}
	}
	SafeDelArr(pBuf);

	return ProgramID;
}

GLuint n2dShaderWrapperImpl::GenPipelineID()
{
	GLuint tRet = 0u;
	glGenProgramPipelines(1, &tRet);
	m_PipelineID.push_back(tRet);

	return tRet;
}