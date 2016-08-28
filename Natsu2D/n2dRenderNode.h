////////////////////////////////////////////////////////////////////////////////
///	@file	n2dRenderNode.h
///	@brief	Natsu2D渲染节点
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <natNode.h>
#include <natMath.h>
#include "n2dGraphics.h"

struct n2dRenderDevice;

////////////////////////////////////////////////////////////////////////////////
///	@brief	渲染节点
////////////////////////////////////////////////////////////////////////////////
class n2dRenderNode
	: public natNode
{
public:
	///	@brief	开始渲染
	virtual nBool Render(nDouble ElapsedTime, n2dRenderDevice* pRenderer) = 0;
	///	@brief	开始更新
	virtual nBool Update(nDouble ElapsedTime) = 0;

	///	@brief	设置边界框
	virtual void SetBoundingRect(natRect<> const& Rect) = 0;
	///	@brief	获得边界框
	virtual natRect<> GetBoundingRect() const noexcept = 0;

	///	@brief	设置次序
	virtual void SetOrder(nInt Order) = 0;
	///	@brief	获得次序
	virtual nInt GetOrder() const noexcept = 0;

protected:
	///	@brief	渲染时回调
	virtual nBool OnRender(nDouble ElapsedTime, n2dRenderDevice* pRenderer) = 0;
	///	@brief	更新时回调
	virtual nBool OnUpdate(nDouble ElapsedTime) = 0;
};
