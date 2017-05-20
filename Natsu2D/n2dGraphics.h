////////////////////////////////////////////////////////////////////////////////
///	@file	n2dGraphics.h
///	@brief	Natsu2D图元渲染器
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "n2dInterface.h"
#include <natVec.h>

struct n2dModelData;
struct n2dTexture2D;
struct n2dStaticModelLoader;
struct n2dStaticMeshData;

///	@brief	二维图元渲染器用顶点
struct n2dGraphics2DVertex
{
	natVec3<> vert;
	nuInt reserved;
	natVec2<> uv;
};

///	@brief	三维图元渲染器用顶点
struct n2dGraphics3DVertex
{
	natVec3<> vert;
	natVec2<> uv;
	natVec3<> normal;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	图元渲染器基类
////////////////////////////////////////////////////////////////////////////////
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
///	@brief	二维图元渲染器
////////////////////////////////////////////////////////////////////////////////
struct n2dGraphics2D
	: n2dGraphics
{
	///	@brief	绘制矩形
	virtual nResult DrawQuad(
		natRefPointer<n2dTexture2D> pTex,
		n2dGraphics2DVertex const& v1,
		n2dGraphics2DVertex const& v2,
		n2dGraphics2DVertex const& v3,
		n2dGraphics2DVertex const& v4
		) = 0;

	///	@brief	绘制矩形
	virtual nResult DrawQuad(natRefPointer<n2dTexture2D> pTex, const n2dGraphics2DVertex* varr) = 0;

	///	@brief	自由绘制
	virtual nResult DrawRaw(natRefPointer<n2dTexture2D> pTex, nuInt cVertex, nuInt cIndex, const n2dGraphics2DVertex* varr, const nuShort* iarr) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	三维图元渲染器
////////////////////////////////////////////////////////////////////////////////
struct n2dGraphics3D
	: n2dGraphics
{
	///	@brief	绘制模型
	virtual nResult RenderModel(natRefPointer<n2dModelData> pModelData) = 0;
};