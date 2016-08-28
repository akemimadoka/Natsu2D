////////////////////////////////////////////////////////////////////////////////
///	@file	n2dLayer.h
///	@brief	Natsu2DÍ¼²ã
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "n2dRenderNode.h"

struct n2dLayer
	: n2dRenderNode
{
	void AddChild(natNode* pChild) override = 0;
	nBool ChildExists(natNode* pChild) const noexcept override = 0;
	nBool ChildExists(ncTStr Name) const noexcept override = 0;
	natNode* GetChildByName(ncTStr Name) const override = 0;

	nBool EnumChildNode(nBool Recursive, std::function<nBool(natNode*)> EnumCallback) override = 0;

	size_t GetChildCount() const noexcept override = 0;
	void SetParent(natNode* pParent) override = 0;
	natNode* GetParent() const noexcept override = 0;
	void RemoveChild(natNode* pnatNode) override = 0;
	void RemoveChildByName(ncTStr Name) override = 0;
	void RemoveAllChild() override = 0;

	void SetName(ncTStr Name) override = 0;
	ncTStr GetName() const noexcept override = 0;

	nBool Render(nDouble ElapsedTime, n2dRenderDevice* pRenderer) override = 0;
	nBool Update(nDouble ElapsedTime) override = 0;

	void SetBoundingRect(natRect<> const& Rect) override = 0;
	natRect<> GetBoundingRect() const noexcept override = 0;

	void SetOrder(nInt Order) override = 0;
	nInt GetOrder() const noexcept override = 0;
};