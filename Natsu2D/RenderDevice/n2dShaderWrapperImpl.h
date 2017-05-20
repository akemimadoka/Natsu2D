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
	: public natRefObjImpl<n2dShaderImpl, n2dShader>
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
		default:
			nat_Throw(natException, "Not an available ShaderType"_nv);
		}
	}

public:
	explicit n2dShaderImpl(ShaderType shaderType);
	n2dShaderImpl(ShaderType shaderType, GLhandle shader);
	~n2dShaderImpl();

	HandleType GetHandle() const override;
	ShaderType GetType() const override;
	nBool Deleted() const override;
	nBool Compiled() const override;
	nStrView GetInfoLog() const override;

	void CompileFromStream(natRefPointer<natStream> pStream, nBool bIsBinary = false);
private:
	ShaderType m_ShaderType;
	GLhandle m_Shader;

	mutable nString m_Log;
};

class n2dShaderProgramImpl
	: public natRefObjImpl<n2dShaderProgramImpl, n2dShaderProgram>
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
		case AttribType::Byte:
			return GL_BYTE;
		case AttribType::UnsignedByte:
			return GL_UNSIGNED_BYTE;
		case AttribType::Short:
			return GL_SHORT;
		case AttribType::UnsignedShort:
			return GL_UNSIGNED_SHORT;
		case AttribType::Int:
			return GL_INT;
		case AttribType::UnsignedInt:
			return GL_UNSIGNED_INT;
		case AttribType::HalfFloat:
			return GL_HALF_FLOAT;
		case AttribType::Float:
			return GL_FLOAT;
		case AttribType::Fixed:
			return GL_FIXED;
		case AttribType::Int_2_10_10_10_Rev:
			return GL_INT_2_10_10_10_REV;
		case AttribType::UnsignedInt_2_10_10_10_Rev:
			return GL_UNSIGNED_INT_2_10_10_10_REV;
		case AttribType::UnsignedInt_10F_11F_11F_Rev:
			return GL_UNSIGNED_INT_10F_11F_11F_REV;
		case AttribType::Double:
			return GL_DOUBLE;
		default:
			nat_Throw(natException, "Not a valid AttribType enum"_nv);
		}
	}

	static GLenum GetTypeEnum(VarType type)
	{
		switch (type)
		{
		case VarType::Float:
			return GL_FLOAT;
		case VarType::vec2:
			return GL_FLOAT_VEC2;
		case VarType::vec3:
			return GL_FLOAT_VEC3;
		case VarType::vec4:
			return GL_FLOAT_VEC4;
		case VarType::Double:
			return GL_DOUBLE;
		case VarType::dvec2:
			return GL_DOUBLE_VEC2;
		case VarType::dvec3:
			return GL_DOUBLE_VEC3;
		case VarType::dvec4:
			return GL_DOUBLE_VEC4;;
		case VarType::Int:
			return GL_INT;
		case VarType::ivec2:
			return GL_INT_VEC2;
		case VarType::ivec3:
			return GL_INT_VEC3;
		case VarType::ivec4:
			return GL_INT_VEC4;
		case VarType::Uint:
			return GL_UNSIGNED_INT;
		case VarType::uvec2:
			return GL_UNSIGNED_INT_VEC2;
		case VarType::uvec3:
			return GL_UNSIGNED_INT_VEC3;
		case VarType::uvec4:
			return GL_UNSIGNED_INT_VEC4;
		case VarType::Bool:
			return GL_BOOL;
		case VarType::bvec2:
			return GL_BOOL_VEC2;
		case VarType::bvec3:
			return GL_BOOL_VEC3;
		case VarType::bvec4:
			return GL_BOOL_VEC4;
		case VarType::mat2:
			return GL_FLOAT_MAT2;
		case VarType::mat3:
			return GL_FLOAT_MAT3;
		case VarType::mat4:
			return GL_FLOAT_MAT4;
		case VarType::mat2x3:
			return GL_FLOAT_MAT2x3;
		case VarType::mat2x4:
			return GL_FLOAT_MAT2x4;
		case VarType::mat3x2:
			return GL_FLOAT_MAT3x2;
		case VarType::mat3x4:
			return GL_FLOAT_MAT3x4;
		case VarType::mat4x2:
			return GL_FLOAT_MAT4x2;
		case VarType::mat4x3:
			return GL_FLOAT_MAT4x3;
		case VarType::dmat2:
			return GL_DOUBLE_MAT2;
		case VarType::dmat3:
			return GL_DOUBLE_MAT3;
		case VarType::dmat4:
			return GL_DOUBLE_MAT4;
		case VarType::dmat2x3:
			return GL_DOUBLE_MAT2x3;
		case VarType::dmat2x4:
			return GL_DOUBLE_MAT2x4;
		case VarType::dmat3x2:
			return GL_DOUBLE_MAT3x2;
		case VarType::dmat3x4:
			return GL_DOUBLE_MAT3x4;
		case VarType::dmat4x2:
			return GL_DOUBLE_MAT4x2;
		case VarType::dmat4x3:
			return GL_DOUBLE_MAT4x3;
		case VarType::sampler1D:
			return GL_SAMPLER_1D;
		case VarType::sampler2D:
			return GL_SAMPLER_2D;
		case VarType::sampler3D:
			return GL_SAMPLER_3D;
		case VarType::samplerCube:
			return GL_SAMPLER_CUBE;
		case VarType::sampler1DShadow:
			return GL_SAMPLER_1D_SHADOW;
		case VarType::sampler2DShadow:
			return GL_SAMPLER_2D_SHADOW;
		case VarType::sampler1DArray:
			return GL_SAMPLER_1D_ARRAY;
		case VarType::sampler2DArray:
			return GL_SAMPLER_2D_ARRAY;
		case VarType::sampler1DArrayShadow:
			return GL_SAMPLER_1D_ARRAY_SHADOW;
		case VarType::sampler2DArrayShadow:
			return GL_SAMPLER_2D_ARRAY_SHADOW;
		case VarType::sampler2DMS:
			return GL_SAMPLER_2D_MULTISAMPLE;
		case VarType::sampler2DMSArray:
			return GL_SAMPLER_2D_MULTISAMPLE_ARRAY;
		case VarType::samplerCubeShadow:
			return GL_SAMPLER_CUBE_SHADOW;
		case VarType::samplerBuffer:
			return GL_SAMPLER_BUFFER;
		case VarType::sampler2DRect:
			return GL_SAMPLER_2D_RECT;
		case VarType::sampler2DRectShadow:
			return GL_SAMPLER_2D_RECT_SHADOW;
		case VarType::isampler1D:
			return GL_INT_SAMPLER_1D;
		case VarType::isampler2D:
			return GL_INT_SAMPLER_2D;
		case VarType::isampler3D:
			return GL_INT_SAMPLER_3D;
		case VarType::isamplerCube:
			return GL_INT_SAMPLER_CUBE;
		case VarType::isampler1DArray:
			return GL_INT_SAMPLER_1D_ARRAY;
		case VarType::isampler2DArray:
			return GL_INT_SAMPLER_2D_ARRAY;
		case VarType::isampler2DMS:
			return GL_INT_SAMPLER_2D_MULTISAMPLE;
		case VarType::isampler2DMSArray:
			return GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
		case VarType::isamplerBuffer:
			return GL_INT_SAMPLER_BUFFER;
		case VarType::isampler2DRect:
			return GL_INT_SAMPLER_2D_RECT;
		case VarType::usampler1D:
			return GL_UNSIGNED_INT_SAMPLER_1D;
		case VarType::usampler2D:
			return GL_UNSIGNED_INT_SAMPLER_2D;
		case VarType::usampler3D:
			return GL_UNSIGNED_INT_SAMPLER_3D;
		case VarType::usamplerCube:
			return GL_UNSIGNED_INT_SAMPLER_CUBE;
		case VarType::usampler2DArray:
			return GL_UNSIGNED_INT_SAMPLER_2D_ARRAY;
		case VarType::usampler2DMS:
			return GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE;
		case VarType::usampler2DMSArray:
			return GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
		case VarType::usamplerBuffer:
			return GL_UNSIGNED_INT_SAMPLER_BUFFER;
		case VarType::usampler2DRect:
			return GL_UNSIGNED_INT_SAMPLER_2D_RECT;
		case VarType::image1D:
			return GL_IMAGE_1D;
		case VarType::image2D:
			return GL_IMAGE_2D;
		case VarType::image3D:
			return GL_IMAGE_3D;
		case VarType::image2DRect:
			return GL_IMAGE_2D_RECT;
		case VarType::imageCube:
			return GL_IMAGE_CUBE;
		case VarType::imageBuffer:
			return GL_IMAGE_BUFFER;
		case VarType::image1DArray:
			return GL_IMAGE_1D_ARRAY;
		case VarType::image2DArray:
			return GL_IMAGE_2D_ARRAY;
		case VarType::image2DMS:
			return GL_IMAGE_2D_MULTISAMPLE;
		case VarType::image2DMSArray:
			return GL_IMAGE_2D_MULTISAMPLE_ARRAY;
		case VarType::iimage1D:
			return GL_INT_IMAGE_1D;
		case VarType::iimage2D:
			return GL_INT_IMAGE_2D;
		case VarType::iimage3D:
			return GL_INT_IMAGE_3D;
		case VarType::iimage2DRect:
			return GL_INT_IMAGE_2D_RECT;
		case VarType::iimageCube:
			return GL_INT_IMAGE_CUBE;
		case VarType::iimageBuffer:
			return GL_INT_IMAGE_BUFFER;
		case VarType::iimage1DArray:
			return GL_INT_IMAGE_1D_ARRAY;
		case VarType::iimage2DArray:
			return GL_INT_IMAGE_2D_ARRAY;
		case VarType::iimage2DMS:
			return GL_INT_IMAGE_2D_MULTISAMPLE;
		case VarType::iimage2DMSArray:
			return GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY;
		case VarType::uimage1D:
			return GL_UNSIGNED_INT_IMAGE_1D;
		case VarType::uimage2D:
			return GL_UNSIGNED_INT_IMAGE_2D;
		case VarType::uimage3D:
			return GL_UNSIGNED_INT_IMAGE_3D;
		case VarType::uimage2DRect:
			return GL_UNSIGNED_INT_IMAGE_2D_RECT;
		case VarType::uimageCube:
			return GL_UNSIGNED_INT_IMAGE_CUBE;
		case VarType::uimageBuffer:
			return GL_UNSIGNED_INT_IMAGE_BUFFER;
		case VarType::uimage1DArray:
			return GL_UNSIGNED_INT_IMAGE_1D_ARRAY;
		case VarType::uimage2DArray:
			return GL_UNSIGNED_INT_IMAGE_2D_ARRAY;
		case VarType::uimage2DMS:
			return GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE;
		case VarType::uimage2DMSArray:
			return GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY;
		case VarType::atomic_uint:
			return GL_UNSIGNED_INT_ATOMIC_COUNTER;
		default:
			nat_Throw(natException, "Not a valid VarType"_nv);
		}
	}

	static VarType GetVarType(GLenum type)
	{
		switch (type)
		{
		case GL_FLOAT:
			return VarType::Float;
		case GL_FLOAT_VEC2:
			return VarType::vec2;
		case GL_FLOAT_VEC3:
			return VarType::vec3;
		case GL_FLOAT_VEC4:
			return VarType::vec4;
		case GL_DOUBLE:
			return VarType::Double;
		case GL_DOUBLE_VEC2:
			return VarType::dvec2;
		case GL_DOUBLE_VEC3:
			return VarType::dvec3;
		case GL_DOUBLE_VEC4:
			return VarType::dvec4;
		case GL_INT:
			return VarType::Int;
		case GL_INT_VEC2:
			return VarType::ivec2;
		case GL_INT_VEC3:
			return VarType::ivec3;
		case GL_INT_VEC4:
			return VarType::ivec4;
		case GL_UNSIGNED_INT:
			return VarType::Uint;
		case GL_UNSIGNED_INT_VEC2:
			return VarType::uvec2;
		case GL_UNSIGNED_INT_VEC3:
			return VarType::uvec3;
		case GL_UNSIGNED_INT_VEC4:
			return VarType::uvec4;
		case GL_BOOL:
			return VarType::Bool;
		case GL_BOOL_VEC2:
			return VarType::bvec2;
		case GL_BOOL_VEC3:
			return VarType::bvec3;
		case GL_BOOL_VEC4:
			return VarType::bvec4;
		case GL_FLOAT_MAT2:
			return VarType::mat2;
		case GL_FLOAT_MAT3:
			return VarType::mat3;
		case GL_FLOAT_MAT4:
			return VarType::mat4;
		case GL_FLOAT_MAT2x3:
			return VarType::mat2x3;
		case GL_FLOAT_MAT2x4:
			return VarType::mat2x4;
		case GL_FLOAT_MAT3x2:
			return VarType::mat3x2;
		case GL_FLOAT_MAT3x4:
			return VarType::mat3x4;
		case GL_FLOAT_MAT4x2:
			return VarType::mat4x2;
		case GL_FLOAT_MAT4x3:
			return VarType::mat4x3;
		case GL_DOUBLE_MAT2:
			return VarType::dmat2;
		case GL_DOUBLE_MAT3:
			return VarType::dmat3;
		case GL_DOUBLE_MAT4:
			return VarType::dmat4;
		case GL_DOUBLE_MAT2x3:
			return VarType::dmat2x3;
		case GL_DOUBLE_MAT2x4:
			return VarType::dmat2x4;
		case GL_DOUBLE_MAT3x2:
			return VarType::dmat3x2;
		case GL_DOUBLE_MAT3x4:
			return VarType::dmat3x4;
		case GL_DOUBLE_MAT4x2:
			return VarType::dmat4x2;
		case GL_DOUBLE_MAT4x3:
			return VarType::dmat4x3;
		case GL_SAMPLER_1D:
			return VarType::sampler1D;
		case GL_SAMPLER_2D:
			return VarType::sampler2D;
		case GL_SAMPLER_3D:
			return VarType::sampler3D;
		case GL_SAMPLER_CUBE:
			return VarType::samplerCube;
		case GL_SAMPLER_1D_SHADOW:
			return VarType::sampler1DShadow;
		case GL_SAMPLER_2D_SHADOW:
			return VarType::sampler2DShadow;
		case GL_SAMPLER_1D_ARRAY:
			return VarType::sampler1DArray;
		case GL_SAMPLER_2D_ARRAY:
			return VarType::sampler2DArray;
		case GL_SAMPLER_1D_ARRAY_SHADOW:
			return VarType::sampler1DArrayShadow;
		case GL_SAMPLER_2D_ARRAY_SHADOW:
			return VarType::sampler2DArrayShadow;
		case GL_SAMPLER_2D_MULTISAMPLE:
			return VarType::sampler2DMS;
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
			return VarType::sampler2DMSArray;
		case GL_SAMPLER_CUBE_SHADOW:
			return VarType::samplerCubeShadow;
		case GL_SAMPLER_BUFFER:
			return VarType::samplerBuffer;
		case GL_SAMPLER_2D_RECT:
			return VarType::sampler2DRect;
		case GL_SAMPLER_2D_RECT_SHADOW:
			return VarType::sampler2DRectShadow;
		case GL_INT_SAMPLER_1D:
			return VarType::isampler1D;
		case GL_INT_SAMPLER_2D:
			return VarType::isampler2D;
		case GL_INT_SAMPLER_3D:
			return VarType::isampler3D;
		case GL_INT_SAMPLER_CUBE:
			return VarType::isamplerCube;
		case GL_INT_SAMPLER_1D_ARRAY:
			return VarType::isampler1DArray;
		case GL_INT_SAMPLER_2D_ARRAY:
			return VarType::isampler2DArray;
		case GL_INT_SAMPLER_2D_MULTISAMPLE:
			return VarType::isampler2DMS;
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
			return VarType::isampler2DMSArray;
		case GL_INT_SAMPLER_BUFFER:
			return VarType::isamplerBuffer;
		case GL_INT_SAMPLER_2D_RECT:
			return VarType::isampler2DRect;
		case GL_UNSIGNED_INT_SAMPLER_1D:
			return VarType::usampler1D;
		case GL_UNSIGNED_INT_SAMPLER_2D:
			return VarType::usampler2D;
		case GL_UNSIGNED_INT_SAMPLER_3D:
			return VarType::usampler3D;
		case GL_UNSIGNED_INT_SAMPLER_CUBE:
			return VarType::usamplerCube;
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
			return VarType::usampler2DArray;
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
			return VarType::usampler2DMS;
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
			return VarType::usampler2DMSArray;
		case GL_UNSIGNED_INT_SAMPLER_BUFFER:
			return VarType::usamplerBuffer;
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
			return VarType::usampler2DRect;
		case GL_IMAGE_1D:
			return VarType::image1D;
		case GL_IMAGE_2D:
			return VarType::image2D;
		case GL_IMAGE_3D:
			return VarType::image3D;
		case GL_IMAGE_2D_RECT:
			return VarType::image2DRect;
		case GL_IMAGE_CUBE:
			return VarType::imageCube;
		case GL_IMAGE_BUFFER:
			return VarType::imageBuffer;
		case GL_IMAGE_1D_ARRAY:
			return VarType::image1DArray;
		case GL_IMAGE_2D_ARRAY:
			return VarType::image2DArray;
		case GL_IMAGE_2D_MULTISAMPLE:
			return VarType::image2DMS;
		case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
			return VarType::image2DMSArray;
		case GL_INT_IMAGE_1D:
			return VarType::iimage1D;
		case GL_INT_IMAGE_2D:
			return VarType::iimage2D;
		case GL_INT_IMAGE_3D:
			return VarType::iimage3D;
		case GL_INT_IMAGE_2D_RECT:
			return VarType::iimage2DRect;
		case GL_INT_IMAGE_CUBE:
			return VarType::iimageCube;
		case GL_INT_IMAGE_BUFFER:
			return VarType::iimageBuffer;
		case GL_INT_IMAGE_1D_ARRAY:
			return VarType::iimage1DArray;
		case GL_INT_IMAGE_2D_ARRAY:
			return VarType::iimage2DArray;
		case GL_INT_IMAGE_2D_MULTISAMPLE:
			return VarType::iimage2DMS;
		case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
			return VarType::iimage2DMSArray;
		case GL_UNSIGNED_INT_IMAGE_1D:
			return VarType::uimage1D;
		case GL_UNSIGNED_INT_IMAGE_2D:
			return VarType::uimage2D;
		case GL_UNSIGNED_INT_IMAGE_3D:
			return VarType::uimage3D;
		case GL_UNSIGNED_INT_IMAGE_2D_RECT:
			return VarType::uimage2DRect;
		case GL_UNSIGNED_INT_IMAGE_CUBE:
			return VarType::uimageCube;
		case GL_UNSIGNED_INT_IMAGE_BUFFER:
			return VarType::uimageBuffer;
		case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
			return VarType::uimage1DArray;
		case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
			return VarType::uimage2DArray;
		case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
			return VarType::uimage2DMS;
		case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
			return VarType::uimage2DMSArray;
		case GL_UNSIGNED_INT_ATOMIC_COUNTER:
			return VarType::atomic_uint;
		default:
			nat_Throw(natException, "Not a valid type enum"_nv);
		}
	}
public:
	class AttributeReferenceImpl
		: public natRefObjImpl<AttributeReferenceImpl, AttributeReference>
	{
		friend class n2dShaderProgramImpl;
	public:
		AttributeReferenceImpl(n2dShaderProgramImpl* pProgram, GLhandle Location);

		natRefPointer<n2dShaderProgram> GetProgram() const override;

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
		: public natRefObjImpl<UniformReferenceImpl, UniformReference>
	{
		friend class n2dShaderProgramImpl;
	public:
		UniformReferenceImpl(n2dShaderProgramImpl* pProgram, GLhandle Location);
		natRefPointer<n2dShaderProgram> GetProgram() const override;

		VarType GetType() const override;
		nuInt GetSize() const override;
		void GetValue(nuInt Size, void* Value) const override;
		void SetValue(nuInt Count, const void* Value, nBool Transpose) override;

		HandleType GetLocation() const noexcept
		{
			return m_Location;
		}

	private:
		void onProgramDestroy();

		n2dShaderProgramImpl* m_pProgram;
		GLhandle m_Location;
		GLenum m_Type;
		GLint m_Size;
	};

	class UniformBlockReferenceImpl
		: public natRefObjImpl<UniformBlockReferenceImpl, UniformBlockReference>
	{
		friend class n2dShaderProgramImpl;
	public:
		UniformBlockReferenceImpl(n2dShaderProgramImpl* pProgram, GLhandle Location);

		natRefPointer<n2dShaderProgram> GetProgram() const override;

		nuInt GetSize() const override;
		nuInt ActiveUniformCount() const override;

		void Bind(natRefPointer<n2dBuffer> pBuffer) override;
		nuInt GetBindingPoint() const override;

	private:
		void onProgramDestroy();

		n2dShaderProgramImpl* m_pProgram;
		GLhandle m_Location;
		GLint m_Size, m_ActiveUniformCount;
	};

	explicit n2dShaderProgramImpl(GLhandle ProgramID = 0u);
	n2dShaderProgramImpl(natRefPointer<n2dShaderImpl> pVertexShader, natRefPointer<n2dShaderImpl> pFragmentShader, natRefPointer<n2dShaderImpl> pGeometryShader);
	~n2dShaderProgramImpl();

	HandleType GetHandle() const override;

	void AttachShader(natRefPointer<n2dShader> pShader) override;
	void DetachShader(natRefPointer<n2dShader> pShader) override;

	nuInt AttachedShaderCount() const override;

	void SetBinaryRetrievable(nBool Value) override;
	void SetSeparable(nBool Value) override;

	void Link() override;
	nBool IsLinked() const override;
	nBool Validate() const override;
	nStrView GetInfoLog() const override;
	void Use() const override;
	nBool IsUsing() const override;

	nResult OutputBinary(natRefPointer<natStream> pStream) const override;

	nuInt ActiveAttributeCount() const override;
	nuInt ActiveUniformCount() const override;
	nuInt ActiveUniformBlockCount() const override;

	natRefPointer<AttributeReference> GetAttributeReference(nuInt Location) override;
	natRefPointer<AttributeReference> GetAttributeReference(nStrView Name) override;

	natRefPointer<UniformReference> GetUniformReference(nuInt Location) override;
	natRefPointer<UniformReference> GetUniformReference(nStrView Name) override;

	natRefPointer<UniformBlockReference> GetUniformBlockReference(nuInt Location) override;
	natRefPointer<UniformBlockReference> GetUniformBlockReference(nStrView Name) override;

	static natRefPointer<n2dShaderProgramImpl> CreateFromStream(natRefPointer<natStream> pStream);
private:
	GLhandle m_Program;

	std::unordered_map<GLuint, natRefPointer<AttributeReferenceImpl>> m_AttribMap;
	std::unordered_map<GLuint, natRefPointer<UniformReferenceImpl>> m_UniformMap;
	std::unordered_map<GLuint, natRefPointer<UniformBlockReferenceImpl>> m_UniformBlockMap;

	mutable nString m_Log;
};

class n2dProgramPipelineImpl
	: public natRefObjImpl<n2dProgramPipelineImpl, n2dProgramPipeline>
{
public:
	explicit n2dProgramPipelineImpl(GLhandle PipelineID = 0u);
	~n2dProgramPipelineImpl();

	HandleType GetHandle() const override;
	void Bind() const override;
	nBool IsBinding() const override;

	void UseProgramStages(nuInt stages, natRefPointer<n2dShaderProgram> pProgram) override;
private:
	GLhandle m_Pipeline;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	Shader包装类
////////////////////////////////////////////////////////////////////////////////
class n2dShaderWrapperImpl final
	: public natRefObjImpl<n2dShaderWrapperImpl, n2dShaderWrapper>
{
	friend class n2dEngineImpl;
public:
	explicit n2dShaderWrapperImpl(n2dRenderDevice* pRenderDevice);
	~n2dShaderWrapperImpl();

	nResult CreateShaderFromStream(natRefPointer<natStream> pStream, n2dShader::ShaderType shaderType, nBool bIsBinary, natRefPointer<n2dShader>& pOut) override;
	nResult CreateProgram(natRefPointer<n2dShaderProgram>& pOut) override;
	nResult CreateProgramFromStream(natRefPointer<natStream> pStream, natRefPointer<n2dShaderProgram>& pOut) override;
	nResult CreateProgramPipeline(natRefPointer<n2dProgramPipeline>& pOut) override;

	natRefPointer<n2dShaderProgram> GetCurrentProgram() override;
	natRefPointer<n2dProgramPipeline> GetCurrentProgramPipeline() override;

	natRefPointer<n2dShaderProgramImpl> SetDefaultProgram(natRefPointer<n2dShaderProgramImpl> pProgram);
	natRefPointer<n2dShaderProgram> GetDefaultProgram() const override;

	natRefPointer<n2dShaderProgram> SetFontProgram(natRefPointer<n2dShaderProgram> pProgram) override;
	natRefPointer<n2dShaderProgram> GetFontProgram() const override;

	GLuint GetAvailableBindPoint();
	void UseBindingPoint(GLuint BindingPoint);
	void ReleaseBindingPoint(GLuint BindingPoint);
private:
	std::unordered_set<nuInt> m_AvailableBindingPoint;
	std::unordered_map<GLhandle, natRefPointer<n2dShaderProgramImpl>> m_Programs;
	std::unordered_map<GLhandle, natRefPointer<n2dProgramPipelineImpl>> m_ProgramPipelines;

	n2dRenderDevice* m_pRenderDevice;
	natRefPointer<n2dShaderProgramImpl> m_DefaultProgram;
	natRefPointer<n2dShaderProgram> m_FontProgram;

	nuInt m_MaxBindingPoint;
};
