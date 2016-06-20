#pragma once
#include <natNode.h>
#include <natMath.h>
#include "n2dGraphics.h"

struct n2dRenderDevice;

class n2dRenderNode
	: public natNode
{
public:
	virtual nBool Render(nDouble ElapsedTime, n2dRenderDevice* pRenderer) = 0;
	virtual nBool Update(nDouble ElapsedTime) = 0;

	virtual void SetBoundingRect(natRect<> const& Rect) = 0;
	virtual natRect<> GetBoundingRect() const noexcept = 0;

	virtual void SetOrder(nInt Order) = 0;
	virtual nInt GetOrder() const noexcept = 0;

protected:
	virtual nBool OnRender(nDouble ElapsedTime, n2dRenderDevice* pRenderer) = 0;
	virtual nBool OnUpdate(nDouble ElapsedTime) = 0;
};
