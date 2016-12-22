////////////////////////////////////////////////////////////////////////////////
///	@file	n2dLayer.h
///	@brief	Natsu2D图层
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "n2dRenderNode.h"

struct n2dLayer
	: n2dRenderNode
{
	void AddChild(natNode* pChild) override = 0;
	nBool ChildExists(natNode* pChild) const noexcept override = 0;
	nBool ChildExists(nStrView Name) const noexcept override = 0;
	natNode* GetChildByName(nStrView Name) const override = 0;

	nBool EnumChildNode(nBool Recursive, std::function<nBool(natNode*)> EnumCallback) override = 0;

	size_t GetChildCount() const noexcept override = 0;
	void SetParent(natNode* pParent) override = 0;
	natNode* GetParent() const noexcept override = 0;
	void RemoveChild(natNode* pnatNode) override = 0;
	void RemoveChildByName(nStrView Name) override = 0;
	void RemoveAllChild() override = 0;

	void SetName(nStrView Name) override = 0;
	nStrView GetName() const noexcept override = 0;

	nBool Render(nDouble ElapsedTime, n2dRenderDevice* pRenderer) override = 0;
	nBool Update(nDouble ElapsedTime) override = 0;

	void SetBoundingRect(natRect<> const& Rect) override = 0;
	natRect<> GetBoundingRect() const noexcept override = 0;

	void SetOrder(nInt Order) override = 0;
	nInt GetOrder() const noexcept override = 0;
};

struct n2dLayerHandler
	: n2dInterface
{
	///	@brief	渲染时回调
	virtual nBool OnRender(nDouble ElapsedTime, n2dRenderDevice* pRenderer) = 0;
	///	@brief	更新时回调
	virtual nBool OnUpdate(nDouble ElapsedTime) = 0;
};

struct n2dLayerMgr
	: n2dInterface
{
	///	@brief	创建图层
	///	@param[in]	Handler	处理对象
	///	@param[out]	pOut	创建的图层
	///	@param[in]	Order	图层顺序，可省略
	///	@param[in]	Name	图层名，可省略
	///	@param[in]	pParent	图层所属的节点
	///	@return	处理结果
	virtual nResult CreateLayer(n2dLayerHandler* Handler, n2dLayer** pOut, nInt Order = 0, nStrView Name = nullptr, natNode* pParent = nullptr) = 0;

	/// @brief	获得根图层
	virtual n2dLayer* GetRootLayer() = 0;
};
