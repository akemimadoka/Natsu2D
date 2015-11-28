////////////////////////////////////////////////////////////////////////////////
///	@file	natRefObj.h
///	@brief	���ü����������
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "natType.h"
#include <Windows.h>

////////////////////////////////////////////////////////////////////////////////
///	@brief	���ü����ӿ�
////////////////////////////////////////////////////////////////////////////////
struct natRefObj
{
	virtual ~natRefObj() = default;

	virtual void AddRef() = 0;
	virtual void Release() = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	���ü���ʵ��
///	@note	ʹ��ģ���ֹ���μ̳�
////////////////////////////////////////////////////////////////////////////////
template <typename T>
class natRefObjImpl
	: public T
{
public:
	natRefObjImpl()
		: m_cRef(1u)
	{}
	virtual ~natRefObjImpl() = default;

	virtual void AddRef()
	{
		InterlockedIncrement(&m_cRef);
	}

	virtual void Release()
	{
		auto tRet = InterlockedDecrement(&m_cRef);
		if (tRet == 0u)
		{
			delete this;
		}
	}
private:
	nuInt m_cRef;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	����ָ��ʵ��
///	@note	�����������ü�������
////////////////////////////////////////////////////////////////////////////////
template <typename T>
class natRefPointer final
{
public:
	natRefPointer()
		: m_pPointer(nullptr)
	{
	}
	natRefPointer(T* pObj)
		: m_pPointer(pObj)
	{
		if (m_pPointer)
		{
			m_pPointer->AddRef();
		}
	}

	natRefPointer(natRefPointer const& other)
		: m_pPointer(other.m_pPointer)
	{
		if (m_pPointer)
		{
			m_pPointer->AddRef();
		}
	}

	~natRefPointer()
	{
		SafeRelease(m_pPointer);
	}

	natRefPointer& RawSet(T* ptr)
	{
		if (ptr != m_pPointer)
		{
			SafeRelease(m_pPointer);
			m_pPointer = ptr;
		}

		return *this;
	}

	bool operator==(natRefPointer const& other) const
	{
		return (m_pPointer == other.m_pPointer);
	}

	natRefPointer& operator=(natRefPointer const& other)
	{
		if (m_pPointer != other.m_pPointer)
		{
			SafeRelease(m_pPointer);
			m_pPointer = other.m_pPointer;
			if (m_pPointer)
			{
				m_pPointer->AddRef();
			}
		}

		return *this;
	}

	T* operator->() const
	{
		return m_pPointer;
	}

	T& operator*() const
	{
		return *m_pPointer;
	}

	T** operator&()
	{
		return &m_pPointer;
	}

	operator T*() const
	{
		return m_pPointer;
	}

	template <typename P>
	operator natRefPointer<P>() const
	{
		return natRefPointer<P>(dynamic_cast<P*>(m_pPointer));
	}
private:
	T* m_pPointer;
};