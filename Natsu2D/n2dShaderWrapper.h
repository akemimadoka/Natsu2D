#pragma once
#include "n2dInterface.h"
#include "n2dCommon.h"

struct natStream;

struct n2dShaderWrapper
	: n2dInterface
{
	///	@brief	着色器类型
	enum ShaderType
	{
		Vertex = GL_VERTEX_SHADER,		///< @brief	顶点着色器
		Fragment = GL_FRAGMENT_SHADER		///< @brief	片段着色器
	};

	///	@brief	添加着色器
	virtual void AddShader(ShaderType shaderType, ncTStr filename, nBool bIsBinary = false) = 0;
	///	@brief	添加着色器
	virtual void AddShader(ShaderType shaderType, natStream* pStream, nBool bIsBinary = false) = 0;
	virtual GLuint CompileShader(ncData lpData, nLen lLen, ShaderType shaderType, nBool bIsBinary = false) = 0;
	///	@brief		编译着色器程序
	///	@warning	请勿手动删除程序
	virtual GLuint CompileProgram(ncTStr lpOutput = nullptr) = 0;

	virtual GLuint LoadProgram(natStream* pStream) = 0;

	virtual GLuint GenPipelineID() = 0;
};