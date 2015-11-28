////////////////////////////////////////////////////////////////////////////////
///	@file	n2dShaderWrapperImpl.h
///	@brief	Shader��װ��
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\n2dShaderWrapper.h"

#include <vector>
#include <map>
#include <Windows.h>

struct natStream;

////////////////////////////////////////////////////////////////////////////////
///	@brief	Shader��װ��
////////////////////////////////////////////////////////////////////////////////
class n2dShaderWrapperImpl final
	: public natRefObjImpl<n2dShaderWrapper>
{
public:
	n2dShaderWrapperImpl() = default;
	~n2dShaderWrapperImpl();

	///	@brief	�����ɫ��
	void AddShader(ShaderType shaderType, ncTStr filename, nBool bIsBinary = false) override;
	///	@brief	�����ɫ��
	void AddShader(ShaderType shaderType, natStream* pStream, nBool bIsBinary = false) override;
	GLuint CompileShader(ncData lpData, nLen lLen, ShaderType shaderType, nBool bIsBinary = false) override;
	///	@brief		������ɫ������
	///	@warning	�����ֶ�ɾ������
	GLuint CompileProgram(ncTStr lpOutput = nullptr) override;

	GLuint LoadProgram(natStream* pStream) override;

	GLuint GenPipelineID() override;
private:
	///	@brief	nBool��ʾ�Ƿ�Ϊ�������ļ�
	std::map<ShaderType, std::vector<std::pair<nBool, std::vector<nByte>>>> m_ShaderData;
	std::vector<std::vector<nByte>> m_ProgramData;
	std::vector<GLuint> m_ProgramID;
	std::vector<GLuint> m_PipelineID;
};