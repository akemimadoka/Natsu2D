////////////////////////////////////////////////////////////////////////////////
///	@file	natException.h
///	@brief	�쳣���ͷ�ļ�
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "natType.h"
#include <Windows.h>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
///	@brief	NatsuLib�쳣����
///	@note	�쳣�ɴ�������������ʹ�ÿ����׳��쳣�Ĵ���
////////////////////////////////////////////////////////////////////////////////
class natException
{
public:
	natException(ncTStr Src, ncTStr Desc, ...) noexcept;
	natException(ncTStr Src, ncTStr Desc, va_list vl) noexcept;
	virtual ~natException() = default;

	nuInt GetTime() const noexcept;
	ncTStr GetSource() const noexcept;
	ncTStr GetDesc() const noexcept;

protected:
	natException() noexcept = default;

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
	natWinException(ncTStr Src, ncTStr Desc, ...) noexcept;
	natWinException(ncTStr Src, DWORD LastErr, ncTStr Desc, ...) noexcept;

	DWORD GetErrNo() const noexcept;

private:
	DWORD m_LastErr;
};

#define nat_Throw(ExceptionClass, ...) do { throw ExceptionClass(_T(__FUNCTION__), __VA_ARGS__); } while (false)