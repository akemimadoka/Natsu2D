#pragma once
#include "n2dInterface.h"

struct natStream;
struct n2dTexture;

struct n2dModelLoader
	: n2dInterface
{
	virtual ~n2dModelLoader() = default;

	virtual nResult LoadFromStream(natStream* pStream) = 0;
	virtual nResult LoadFromFile(ncTStr lpPath) = 0;

	virtual nuInt GetVertexBuffer() = 0;
	virtual nuInt GetUVBuffer() = 0;
	virtual nuInt GetNormalBuffer() = 0;
	virtual nuInt GetIndexBuffer() = 0;

	virtual nuInt GetVertexCount() const = 0;
	virtual nuInt GetUVCount() const = 0;
	virtual nuInt GetNormalCount() const = 0;
	virtual nuInt GetIndexCount() const = 0;
};

