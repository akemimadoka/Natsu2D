#pragma once
#include "n2dInterface.h"
#include "n2dTexture.h"
#include "natVec.h"

struct n2dModelData;
struct natStream;

struct n2dMeshData
	: n2dInterface
{
	struct Material
	{
		nBool WireFrame;
		nBool Both_sided;
		natRefPointer<n2dTexture2D> Texture;
		natVec4<> Diffuse;
		natVec4<> Specular;
		natVec4<> Ambient;
		natVec4<> Emission;
		nFloat Shininess;
		nFloat Strength;
	};

	virtual nuInt GetVertexBuffer() = 0;
	virtual nuInt GetIndexBuffer() = 0;

	virtual nuInt GetVertexCount() const = 0;
	virtual nuInt GetIndexCount() const = 0;
};

struct n2dModelLoader
	: n2dInterface
{
	virtual nResult LoadFromStream(natStream* pStream) = 0;
	virtual nResult LoadFromFile(ncTStr lpPath) = 0;

	virtual void SetDefaultTexture(n2dTexture2D* Texture) = 0;

	virtual n2dModelData* GetModel() = 0;

	virtual nuInt GetVertexBuffer() = 0;
	virtual nuInt GetUVBuffer() = 0;
	virtual nuInt GetNormalBuffer() = 0;
	virtual nuInt GetIndexBuffer() = 0;

	virtual nuInt GetVertexCount() const = 0;
	virtual nuInt GetIndexCount() const = 0;
};

struct n2dModelData
	: n2dInterface
{
	virtual nuInt GetMeshCount() const = 0;
	//virtual n2dMeshData* GetMeshes() = 0;
};