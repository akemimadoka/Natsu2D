#include "n2dLayerImpl.h"
#include <natUtil.h>
#include <cassert>

n2dLayerImpl::n2dLayerImpl(n2dLayerHandler* Handler, nInt Order, nStrView Name, natNode* pParent)
	: m_Handler(Handler), m_pParent(pParent), m_Name(Name), m_Order(Order)
{
}

n2dLayerImpl::~n2dLayerImpl()
{
	natRefObjImpl<n2dLayer>::AddRef();
	natRefObjImpl<n2dLayer>::AddRef();
	if (m_pParent)
	{
		n2dLayerImpl::SetParent(nullptr);
	}
	n2dLayerImpl::RemoveAllChild();
}

void n2dLayerImpl::AddChild(natNode* pChild)
{
	assert(pChild && "pChild should be a valid pointer.");

	n2dRenderNode* pChildn2dRenderNode = dynamic_cast<n2dRenderNode*>(pChild);
	if (!pChildn2dRenderNode)
	{
		nat_Throw(natException, "pChild is not a Layer."_nv);
	}

	auto Order = pChildn2dRenderNode->GetOrder();
	auto& Child = m_ChildRenderNodes[Order];
	auto Name = pChildn2dRenderNode->GetName();
	natRefPointer<n2dRenderNode> pRefChild(pChildn2dRenderNode);

	if (!Name.empty())
	{
		nString StrName(Name);
		auto iter = Child.m_NamedRenderNodes.find(StrName);
		if (iter == Child.m_NamedRenderNodes.end())
		{
			Child.m_NamedRenderNodes[StrName] = pRefChild;
			pChildn2dRenderNode->SetParent(this);
			return;
		}
		nat_Throw(natException, "Node \"%s\" already existed."_nv, Name);
	}

	auto iter = Child.m_UnnamedRenderNodes.find(pRefChild);
	if (iter == Child.m_UnnamedRenderNodes.end())
	{
		Child.m_UnnamedRenderNodes.insert(pRefChild);
		pChildn2dRenderNode->SetParent(this);
		return;
	}
	nat_Throw(natException, "Node %p already existed."_nv, pChildn2dRenderNode);
}

nBool n2dLayerImpl::ChildExists(natNode* pChild) const noexcept
{
	assert(pChild && "pChild should be a valid pointer.");

	n2dRenderNode* pRenderNode = dynamic_cast<n2dRenderNode*>(pChild);

	assert(pRenderNode && "pChild should be a RenderNode.");

	auto iter = m_ChildRenderNodes.find(pRenderNode->GetOrder());
	if (iter != m_ChildRenderNodes.end())
	{
		auto Name = pChild->GetName();
		if (!Name.empty() && iter->second.m_NamedRenderNodes.find(Name) != iter->second.m_NamedRenderNodes.end())
		{
			return true;
		}

		return iter->second.m_UnnamedRenderNodes.find(natRefPointer<n2dRenderNode>(pRenderNode)) != iter->second.m_UnnamedRenderNodes.end();
	}

	return false;
}

nBool n2dLayerImpl::ChildExists(nStrView Name) const noexcept
{
	assert(!Name.empty() && "Name should be a valid pointer.");

	nString StrName(Name);
	for (auto& iter : m_ChildRenderNodes)
	{
		if (iter.second.m_NamedRenderNodes.find(StrName) != iter.second.m_NamedRenderNodes.end())
		{
			return true;
		}
	}

	return false;
}

natNode* n2dLayerImpl::GetChildByName(nStrView Name) const noexcept
{
	assert(!Name.empty() && "Name should be a valid pointer.");

	nString StrName(Name);
	for (auto& iter : m_ChildRenderNodes)
	{
		auto iter2 = iter.second.m_NamedRenderNodes.find(StrName);
		if (iter2 != iter.second.m_NamedRenderNodes.end())
		{
			return iter2->second;
		}
	}

	return nullptr;
}

nBool n2dLayerImpl::EnumChildNode(nBool Recursive, std::function<nBool(natNode*)> EnumCallback)
{
	assert(EnumCallback && "EnumCallback should be a valid callable object.");

	bool Enumerated = false;
	for (auto& iter : m_ChildRenderNodes)
	{
		if (!Enumerated && iter.first > 0)
		{
			if (EnumCallback(this))
			{
				return true;
			}
		}

		for (auto& iter2 : iter.second.m_NamedRenderNodes)
		{
			if (EnumCallback(iter2.second))
			{
				return true;
			}

			if (Recursive)
			{
				if (iter2.second->EnumChildNode(Recursive, EnumCallback))
				{
					return true;
				}
			}
		}

		for (auto& iter2 : iter.second.m_UnnamedRenderNodes)
		{
			if (EnumCallback(iter2))
			{
				return true;
			}

			if (Recursive)
			{
				if (iter2->EnumChildNode(Recursive, EnumCallback))
				{
					return true;
				}
			}
		}
	}

	if (!Enumerated)
	{
		return EnumCallback(this);
	}

	return false;
}

size_t n2dLayerImpl::GetChildCount() const noexcept
{
	size_t Result = 0u;
	for (auto& iter : m_ChildRenderNodes)
	{
		Result += iter.second.m_NamedRenderNodes.size() + iter.second.m_UnnamedRenderNodes.size();
	}

	return Result;
}

void n2dLayerImpl::SetParent(natNode* pParent)
{
	if (m_pParent == pParent)
	{
		return;
	}

	natNode* pOldParent = m_pParent;
	m_pParent = pParent;
	if (pOldParent)
	{
		pOldParent->RemoveChild(this);
	}
}

natNode* n2dLayerImpl::GetParent() const noexcept
{
	return m_pParent;
}

void n2dLayerImpl::RemoveChild(natNode* pNode)
{
	assert(pNode && "pNode should be a valid pointer.");

	n2dRenderNode* pRenderNode = dynamic_cast<n2dRenderNode*>(pNode);

	assert(pRenderNode && "pChild should be a RenderNode.");

	auto iter = m_ChildRenderNodes.find(pRenderNode->GetOrder());
	if (iter != m_ChildRenderNodes.end())
	{
		if (iter->second.m_NamedRenderNodes.erase(pNode->GetName()) == 0u)
		{
			iter->second.m_UnnamedRenderNodes.erase(natRefPointer<natNode>(pNode));
		}
	}
}

void n2dLayerImpl::RemoveChildByName(nStrView Name)
{
	assert(!Name.empty() && "Name should be a valid pointer.");

	for (auto& iter : m_ChildRenderNodes)
	{
		if (iter.second.m_NamedRenderNodes.erase(Name) > 0u)
		{
			return;
		}
	}
}

void n2dLayerImpl::RemoveAllChild()
{
	m_ChildRenderNodes.clear();
}

void n2dLayerImpl::SetName(nStrView Name)
{
	assert(!Name.empty() && "Name should be a valid pointer.");

	if (m_Name != Name)
	{
		nString OldName(std::move(m_Name));
		if (!Name.empty())
		{
			m_Name = Name;
		}
		else
		{
			m_Name.Clear();
		}

		if (m_pParent)
		{
			AddRef();
			m_pParent->RemoveChildByName(OldName);
			m_pParent->AddChild(this);
			Release();
		}
	}
}

nStrView n2dLayerImpl::GetName() const noexcept
{
	return m_Name;
}

nBool n2dLayerImpl::Render(nDouble ElapsedTime, n2dRenderDevice* pRenderer)
{
	bool Result = true;
	bool Rendered = false;

	for (auto& iter : m_ChildRenderNodes)
	{
		if (!Rendered && iter.first > 0)
		{
			OnRender(ElapsedTime, pRenderer);
			Rendered = true;
		}

		for (auto& iter2 : iter.second.m_NamedRenderNodes)
		{
			Result &= iter2.second->Render(ElapsedTime, pRenderer);
		}

		for (auto& iter2 : iter.second.m_UnnamedRenderNodes)
		{
			Result &= iter2->Render(ElapsedTime, pRenderer);
		}
	}

	if (!Rendered)
	{
		OnRender(ElapsedTime, pRenderer);
	}

	return Result;
}

nBool n2dLayerImpl::Update(nDouble ElapsedTime)
{
	bool Result = true;
	bool Updated = false;

	for (auto& iter : m_ChildRenderNodes)
	{
		if (!Updated && iter.first > 0)
		{
			OnUpdate(ElapsedTime);
			Updated = true;
		}

		for (auto& iter2 : iter.second.m_NamedRenderNodes)
		{
			Result &= iter2.second->Update(ElapsedTime);
		}

		for (auto& iter2 : iter.second.m_UnnamedRenderNodes)
		{
			Result &= iter2->Update(ElapsedTime);
		}
	}

	if (!Updated)
	{
		OnUpdate(ElapsedTime);
	}

	return Result;
}

void n2dLayerImpl::SetBoundingRect(natRect<> const& Rect)
{
	m_BoundingRect = Rect;
}

natRect<> n2dLayerImpl::GetBoundingRect() const noexcept
{
	return m_BoundingRect;
}

void n2dLayerImpl::SetOrder(nInt Order)
{
	if (m_Order != Order)
	{
		m_Order = Order;
		if (m_pParent && dynamic_cast<n2dRenderNode*>(m_pParent))
		{
			AddRef();
			m_pParent->RemoveChild(this);
			m_pParent->AddChild(this);
			Release();
		}
	}
}

nInt n2dLayerImpl::GetOrder() const noexcept
{
	return m_Order;
}

nBool n2dLayerImpl::OnRender(nDouble ElapsedTime, n2dRenderDevice* pRenderer)
{
	return m_Handler->OnRender(ElapsedTime, pRenderer);
}

nBool n2dLayerImpl::OnUpdate(nDouble ElapsedTime)
{
	return m_Handler->OnUpdate(ElapsedTime);
}

namespace
{
	struct DummyLayerHandler
		: natRefObjImpl<n2dLayerHandler>
	{
		nBool OnRender(nDouble ElapsedTime, n2dRenderDevice* pRenderer) override
		{
			return true;
		}

		nBool OnUpdate(nDouble ElapsedTime) override
		{
			return true;
		}
	} g_DummyLayerHandler;
}

n2dLayerMgrImpl::n2dLayerMgrImpl()
	: m_RootLayer(&g_DummyLayerHandler)
{
}

nResult n2dLayerMgrImpl::CreateLayer(n2dLayerHandler* Handler, n2dLayer** pOut, nInt Order, nStrView Name, natNode* pParent)
{
	if (pOut == nullptr)
	{
		return NatErr_InvalidArg;
	}

	try
	{
		*pOut = new n2dLayerImpl(Handler, Order, Name, pParent ? pParent : &m_RootLayer);
	}
	catch (std::bad_alloc&)
	{
		nat_Throw(natException, "Failed to allocate memory"_nv);
	}
	catch (...)
	{
		return NatErr_Unknown;
	}

	return NatErr_OK;
}

n2dLayer* n2dLayerMgrImpl::GetRootLayer()
{
	return &m_RootLayer;
}
