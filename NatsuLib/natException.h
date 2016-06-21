////////////////////////////////////////////////////////////////////////////////
///	@file	natException.h
///	@brief	�쳣���ͷ�ļ�
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "natType.h"
#include <Windows.h>

namespace natUtil
{
	template <typename... Args>
	nTString FormatString(ncTStr lpStr, Args&&... args);
	inline std::wstring C2Wstr(std::string const& str);
	inline std::string W2Cstr(std::wstring const& str);
}

////////////////////////////////////////////////////////////////////////////////
///	@brief	NatsuLib�쳣����
///	@note	�쳣�ɴ�������������ʹ�ÿ����׳��쳣�Ĵ���
////////////////////////////////////////////////////////////////////////////////
class natException
	: public std::exception
{
public:
	template <typename... Args>
	natException(ncTStr Src, ncTStr Desc, Args&&... args) noexcept
		: exception(natUtil::W2Cstr(natUtil::FormatString(Desc, std::forward<Args>(args)...)).c_str()), m_Time(GetTickCount()), m_Source(Src), m_Description(natUtil::C2Wstr(exception::what()))
	{
	}

	virtual ~natException() = default;

	nuInt GetTime() const noexcept
	{
		return m_Time;
	}

	ncTStr GetSource() const noexcept
	{
		return m_Source.c_str();
	}

	ncTStr GetDesc() const noexcept
	{
		return m_Description.c_str();
	}

protected:
	nuInt m_Time;
	nTString m_Source;
	nTString m_Description;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	NatsuLib WinAPI�����쳣
///	@note	�����Զ�����LastErr��Ϣ
////////////////////////////////////////////////////////////////////////////////
class natWinException
	: public natException
{
public:
	template <typename... Args>
	natWinException(ncTStr Src, ncTStr Desc, Args&&... args) noexcept
		: natWinException(Src, GetLastError(), Desc, std::forward<Args>(args)...)
	{
	}

	template <typename... Args>
	natWinException(ncTStr Src, DWORD LastErr, ncTStr Desc, Args&&... args) noexcept
		: natException(Src, Desc, std::forward<Args>(args)...), m_LastErr(LastErr)
	{
		m_Description = natUtil::FormatString((m_Description + _T(" (LastErr = %d)")).c_str(), m_LastErr);
	}

	DWORD GetErrNo() const noexcept
	{
		return m_LastErr;
	}

private:
	DWORD m_LastErr;
};

#define nat_Throw(ExceptionClass, ...) do { throw ExceptionClass(_T(__FUNCTION__), __VA_ARGS__); } while (false)