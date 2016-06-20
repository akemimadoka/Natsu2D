////////////////////////////////////////////////////////////////////////////////
///	@file	natException.h
///	@brief	异常相关头文件
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "natType.h"
#include <Windows.h>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
///	@brief	NatsuLib异常基类
///	@note	异常由此类派生，请勿使用可能抛出异常的代码
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
///	@brief	NatsuLib WinAPI调用异常
///	@note	可以自动附加LastErr信息
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