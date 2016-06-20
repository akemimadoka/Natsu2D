#pragma once
#include "..\n2dLayer.h"
#include <unordered_map>
#include <unordered_set>
#include <map>

class n2dLayerImpl
	: public natRefObjImpl<n2dLayer>
{
public:
	n2dLayerImpl(std::function<nBool(nDouble, n2dRenderDevice*)> RenderHandler, std::function<nBool(nDouble)> UpdateHandler, nInt Order = 0, ncTStr Name = nullptr, natNode* pParent = nullptr);
	~n2dLayerImpl();

	void AddChild(natNode* pChild) override;
	nBool ChildExists(natNode* pChild) const noexcept override;
	nBool ChildExists(ncTStr Name) const noexcept override;
	natNode* GetChildByName(ncTStr Name) const noexcept override;

	nBool EnumChildNode(nBool Recursive, std::function<nBool(natNode*)> EnumCallback) override;

	size_t GetChildCount() const noexcept override;
	void SetParent(natNode* pParent) override;
	natNode* GetParent() const noexcept override;
	void RemoveChild(natNode* pnatNode) override;
	void RemoveChildByName(ncTStr Name) override;
	void RemoveAllChild() override;

	void SetName(ncTStr Name) override;
	ncTStr GetName() const noexcept override;

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
		std::unordered_map<nTString, natRefPointer<n2dRenderNode>> m_NamedRenderNodes;
		std::unordered_set<natRefPointer<n2dRenderNode>> m_UnnamedRenderNodes;
	};

	/// @brief key: Order
	std::map<nInt, ChildRenderNodes> m_ChildRenderNodes;

	std::function<nBool(nDouble, n2dRenderDevice*)> m_RenderHandler;
	std::function<nBool(nDouble)> m_UpdateHandler;

	natNode* m_pParent;
	nTString m_Name;
	natRect<> m_BoundingRect;
	nInt m_Order;
};
