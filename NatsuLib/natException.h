////////////////////////////////////////////////////////////////////////////////
///	@file	natException.h
///	@brief	异常相关头文件
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "natType.h"
#include <Windows.h>

////////////////////////////////////////////////////////////////////////////////
///	@brief	NatsuLib异常基类
///	@note	异常由此类派生，请勿使用可能抛出异常的代码，使用pCausedby实现异常链
////////////////////////////////////////////////////////////////////////////////
class natException
{
public:
	natException(nTString const& Src, nTString const& Desc, const natException* pCausedby = nullptr);

	virtual ~natException() = default;

	nuInt GetTime() const;
	nTString const& GetSource() const;
	nTString const& GetDesc() const;
	const natException* GetCausedbyException() const;

protected:
	nuInt m_Time;
	nTString m_Source;
	nTString m_Description;
	const natException* m_pCausedby = nullptr;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	NatsuLib WinAPI调用异常
///	@note	会自动附加LastErr信息
////////////////////////////////////////////////////////////////////////////////
class natWinException
	: public natException
{
public:
	natWinException(nTString const& Src, nTString const& Desc);

	DWORD GetLastErr() const;

private:
	DWORD m_LastErr;
};