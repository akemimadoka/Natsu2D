#pragma once
#include "../n2dLayer.h"
#include <unordered_map>
#include <unordered_set>
#include <map>

class n2dLayerImpl
	: public natRefObjImpl<n2dLayerImpl, n2dLayer>
{
public:
	explicit n2dLayerImpl(n2dLayerHandler* Handler, nInt Order = 0, nStrView Name = nullptr, natNode* pParent = nullptr);
	~n2dLayerImpl();

	void AddChild(natRefPointer<natNode> pChild) override;
	nBool ChildExists(natRefPointer<natNode> pChild) const noexcept override;
	nBool ChildExists(nStrView Name) const noexcept override;
	natRefPointer<natNode> GetChildByName(nStrView Name) const noexcept override;

	nBool EnumChildNode(nBool Recursive, std::function<nBool(natRefPointer<natNode>)> EnumCallback) override;

	size_t GetChildCount() const noexcept override;
	void SetParent(natNode* pParent) override;
	natRefPointer<natNode> GetParent() const noexcept override;
	void RemoveChild(natRefPointer<natNode> pnatNode) override;
	void RemoveChildByName(nStrView Name) override;
	void RemoveAllChild() override;

	void SetName(nStrView Name) override;
	nStrView GetName() const noexcept override;

	nBool Render(nDouble ElapsedTime, n2dRenderDevice* pRenderer) override;
	nBool Update(nDouble ElapsedTime) override;

	void SetBoundingRect(natRect<> const& Rect) override;
	natRect<> GetBoundingRect() const noexcept override;

	void SetOrder(nInt Order) override;
	nInt GetOrder() const noexcept override;

private:
	nBool OnRender(nDouble ElapsedTime, n2dRenderDevice* pRenderer) override;
	nBool OnUpdate(nDouble ElapsedTime) override;

	struct ChildRenderNodes
	{
		std::unordered_map<nString, natRefPointer<n2dRenderNode>> m_NamedRenderNodes;
		std::unordered_set<natRefPointer<n2dRenderNode>> m_UnnamedRenderNodes;
	};

	/// @brief key: Order
	std::map<nInt, ChildRenderNodes> m_ChildRenderNodes;

	n2dLayerHandler* m_Handler;

	natNode* m_pParent;
	nString m_Name;
	natRect<> m_BoundingRect;
	nInt m_Order;
};

class n2dLayerMgrImpl
	: public natRefObjImpl<n2dLayerMgrImpl, n2dLayerMgr>
{
public:
	n2dLayerMgrImpl();

	nResult CreateLayer(n2dLayerHandler* Handler, natRefPointer<n2dLayer>& pOut, nInt Order = 0, nStrView Name = nullptr, natNode* pParent = nullptr) override;
	n2dLayer* GetRootLayer() override;

private:
	n2dLayerImpl m_RootLayer;
};
