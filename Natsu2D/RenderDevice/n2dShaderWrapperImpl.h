////////////////////////////////////////////////////////////////////////////////
///	@file	n2dShaderWrapperImpl.h
///	@brief	Shader包装类
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\n2dShaderWrapper.h"

#include <vector>
#include <map>
#include <Windows.h>

struct natStream;

////////////////////////////////////////////////////////////////////////////////
///	@brief	Shader包装类
////////////////////////////////////////////////////////////////////////////////
class n2dShaderWrapperImpl final
	: public natRefObjImpl<n2dShaderWrapper>
{
public:
	n2dShaderWrapperImpl() = default;
	~n2dShaderWrapperImpl();

	///	@brief	添加着色器
	void AddShader(ShaderType shaderType, ncTStr filename, nBool bIsBinary = false) override;
	///	@brief	添加着色器
	void AddShader(ShaderType shaderType, natStream* pStream, nBool bIsBinary = false) override;
	GLuint CompileShader(ncData lpData, nLen lLen, ShaderType shaderType, nBool bIsBinary = false) override;
	///	@brief		编译着色器程序
	///	@warning	请勿手动删除程序
	GLuint CompileProgram(ncTStr lpOutput = nullptr) override;

	GLuint LoadProgram(natStream* pStream) override;

	GLuint GenPipelineID() override;
private:
	///	@brief	nBool表示是否为二进制文件
	std::map<ShaderType, std::vector<std::pair<nBool, std::vector<nByte>>>> m_ShaderData;
	std::vector<std::vector<nByte>> m_ProgramData;
	std::vector<GLuint> m_ProgramID;
	std::vector<GLuint> m_PipelineID;
};