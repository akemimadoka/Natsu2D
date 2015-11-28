#pragma once
#include "n2dInterface.h"
#include "n2dCommon.h"

struct natStream;

struct n2dShaderWrapper
	: n2dInterface
{
	///	@brief	��ɫ������
	enum ShaderType
	{
		Vertex = GL_VERTEX_SHADER,		///< @brief	������ɫ��
		Fragment = GL_FRAGMENT_SHADER		///< @brief	Ƭ����ɫ��
	};

	///	@brief	�����ɫ��
	virtual void AddShader(ShaderType shaderType, ncTStr filename, nBool bIsBinary = false) = 0;
	///	@brief	�����ɫ��
	virtual void AddShader(ShaderType shaderType, natStream* pStream, nBool bIsBinary = false) = 0;
	virtual GLuint CompileShader(ncData lpData, nLen lLen, ShaderType shaderType, nBool bIsBinary = false) = 0;
	///	@brief		������ɫ������
	///	@warning	�����ֶ�ɾ������
	virtual GLuint CompileProgram(ncTStr lpOutput = nullptr) = 0;

	virtual GLuint LoadProgram(natStream* pStream) = 0;

	virtual GLuint GenPipelineID() = 0;
};