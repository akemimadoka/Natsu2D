#pragma once
#include "n2dInterface.h"
#include <natVec.h>

struct n2dModelData;
struct n2dTexture2D;
struct n2dModelLoader;
struct n2dMeshData;

struct n2dGraphics2DVertex
{
	natVec3<> vert;
	nuInt reserved;
	natVec2<> uv;
};

struct n2dGraphics3DVertex
{
	natVec3<> vert;
	natVec2<> uv;
	natVec3<> normal;
};

struct n2dGraphics
	: n2dInterface
{
	virtual ~n2dGraphics() = default;

	virtual nBool IsRendering() const = 0;

	virtual nResult Begin() = 0;
	virtual nResult Flush() = 0;
	virtual nResult End() = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	��άͼԪ��Ⱦ��
////////////////////////////////////////////////////////////////////////////////
struct n2dGraphics2D
	: n2dGraphics
{
	///	@brief	���ƾ���
	virtual nResult DrawQuad(
		n2dTexture2D* pTex,
		n2dGraphics2DVertex const& v1,
		n2dGraphics2DVertex const& v2,
		n2dGraphics2DVertex const& v3,
		n2dGraphics2DVertex const& v4
		) = 0;

	///	@brief	���ƾ���
	virtual nResult DrawQuad(n2dTexture2D* pTex, const n2dGraphics2DVertex* varr) = 0;

	///	@brief	���ɻ���
	virtual nResult DrawRaw(n2dTexture2D* pTex, nuInt cVertex, nuInt cIndex, const n2dGraphics2DVertex* varr, const nuShort* iarr) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	��άͼԪ��Ⱦ��
////////////////////////////////////////////////////////////////////////////////
struct n2dGraphics3D
	: n2dGraphics
{
	///	@brief	����ģ��
	virtual nResult RenderModel(n2dModelData* pModelData) = 0;
};