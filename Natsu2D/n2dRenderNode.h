////////////////////////////////////////////////////////////////////////////////
///	@file	n2dRenderNode.h
///	@brief	Natsu2D��Ⱦ�ڵ�
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <natNode.h>
#include <natMath.h>
#include "n2dGraphics.h"

struct n2dRenderDevice;

////////////////////////////////////////////////////////////////////////////////
///	@brief	��Ⱦ�ڵ�
////////////////////////////////////////////////////////////////////////////////
class n2dRenderNode
	: public natNode
{
public:
	///	@brief	��ʼ��Ⱦ
	virtual nBool Render(nDouble ElapsedTime, n2dRenderDevice* pRenderer) = 0;
	///	@brief	��ʼ����
	virtual nBool Update(nDouble ElapsedTime) = 0;

	///	@brief	���ñ߽��
	virtual void SetBoundingRect(natRect<> const& Rect) = 0;
	///	@brief	��ñ߽��
	virtual natRect<> GetBoundingRect() const noexcept = 0;

	///	@brief	���ô���
	virtual void SetOrder(nInt Order) = 0;
	///	@brief	��ô���
	virtual nInt GetOrder() const noexcept = 0;

protected:
	///	@brief	��Ⱦʱ�ص�
	virtual nBool OnRender(nDouble ElapsedTime, n2dRenderDevice* pRenderer) = 0;
	///	@brief	����ʱ�ص�
	virtual nBool OnUpdate(nDouble ElapsedTime) = 0;
};
