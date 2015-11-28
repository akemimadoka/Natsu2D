////////////////////////////////////////////////////////////////////////////////
///	@file	event.h
///	@brief	Event��ʵ��
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <map>
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
	explicit natEvent(bool isEventCancelable = false)
		: m_Cancelable(isEventCancelable),
		m_isCanceled(false),
		m_data()
	{
	}
	explicit natEvent(dataT const& data, bool isEventCancelable)
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

	virtual nuInt Register(EventHandle func, int priority = Priority::Normal)
	{
		auto& eventhandlerlist = m_EventHandler[priority];
		eventhandlerlist.push_back(func);
		return eventhandlerlist.size() - 1u;
	}

	bool Unregister(nuInt HandlerIndex, int Priority)
	{
		auto eventhandlerlist = m_EventHandler.find(Priority);
		if (eventhandlerlist != m_EventHandler.end())
		{
			if (HandlerIndex >= eventhandlerlist->second.size())
			{
				return false;
			}

/*			auto i = eventhandlerlist->second.begin();
			for (uint id = HandlerIndex; id > 0u; --id)
			{
				++i;
			}

			eventhandlerlist->second.erase(i);*/
			eventhandlerlist->second.erase(eventhandlerlist->second.begin() + HandlerIndex);
			return true;
		}

		return false;
	}

	virtual void Unregister(EventHandle Handler)
	{
		for (auto& eventhandlerlist : m_EventHandler)
		{
			auto i = std::find(eventhandlerlist.second.begin(), eventhandlerlist.second.end(), Handler);
			eventhandlerlist.second.erase(i);
		}
	}

	virtual int operator+=(EventHandle func)
	{
		return Register(func);
	}

	bool operator-=(nuInt HandlerIndex)
	{
		return Unregister(HandlerIndex, Priority::Normal);
	}
	virtual void operator-=(EventHandle Handler)
	{
		Unregister(Handler);
	}

	bool isCancelable() const
	{
		return m_Cancelable;
	}

	void SetCanceled(bool cancel = true)
	{
		if (m_Cancelable)
		{
			m_isCanceled = cancel;
		}
	}

	bool isCanceled() const
	{
		return m_isCanceled;
	}

	virtual void Release()
	{
		m_EventHandler.clear();
		SetCanceled();
	}

	//__declspec(deprecated)
	virtual bool Activate(int PriorityLimitmin = Priority::Low, int PriorityLimitmax = Priority::High)
	{
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

	virtual bool Activate(dataType const& data, int PriorityLimitmin, int PriorityLimitmax)
	{
		SetData(data);
		return Activate(PriorityLimitmin, PriorityLimitmax);
	}

	bool operator()()
	{
		return Activate(Priority::Low, Priority::High);
	}

	bool operator()(dataType const& data)
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
	std::map<int, std::vector<EventHandle>> m_EventHandler;
	const bool m_Cancelable;
	bool m_isCanceled;

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

	explicit natEvent(bool isEventCancelable = false) : m_Cancelable(isEventCancelable), m_isCanceled(false)
	{
	}
	natEvent(natEvent const& b) : m_EventHandler(b.m_EventHandler), m_Cancelable(b.m_Cancelable), m_isCanceled(b.m_isCanceled)
	{
	}

	virtual ~natEvent()
	{
		Release();
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

	virtual nuInt Register(EventHandle func, int priority = Priority::Normal)
	{
		auto& eventhandlerlist = m_EventHandler[priority];
		eventhandlerlist.push_back(func);
		return eventhandlerlist.size() - 1u;
	}

	bool Unregister(nuInt HandlerIndex, int Priority)
	{
		auto eventhandlerlist = m_EventHandler.find(Priority);
		if (eventhandlerlist != m_EventHandler.end())
		{
			if (HandlerIndex >= eventhandlerlist->second.size())
			{
				return false;
			}

/*			auto i = eventhandlerlist->second.begin();
			for (; HandlerIndex > 0u; --HandlerIndex)
			{
				++i;
			}*/

			eventhandlerlist->second.erase(eventhandlerlist->second.begin() + HandlerIndex);
			return true;
		}

		return false;
	}

	virtual void Unregister(EventHandle Handler)
	{
		for (auto& eventhandlerlist : m_EventHandler)
		{
			auto i = std::find(eventhandlerlist.second.begin(), eventhandlerlist.second.end(), Handler);
			eventhandlerlist.second.erase(i);
		}
	}

	virtual int operator+=(EventHandle func)
	{
		return Register(func);
	}
	bool operator-=(nuInt HandlerIndex)
	{
		return Unregister(HandlerIndex, Priority::Normal);
	}
	virtual void operator-=(EventHandle Handler)
	{
		Unregister(Handler);
	}

	bool isCancelable() const
	{
		return m_Cancelable;
	}

	void SetCanceled(bool cancel = true)
	{
		if (m_Cancelable)
		{
			m_isCanceled = cancel;
		}
	}

	bool isCanceled() const
	{
		return m_isCanceled;
	}

	void Release()
	{
		m_EventHandler.clear();
		m_isCanceled = true;
	}

	virtual bool Activate(int PriorityLimitmin = Priority::Low, int PriorityLimitmax = Priority::High)
	{
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

	bool operator()()
	{
		return Activate(Priority::Low, Priority::High);
	}
protected:
	std::map<int, std::vector<EventHandle>> m_EventHandler;
	const bool m_Cancelable;
	bool m_isCanceled;
};