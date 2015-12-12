////////////////////////////////////////////////////////////////////////////////
///	@file	event.h
///	@brief	Event��ʵ��
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <map>
#include <unordered_set>
#include <vector>
#include "natType.h"
#include <unordered_map>

namespace Priority
{
	////////////////////////////////////////////////////////////////////////////////
	///	@brief	Event���ȼ�
	///	@note	Ϊ��ֹ��Ⱦȫ�������ռ佫�������Priority�����ռ�\n
	///			EventHandleִ�����ȼ�˳��Ϊ��-��ͨ-��
	////////////////////////////////////////////////////////////////////////////////
	enum Priority
	{
		High = 1,	///< @brief	�����ȼ�
		Normal = 2,	///< @brief	��ͨ���ȼ�
		Low = 3		///< @brief	�����ȼ�
	};
}

template <typename dataT>
////////////////////////////////////////////////////////////////////////////////
///	@brief	Event��ʵ��
///	@note	����һ��data���û��ɸ�������Ҫ�ṩ��������
////////////////////////////////////////////////////////////////////////////////
class natEvent
{
public:
	typedef void(*EventHandle)(natEvent&);
	typedef dataT dataType;

	//__declspec(deprecated)
	explicit natEvent(nBool isEventCancelable = false)
		: m_Cancelable(isEventCancelable),
		m_isCanceled(false),
		m_data()
	{
	}
	explicit natEvent(dataT const& data, nBool isEventCancelable)
		: m_Cancelable(isEventCancelable),
		m_isCanceled(false),
		m_data(data)
	{
	}
	natEvent(natEvent const& b)
		: m_EventHandler(b.m_EventHandler),
		m_Cancelable(b.m_Cancelable),
		m_isCanceled(b.m_isCanceled),
		m_data(b.m_data)
	{
	}

	virtual ~natEvent()
	{
	}

	virtual natEvent& operator=(natEvent const& b)
	{
		if (this == &b)
		{
			return *this;
		}

		m_EventHandler = b.m_EventHandler;
		m_data = b.m_data;

		if (m_Cancelable == b.m_Cancelable)
		{
			m_isCanceled = b.m_isCanceled;
		}

		return *this;
	}

	virtual void Register(EventHandle func, nInt priority = Priority::Normal)
	{
		m_EventHandler[priority].insert(func);
	}

	/*bool Unregister(nuInt HandlerIndex, int Priority)
	{
		auto eventhandlerlist = m_EventHandler.find(Priority);
		if (eventhandlerlist != m_EventHandler.end())
		{
			if (HandlerIndex >= eventhandlerlist->second.size())
			{
				return false;
			}

			auto i = eventhandlerlist->second.begin();
			for (uint id = HandlerIndex; id > 0u; --id)
			{
				++i;
			}

			eventhandlerlist->second.erase(i);
			eventhandlerlist->second.erase(eventhandlerlist->second.begin() + HandlerIndex);
			return true;
		}

		return false;
	}*/

	virtual void Unregister(EventHandle Handler)
	{
		for (auto& eventhandlerlist : m_EventHandler)
		{
			eventhandlerlist.second.erase(Handler);
		}
	}

	virtual void operator+=(EventHandle func)
	{
		Register(func);
	}

	/*bool operator-=(nuInt HandlerIndex)
	{
		return Unregister(HandlerIndex, Priority::Normal);
	}*/
	virtual void operator-=(EventHandle Handler)
	{
		Unregister(Handler);
	}

	nBool isCancelable() const
	{
		return m_Cancelable;
	}

	void SetCanceled(nBool cancel = true)
	{
		if (m_Cancelable)
		{
			m_isCanceled = cancel;
		}
	}

	nBool isCanceled() const
	{
		return m_isCanceled;
	}

	virtual void Release()
	{
		m_EventHandler.clear();
		//SetCanceled();
	}

	//__declspec(deprecated)
	virtual nBool Activate(nInt PriorityLimitmin = Priority::Low, nInt PriorityLimitmax = Priority::High)
	{
		m_isCanceled = false;
		if (!m_EventHandler.empty())
		{
			for (auto i = PriorityLimitmax; i <= PriorityLimitmin; ++i)
			{
				for (auto& eh : m_EventHandler[i])
				{
					eh(*this);
				}
			}
		}

		return m_isCanceled;
	}

	virtual nBool Activate(dataType const& data, nInt PriorityLimitmin, nInt PriorityLimitmax)
	{
		SetData(data);
		return Activate(PriorityLimitmin, PriorityLimitmax);
	}

	nBool operator()()
	{
		return Activate(Priority::Low, Priority::High);
	}

	nBool operator()(dataType const& data)
	{
		return Activate(data, Priority::Low, Priority::High);
	}

	const dataT& GetData() const
	{
		return m_data;
	}

	void SetData(dataType const& val)
	{
		dataT tmp(val);
		std::swap(m_data, tmp);
	}

	void SetData(dataType&& val)
	{
		std::swap(m_data, std::move(val));
	}

	__declspec(property(get = GetData, put = SetData))
	dataT Data;
protected:
	std::map<nInt, std::unordered_set<EventHandle>> m_EventHandler;
	const nBool m_Cancelable;
	nBool m_isCanceled;

	mutable dataT m_data;
};


template <>
////////////////////////////////////////////////////////////////////////////////
///	@brief	Eventʵ��
///	@note	���಻����data
////////////////////////////////////////////////////////////////////////////////
class natEvent<void>
{
public:
	typedef void(*EventHandle)(natEvent&);

	explicit natEvent(nBool isEventCancelable = false) : m_Cancelable(isEventCancelable), m_isCanceled(false)
	{
	}
	natEvent(natEvent const& b) : m_EventHandler(b.m_EventHandler), m_Cancelable(b.m_Cancelable), m_isCanceled(b.m_isCanceled)
	{
	}

	virtual ~natEvent()
	{
		//Release();
	}

	virtual natEvent& operator=(natEvent const& b)
	{
		for each (auto x in b.m_EventHandler)
		{
			m_EventHandler.insert(x);
		}

		if (m_Cancelable == b.m_Cancelable)
		{
			m_isCanceled = b.m_isCanceled;
		}

		return *this;
	}

	virtual void Register(EventHandle func, nInt priority = Priority::Normal)
	{
		m_EventHandler[priority].insert(func);
	}

	/*bool Unregister(nuInt HandlerIndex, int Priority)
	{
		auto eventhandlerlist = m_EventHandler.find(Priority);
		if (eventhandlerlist != m_EventHandler.end())
		{
			if (HandlerIndex >= eventhandlerlist->second.size())
			{
				return false;
			}

			auto i = eventhandlerlist->second.begin();
			for (; HandlerIndex > 0u; --HandlerIndex)
			{
				++i;
			}

			eventhandlerlist->second.erase(eventhandlerlist->second.begin() + HandlerIndex);
			return true;
		}

		return false;
	}*/

	virtual void Unregister(EventHandle Handler)
	{
		for (auto& eventhandlerlist : m_EventHandler)
		{
			eventhandlerlist.second.erase(Handler);
		}
	}

	virtual void operator+=(EventHandle func)
	{
		Register(func);
	}
	/*bool operator-=(nuInt HandlerIndex)
	{
		return Unregister(HandlerIndex, Priority::Normal);
	}*/
	virtual void operator-=(EventHandle Handler)
	{
		Unregister(Handler);
	}

	nBool isCancelable() const
	{
		return m_Cancelable;
	}

	void SetCanceled(nBool cancel = true)
	{
		if (m_Cancelable)
		{
			m_isCanceled = cancel;
		}
	}

	nBool isCanceled() const
	{
		return m_isCanceled;
	}

	void Release()
	{
		m_EventHandler.clear();
		//m_isCanceled = true;
	}

	virtual nBool Activate(nInt PriorityLimitmin = Priority::Low, nInt PriorityLimitmax = Priority::High)
	{
		m_isCanceled = false;
		if (m_EventHandler.size() > 0u)
		{
			for (auto i = PriorityLimitmax; i <= PriorityLimitmin; ++i)
			{
				for (auto& eh : m_EventHandler[i])
				{
					eh(*this);
				}
			}
		}

		return m_isCanceled;
	}

	nBool operator()()
	{
		return Activate(Priority::Low, Priority::High);
	}
protected:
	std::map<nInt, std::unordered_set<EventHandle>> m_EventHandler;
	const nBool m_Cancelable;
	nBool m_isCanceled;
};