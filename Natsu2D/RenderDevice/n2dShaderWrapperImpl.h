////////////////////////////////////////////////////////////////////////////////
///	@file	n2dShaderWrapperImpl.h
///	@brief	Shader包装类
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../n2dRenderDevice.h"
#include "OpenGL.h"

#include <unordered_map>
#include <unordered_set>

namespace NatsuLib
{
	struct natStream;
}

class n2dShaderImpl
	: public natRefObjImpl<n2dShader>
{
	static GLenum GetShaderType(ShaderType shaderType)
	{
		switch (shaderType)
		{
		case ShaderType::Vertex:
			return GL_VERTEX_SHADER;
		case ShaderType::Fragment:
			return GL_FRAGMENT_SHADER;
		case ShaderType::Geometry:
			return GL_GEOMETRY_SHADER;
		}

		nat_Throw(natException, _T("Not an available ShaderType"));
	}

public:
	explicit n2dShaderImpl(ShaderType shaderType);
	n2dShaderImpl(ShaderType shaderType, GLhandle shader);
	~n2dShaderImpl();

	GLhandle GetHandle() const override;
	ShaderType GetType() const override;
	nBool Deleted() const override;
	nBool Compiled() const override;
	ncTStr GetInfoLog() const override;

	void CompileFromStream(natStream* pStream, nBool bIsBinary = false);
private:
	ShaderType m_ShaderType;
	GLhandle m_Shader;

	mutable nTString m_Log;
};

class n2dShaderProgramImpl
	: public natRefObjImpl<n2dShaderProgram>
{
	GLint GetParameter(GLenum pname) const
	{
		GLint tResult = 0;
		glGetProgramiv(m_Program, pname, &tResult);

		return tResult;
	}

	static GLenum GetAttribTypeEnum(AttribType type)
	{
		switch (type)
		{
		case n2dShaderProgram::AttribType::Byte:
			return GL_BYTE;
		case n2dShaderProgram::AttribType::UnsignedByte:
			return GL_UNSIGNED_BYTE;
		case n2dShaderProgram::AttribType::Short:
			return GL_SHORT;
		case n2dShaderProgram::AttribType::UnsignedShort:
			return GL_UNSIGNED_SHORT;
		case n2dShaderProgram::AttribType::Int:
			return GL_INT;
		case n2dShaderProgram::AttribType::UnsignedInt:
			return GL_UNSIGNED_INT;
		case n2dShaderProgram::AttribType::HalfFloat:
			return GL_HALF_FLOAT;
		case n2dShaderProgram::AttribType::Float:
			return GL_FLOAT;
		case n2dShaderProgram::AttribType::Fixed:
			return GL_FIXED;
		case n2dShaderProgram::AttribType::Int_2_10_10_10_Rev:
			return GL_INT_2_10_10_10_REV;
		case n2dShaderProgram::AttribType::UnsignedInt_2_10_10_10_Rev:
			return GL_UNSIGNED_INT_2_10_10_10_REV;
		case n2dShaderProgram::AttribType::UnsignedInt_10F_11F_11F_Rev:
			return GL_UNSIGNED_INT_10F_11F_11F_REV;
		case n2dShaderProgram::AttribType::Double:
			return GL_DOUBLE;
		default:
			nat_Throw(natException, _T("Not a valid AttribType enum"));
		}
	}

	static GLenum GetTypeEnum(VarType type)
	{
		switch (type)
		{
		case n2dShaderProgram::VarType::Float:
			return GL_FLOAT;
		case n2dShaderProgram::VarType::vec2:
			return GL_FLOAT_VEC2;
		case n2dShaderProgram::VarType::vec3:
			return GL_FLOAT_VEC3;
		case n2dShaderProgram::VarType::vec4:
			return GL_FLOAT_VEC4;
		case n2dShaderProgram::VarType::Double:
			return GL_DOUBLE;
		case n2dShaderProgram::VarType::dvec2:
			return GL_DOUBLE_VEC2;
		case n2dShaderProgram::VarType::dvec3:
			return GL_DOUBLE_VEC3;
		case n2dShaderProgram::VarType::dvec4:
			return GL_DOUBLE_VEC4;;
		case n2dShaderProgram::VarType::Int:
			return GL_INT;
		case n2dShaderProgram::VarType::ivec2:
			return GL_INT_VEC2;
		case n2dShaderProgram::VarType::ivec3:
			return GL_INT_VEC3;
		case n2dShaderProgram::VarType::ivec4:
			return GL_INT_VEC4;
		case n2dShaderProgram::VarType::Uint:
			return GL_UNSIGNED_INT;
		case n2dShaderProgram::VarType::uvec2:
			return GL_UNSIGNED_INT_VEC2;
		case n2dShaderProgram::VarType::uvec3:
			return GL_UNSIGNED_INT_VEC3;
		case n2dShaderProgram::VarType::uvec4:
			return GL_UNSIGNED_INT_VEC4;
		case n2dShaderProgram::VarType::Bool:
			return GL_BOOL;
		case n2dShaderProgram::VarType::bvec2:
			return GL_BOOL_VEC2;
		case n2dShaderProgram::VarType::bvec3:
			return GL_BOOL_VEC3;
		case n2dShaderProgram::VarType::bvec4:
			return GL_BOOL_VEC4;
		case n2dShaderProgram::VarType::mat2:
			return GL_FLOAT_MAT2;
		case n2dShaderProgram::VarType::mat3:
			return GL_FLOAT_MAT3;
		case n2dShaderProgram::VarType::mat4:
			return GL_FLOAT_MAT4;
		case n2dShaderProgram::VarType::mat2x3:
			return GL_FLOAT_MAT2x3;
		case n2dShaderProgram::VarType::mat2x4:
			return GL_FLOAT_MAT2x4;
		case n2dShaderProgram::VarType::mat3x2:
			return GL_FLOAT_MAT3x2;
		case n2dShaderProgram::VarType::mat3x4:
			return GL_FLOAT_MAT3x4;
		case n2dShaderProgram::VarType::mat4x2:
			return GL_FLOAT_MAT4x2;
		case n2dShaderProgram::VarType::mat4x3:
			return GL_FLOAT_MAT4x3;
		case n2dShaderProgram::VarType::dmat2:
			return GL_DOUBLE_MAT2;
		case n2dShaderProgram::VarType::dmat3:
			return GL_DOUBLE_MAT3;
		case n2dShaderProgram::VarType::dmat4:
			return GL_DOUBLE_MAT4;
		case n2dShaderProgram::VarType::dmat2x3:
			return GL_DOUBLE_MAT2x3;
		case n2dShaderProgram::VarType::dmat2x4:
			return GL_DOUBLE_MAT2x4;
		case n2dShaderProgram::VarType::dmat3x2:
			return GL_DOUBLE_MAT3x2;
		case n2dShaderProgram::VarType::dmat3x4:
			return GL_DOUBLE_MAT3x4;
		case n2dShaderProgram::VarType::dmat4x2:
			return GL_DOUBLE_MAT4x2;
		case n2dShaderProgram::VarType::dmat4x3:
			return GL_DOUBLE_MAT4x3;
		case n2dShaderProgram::VarType::sampler1D:
			return GL_SAMPLER_1D;
		case n2dShaderProgram::VarType::sampler2D:
			return GL_SAMPLER_2D;
		case n2dShaderProgram::VarType::sampler3D:
			return GL_SAMPLER_3D;
		case n2dShaderProgram::VarType::samplerCube:
			return GL_SAMPLER_CUBE;
		case n2dShaderProgram::VarType::sampler1DShadow:
			return GL_SAMPLER_1D_SHADOW;
		case n2dShaderProgram::VarType::sampler2DShadow:
			return GL_SAMPLER_2D_SHADOW;
		case n2dShaderProgram::VarType::sampler1DArray:
			return GL_SAMPLER_1D_ARRAY;
		case n2dShaderProgram::VarType::sampler2DArray:
			return GL_SAMPLER_2D_ARRAY;
		case n2dShaderProgram::VarType::sampler1DArrayShadow:
			return GL_SAMPLER_1D_ARRAY_SHADOW;
		case n2dShaderProgram::VarType::sampler2DArrayShadow:
			return GL_SAMPLER_2D_ARRAY_SHADOW;
		case n2dShaderProgram::VarType::sampler2DMS:
			return GL_SAMPLER_2D_MULTISAMPLE;
		case n2dShaderProgram::VarType::sampler2DMSArray:
			return GL_SAMPLER_2D_MULTISAMPLE_ARRAY;
		case n2dShaderProgram::VarType::samplerCubeShadow:
			return GL_SAMPLER_CUBE_SHADOW;
		case n2dShaderProgram::VarType::samplerBuffer:
			return GL_SAMPLER_BUFFER;
		case n2dShaderProgram::VarType::sampler2DRect:
			return GL_SAMPLER_2D_RECT;
		case n2dShaderProgram::VarType::sampler2DRectShadow:
			return GL_SAMPLER_2D_RECT_SHADOW;
		case n2dShaderProgram::VarType::isampler1D:
			return GL_INT_SAMPLER_1D;
		case n2dShaderProgram::VarType::isampler2D:
			return GL_INT_SAMPLER_2D;
		case n2dShaderProgram::VarType::isampler3D:
			return GL_INT_SAMPLER_3D;
		case n2dShaderProgram::VarType::isamplerCube:
			return GL_INT_SAMPLER_CUBE;
		case n2dShaderProgram::VarType::isampler1DArray:
			return GL_INT_SAMPLER_1D_ARRAY;
		case n2dShaderProgram::VarType::isampler2DArray:
			return GL_INT_SAMPLER_2D_ARRAY;
		case n2dShaderProgram::VarType::isampler2DMS:
			return GL_INT_SAMPLER_2D_MULTISAMPLE;
		case n2dShaderProgram::VarType::isampler2DMSArray:
			return GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
		case n2dShaderProgram::VarType::isamplerBuffer:
			return GL_INT_SAMPLER_BUFFER;
		case n2dShaderProgram::VarType::isampler2DRect:
			return GL_INT_SAMPLER_2D_RECT;
		case n2dShaderProgram::VarType::usampler1D:
			return GL_UNSIGNED_INT_SAMPLER_1D;
		case n2dShaderProgram::VarType::usampler2D:
			return GL_UNSIGNED_INT_SAMPLER_2D;
		case n2dShaderProgram::VarType::usampler3D:
			return GL_UNSIGNED_INT_SAMPLER_3D;
		case n2dShaderProgram::VarType::usamplerCube:
			return GL_UNSIGNED_INT_SAMPLER_CUBE;
		case n2dShaderProgram::VarType::usampler2DArray:
			return GL_UNSIGNED_INT_SAMPLER_2D_ARRAY;
		case n2dShaderProgram::VarType::usampler2DMS:
			return GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE;
		case n2dShaderProgram::VarType::usampler2DMSArray:
			return GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
		case n2dShaderProgram::VarType::usamplerBuffer:
			return GL_UNSIGNED_INT_SAMPLER_BUFFER;
		case n2dShaderProgram::VarType::usampler2DRect:
			return GL_UNSIGNED_INT_SAMPLER_2D_RECT;
		case n2dShaderProgram::VarType::image1D:
			return GL_IMAGE_1D;
		case n2dShaderProgram::VarType::image2D:
			return GL_IMAGE_2D;
		case n2dShaderProgram::VarType::image3D:
			return GL_IMAGE_3D;
		case n2dShaderProgram::VarType::image2DRect:
			return GL_IMAGE_2D_RECT;
		case n2dShaderProgram::VarType::imageCube:
			return GL_IMAGE_CUBE;
		case n2dShaderProgram::VarType::imageBuffer:
			return GL_IMAGE_BUFFER;
		case n2dShaderProgram::VarType::image1DArray:
			return GL_IMAGE_1D_ARRAY;
		case n2dShaderProgram::VarType::image2DArray:
			return GL_IMAGE_2D_ARRAY;
		case n2dShaderProgram::VarType::image2DMS:
			return GL_IMAGE_2D_MULTISAMPLE;
		case n2dShaderProgram::VarType::image2DMSArray:
			return GL_IMAGE_2D_MULTISAMPLE_ARRAY;
		case n2dShaderProgram::VarType::iimage1D:
			return GL_INT_IMAGE_1D;
		case n2dShaderProgram::VarType::iimage2D:
			return GL_INT_IMAGE_2D;
		case n2dShaderProgram::VarType::iimage3D:
			return GL_INT_IMAGE_3D;
		case n2dShaderProgram::VarType::iimage2DRect:
			return GL_INT_IMAGE_2D_RECT;
		case n2dShaderProgram::VarType::iimageCube:
			return GL_INT_IMAGE_CUBE;
		case n2dShaderProgram::VarType::iimageBuffer:
			return GL_INT_IMAGE_BUFFER;
		case n2dShaderProgram::VarType::iimage1DArray:
			return GL_INT_IMAGE_1D_ARRAY;
		case n2dShaderProgram::VarType::iimage2DArray:
			return GL_INT_IMAGE_2D_ARRAY;
		case n2dShaderProgram::VarType::iimage2DMS:
			return GL_INT_IMAGE_2D_MULTISAMPLE;
		case n2dShaderProgram::VarType::iimage2DMSArray:
			return GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY;
		case n2dShaderProgram::VarType::uimage1D:
			return GL_UNSIGNED_INT_IMAGE_1D;
		case n2dShaderProgram::VarType::uimage2D:
			return GL_UNSIGNED_INT_IMAGE_2D;
		case n2dShaderProgram::VarType::uimage3D:
			return GL_UNSIGNED_INT_IMAGE_3D;
		case n2dShaderProgram::VarType::uimage2DRect:
			return GL_UNSIGNED_INT_IMAGE_2D_RECT;
		case n2dShaderProgram::VarType::uimageCube:
			return GL_UNSIGNED_INT_IMAGE_CUBE;
		case n2dShaderProgram::VarType::uimageBuffer:
			return GL_UNSIGNED_INT_IMAGE_BUFFER;
		case n2dShaderProgram::VarType::uimage1DArray:
			return GL_UNSIGNED_INT_IMAGE_1D_ARRAY;
		case n2dShaderProgram::VarType::uimage2DArray:
			return GL_UNSIGNED_INT_IMAGE_2D_ARRAY;
		case n2dShaderProgram::VarType::uimage2DMS:
			return GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE;
		case n2dShaderProgram::VarType::uimage2DMSArray:
			return GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY;
		case n2dShaderProgram::VarType::atomic_uint:
			return GL_UNSIGNED_INT_ATOMIC_COUNTER;
		default:
			nat_Throw(natException, _T("Not a valid VarType"));
		}
	}

	static VarType GetVarType(GLenum type)
	{
		switch (type)
		{
		case GL_FLOAT:
			return n2dShaderProgram::VarType::Float;
		case GL_FLOAT_VEC2:
			return n2dShaderProgram::VarType::vec2;
		case GL_FLOAT_VEC3:
			return n2dShaderProgram::VarType::vec3;
		case GL_FLOAT_VEC4:
			return n2dShaderProgram::VarType::vec4;
		case GL_DOUBLE:
			return n2dShaderProgram::VarType::Double;
		case GL_DOUBLE_VEC2:
			return n2dShaderProgram::VarType::dvec2;
		case GL_DOUBLE_VEC3:
			return n2dShaderProgram::VarType::dvec3;
		case GL_DOUBLE_VEC4:
			return n2dShaderProgram::VarType::dvec4;
		case GL_INT:
			return n2dShaderProgram::VarType::Int;
		case GL_INT_VEC2:
			return n2dShaderProgram::VarType::ivec2;
		case GL_INT_VEC3:
			return n2dShaderProgram::VarType::ivec3;
		case GL_INT_VEC4:
			return n2dShaderProgram::VarType::ivec4;
		case GL_UNSIGNED_INT:
			return n2dShaderProgram::VarType::Uint;
		case GL_UNSIGNED_INT_VEC2:
			return n2dShaderProgram::VarType::uvec2;
		case GL_UNSIGNED_INT_VEC3:
			return n2dShaderProgram::VarType::uvec3;
		case GL_UNSIGNED_INT_VEC4:
			return n2dShaderProgram::VarType::uvec4;
		case GL_BOOL:
			return n2dShaderProgram::VarType::Bool;
		case GL_BOOL_VEC2:
			return n2dShaderProgram::VarType::bvec2;
		case GL_BOOL_VEC3:
			return n2dShaderProgram::VarType::bvec3;
		case GL_BOOL_VEC4:
			return n2dShaderProgram::VarType::bvec4;
		case GL_FLOAT_MAT2:
			return n2dShaderProgram::VarType::mat2;
		case GL_FLOAT_MAT3:
			return n2dShaderProgram::VarType::mat3;
		case GL_FLOAT_MAT4:
			return n2dShaderProgram::VarType::mat4;
		case GL_FLOAT_MAT2x3:
			return n2dShaderProgram::VarType::mat2x3;
		case GL_FLOAT_MAT2x4:
			return n2dShaderProgram::VarType::mat2x4;
		case GL_FLOAT_MAT3x2:
			return n2dShaderProgram::VarType::mat3x2;
		case GL_FLOAT_MAT3x4:
			return n2dShaderProgram::VarType::mat3x4;
		case GL_FLOAT_MAT4x2:
			return n2dShaderProgram::VarType::mat4x2;
		case GL_FLOAT_MAT4x3:
			return n2dShaderProgram::VarType::mat4x3;
		case GL_DOUBLE_MAT2:
			return n2dShaderProgram::VarType::dmat2;
		case GL_DOUBLE_MAT3:
			return n2dShaderProgram::VarType::dmat3;
		case GL_DOUBLE_MAT4:
			return n2dShaderProgram::VarType::dmat4;
		case GL_DOUBLE_MAT2x3:
			return n2dShaderProgram::VarType::dmat2x3;
		case GL_DOUBLE_MAT2x4:
			return n2dShaderProgram::VarType::dmat2x4;
		case GL_DOUBLE_MAT3x2:
			return n2dShaderProgram::VarType::dmat3x2;
		case GL_DOUBLE_MAT3x4:
			return n2dShaderProgram::VarType::dmat3x4;
		case GL_DOUBLE_MAT4x2:
			return n2dShaderProgram::VarType::dmat4x2;
		case GL_DOUBLE_MAT4x3:
			return n2dShaderProgram::VarType::dmat4x3;
		case GL_SAMPLER_1D:
			return n2dShaderProgram::VarType::sampler1D;
		case GL_SAMPLER_2D:
			return n2dShaderProgram::VarType::sampler2D;
		case GL_SAMPLER_3D:
			return n2dShaderProgram::VarType::sampler3D;
		case GL_SAMPLER_CUBE:
			return n2dShaderProgram::VarType::samplerCube;
		case GL_SAMPLER_1D_SHADOW:
			return n2dShaderProgram::VarType::sampler1DShadow;
		case GL_SAMPLER_2D_SHADOW:
			return n2dShaderProgram::VarType::sampler2DShadow;
		case GL_SAMPLER_1D_ARRAY:
			return n2dShaderProgram::VarType::sampler1DArray;
		case GL_SAMPLER_2D_ARRAY:
			return n2dShaderProgram::VarType::sampler2DArray;
		case GL_SAMPLER_1D_ARRAY_SHADOW:
			return n2dShaderProgram::VarType::sampler1DArrayShadow;
		case GL_SAMPLER_2D_ARRAY_SHADOW:
			return n2dShaderProgram::VarType::sampler2DArrayShadow;
		case GL_SAMPLER_2D_MULTISAMPLE:
			return n2dShaderProgram::VarType::sampler2DMS;
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
			return n2dShaderProgram::VarType::sampler2DMSArray;
		case GL_SAMPLER_CUBE_SHADOW:
			return n2dShaderProgram::VarType::samplerCubeShadow;
		case GL_SAMPLER_BUFFER:
			return n2dShaderProgram::VarType::samplerBuffer;
		case GL_SAMPLER_2D_RECT:
			return n2dShaderProgram::VarType::sampler2DRect;
		case GL_SAMPLER_2D_RECT_SHADOW:
			return n2dShaderProgram::VarType::sampler2DRectShadow;
		case GL_INT_SAMPLER_1D:
			return n2dShaderProgram::VarType::isampler1D;
		case GL_INT_SAMPLER_2D:
			return n2dShaderProgram::VarType::isampler2D;
		case GL_INT_SAMPLER_3D:
			return n2dShaderProgram::VarType::isampler3D;
		case GL_INT_SAMPLER_CUBE:
			return n2dShaderProgram::VarType::isamplerCube;
		case GL_INT_SAMPLER_1D_ARRAY:
			return n2dShaderProgram::VarType::isampler1DArray;
		case GL_INT_SAMPLER_2D_ARRAY:
			return n2dShaderProgram::VarType::isampler2DArray;
		case GL_INT_SAMPLER_2D_MULTISAMPLE:
			return n2dShaderProgram::VarType::isampler2DMS;
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
			return n2dShaderProgram::VarType::isampler2DMSArray;
		case GL_INT_SAMPLER_BUFFER:
			return n2dShaderProgram::VarType::isamplerBuffer;
		case GL_INT_SAMPLER_2D_RECT:
			return n2dShaderProgram::VarType::isampler2DRect;
		case GL_UNSIGNED_INT_SAMPLER_1D:
			return n2dShaderProgram::VarType::usampler1D;
		case GL_UNSIGNED_INT_SAMPLER_2D:
			return n2dShaderProgram::VarType::usampler2D;
		case GL_UNSIGNED_INT_SAMPLER_3D:
			return n2dShaderProgram::VarType::usampler3D;
		case GL_UNSIGNED_INT_SAMPLER_CUBE:
			return n2dShaderProgram::VarType::usamplerCube;
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
			return n2dShaderProgram::VarType::usampler2DArray;
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
			return n2dShaderProgram::VarType::usampler2DMS;
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
			return n2dShaderProgram::VarType::usampler2DMSArray;
		case GL_UNSIGNED_INT_SAMPLER_BUFFER:
			return n2dShaderProgram::VarType::usamplerBuffer;
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
			return n2dShaderProgram::VarType::usampler2DRect;
		case GL_IMAGE_1D:
			return n2dShaderProgram::VarType::image1D;
		case GL_IMAGE_2D:
			return n2dShaderProgram::VarType::image2D;
		case GL_IMAGE_3D:
			return n2dShaderProgram::VarType::image3D;
		case GL_IMAGE_2D_RECT:
			return n2dShaderProgram::VarType::image2DRect;
		case GL_IMAGE_CUBE:
			return n2dShaderProgram::VarType::imageCube;
		case GL_IMAGE_BUFFER:
			return n2dShaderProgram::VarType::imageBuffer;
		case GL_IMAGE_1D_ARRAY:
			return n2dShaderProgram::VarType::image1DArray;
		case GL_IMAGE_2D_ARRAY:
			return n2dShaderProgram::VarType::image2DArray;
		case GL_IMAGE_2D_MULTISAMPLE:
			return n2dShaderProgram::VarType::image2DMS;
		case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
			return n2dShaderProgram::VarType::image2DMSArray;
		case GL_INT_IMAGE_1D:
			return n2dShaderProgram::VarType::iimage1D;
		case GL_INT_IMAGE_2D:
			return n2dShaderProgram::VarType::iimage2D;
		case GL_INT_IMAGE_3D:
			return n2dShaderProgram::VarType::iimage3D;
		case GL_INT_IMAGE_2D_RECT:
			return n2dShaderProgram::VarType::iimage2DRect;
		case GL_INT_IMAGE_CUBE:
			return n2dShaderProgram::VarType::iimageCube;
		case GL_INT_IMAGE_BUFFER:
			return n2dShaderProgram::VarType::iimageBuffer;
		case GL_INT_IMAGE_1D_ARRAY:
			return n2dShaderProgram::VarType::iimage1DArray;
		case GL_INT_IMAGE_2D_ARRAY:
			return n2dShaderProgram::VarType::iimage2DArray;
		case GL_INT_IMAGE_2D_MULTISAMPLE:
			return n2dShaderProgram::VarType::iimage2DMS;
		case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
			return n2dShaderProgram::VarType::iimage2DMSArray;
		case GL_UNSIGNED_INT_IMAGE_1D:
			return n2dShaderProgram::VarType::uimage1D;
		case GL_UNSIGNED_INT_IMAGE_2D:
			return n2dShaderProgram::VarType::uimage2D;
		case GL_UNSIGNED_INT_IMAGE_3D:
			return n2dShaderProgram::VarType::uimage3D;
		case GL_UNSIGNED_INT_IMAGE_2D_RECT:
			return n2dShaderProgram::VarType::uimage2DRect;
		case GL_UNSIGNED_INT_IMAGE_CUBE:
			return n2dShaderProgram::VarType::uimageCube;
		case GL_UNSIGNED_INT_IMAGE_BUFFER:
			return n2dShaderProgram::VarType::uimageBuffer;
		case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
			return n2dShaderProgram::VarType::uimage1DArray;
		case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
			return n2dShaderProgram::VarType::uimage2DArray;
		case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
			return n2dShaderProgram::VarType::uimage2DMS;
		case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
			return n2dShaderProgram::VarType::uimage2DMSArray;
		case GL_UNSIGNED_INT_ATOMIC_COUNTER:
			return n2dShaderProgram::VarType::atomic_uint;
		default:
			nat_Throw(natException, _T("Not a valid type enum"));
		}
	}
public:
	class AttributeReferenceImpl
		: public natRefObjImpl<AttributeReference>
	{
		friend class n2dShaderProgramImpl;
	public:
		AttributeReferenceImpl(n2dShaderProgramImpl* pProgram, GLhandle Location);

		n2dShaderProgram* GetProgram() const override;

		nuInt GetSize() const override;
		nuInt GetInternalSize() const override;
		VarType GetType() const override;

		void EnablePointer(nBool Value) const override;
		nBool isEnable() const override;

		const void* GetPointer() const override;
		nResult SetPointer(nuInt Size, AttribType Type, nBool Normalized, nuInt Stride, const void* pPointer) override;

		nResult SetStaticValue(nBool Normalized, nBool Explicit, const void* pValue) override;

	private:
		void onProgramDestroy();

		n2dShaderProgramImpl* m_pProgram;
		GLhandle m_Location;
		GLenum m_Type, m_ExType;
		GLint m_Size, m_RealSize;
	};

	class UniformReferenceImpl
		: public natRefObjImpl<UniformReference>
	{
		friend class n2dShaderProgramImpl;
	public:
		UniformReferenceImpl(n2dShaderProgramImpl* pProgram, GLhandle Location);
		n2dShaderProgram* GetProgram() const override;

		VarType GetType() const override;
		nuInt GetSize() const override;
		void GetValue(nuInt Size, void* Value) const override;
		void SetValue(nuInt Count, const void* Value, nBool Transpose) override;

	private:
		void onProgramDestroy();

		n2dShaderProgramImpl* m_pProgram;
		GLhandle m_Location;
		GLenum m_Type;
		GLint m_Size;
	};

	class UniformBlockReferenceImpl
		: public natRefObjImpl<UniformBlockReference>
	{
		friend class n2dShaderProgramImpl;
	public:
		UniformBlockReferenceImpl(n2dShaderProgramImpl* pProgram, GLhandle Location);

		n2dShaderProgram* GetProgram() const override;

		nuInt GetSize() const override;
		nuInt ActiveUniformCount() const override;

		void Bind(n2dBuffer* pBuffer) override;
		nuInt GetBindingPoint() const override;

	private:
		void onProgramDestroy();

		n2dShaderProgramImpl* m_pProgram;
		GLhandle m_Location;
		GLint m_Size, m_ActiveUniformCount;
	};

	explicit n2dShaderProgramImpl(GLhandle ProgramID = 0u);
	n2dShaderProgramImpl(n2dShaderImpl* pVertexShader, n2dShaderImpl* pFragmentShader, n2dShaderImpl* pGeometryShader);
	~n2dShaderProgramImpl();

	GLhandle GetHandle() const override;

	void AttachShader(n2dShader* pShader) override;
	void DetachShader(n2dShader* pShader) override;

	nuInt AttachedShaderCount() const override;

	void SetBinaryRetrievable(nBool Value) override;
	void SetSeparable(nBool Value) override;

	void Link() override;
	nBool IsLinked() const override;
	nBool Validate() const override;
	ncTStr GetInfoLog() const override;
	void Use() const override;
	nBool IsUsing() const override;

	nResult OutputBinary(natStream* pStream) const override;

	nuInt ActiveAttributeCount() const override;
	nuInt ActiveUniformCount() const override;
	nuInt ActiveUniformBlockCount() const override;

	AttributeReference* GetAttributeReference(nuInt Location) override;
	AttributeReference* GetAttributeReference(ncTStr Name) override;

	UniformReference* GetUniformReference(nuInt Location) override;
	UniformReference* GetUniformReference(ncTStr Name) override;

	UniformBlockReference* GetUniformBlockReference(nuInt Location) override;
	UniformBlockReference* GetUniformBlockReference(ncTStr Name) override;

	static n2dShaderProgramImpl* CreateFromStream(natStream* pStream);
private:
	GLhandle m_Program;

	std::unordered_map<GLuint, natRefPointer<AttributeReferenceImpl>> m_AttribMap;
	std::unordered_map<GLuint, natRefPointer<UniformReferenceImpl>> m_UniformMap;
	std::unordered_map<GLuint, natRefPointer<UniformBlockReferenceImpl>> m_UniformBlockMap;

	mutable nTString m_Log;
};

class n2dProgramPipelineImpl
	: public natRefObjImpl<n2dProgramPipeline>
{
public:
	explicit n2dProgramPipelineImpl(GLhandle PipelineID = 0u);
	~n2dProgramPipelineImpl();

	GLhandle GetHandle() const override;
	void Bind() const override;
	nBool IsBinding() const override;

	void UseProgramStages(nuInt stages, n2dShaderProgram* pProgram) override;
private:
	GLhandle m_Pipeline;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	Shader包装类
////////////////////////////////////////////////////////////////////////////////
class n2dShaderWrapperImpl final
	: public natRefObjImpl<n2dShaderWrapper>
{
	friend class n2dEngineImpl;
public:
	explicit n2dShaderWrapperImpl(n2dRenderDevice* pRenderDevice);
	~n2dShaderWrapperImpl();

	nResult CreateShaderFromStream(natStream* pStream, n2dShader::ShaderType shaderType, nBool bIsBinary, n2dShader** pOut) override;
	nResult CreateProgram(n2dShaderProgram** pOut) override;
	nResult CreateProgramFromStream(natStream* pStream, n2dShaderProgram** pOut) override;
	nResult CreateProgramPipeline(n2dProgramPipeline** pOut) override;

	n2dShaderProgram* GetCurrentProgram() override;
	n2dProgramPipeline* GetCurrentProgramPipeline() override;

	n2dShaderProgramImpl* SetDefaultProgram(n2dShaderProgramImpl* pProgram);
	n2dShaderProgram* GetDefaultProgram() const override;

	GLuint GetAvailableBindPoint();
	void UseBindingPoint(GLuint BindingPoint);
	void ReleaseBindingPoint(GLuint BindingPoint);
private:
	std::unordered_set<nuInt> m_AvailableBindingPoint;
	std::unordered_map<GLhandle, natRefPointer<n2dShaderProgramImpl>> m_Programs;
	std::unordered_map<GLhandle, natRefPointer<n2dProgramPipelineImpl>> m_ProgramPipelines;

	n2dRenderDevice* m_pRenderDevice;
	natRefPointer<n2dShaderProgramImpl> m_DefaultProgram;

	nuInt m_MaxBindingPoint;
};