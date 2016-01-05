#include "stdafx.h"
#include "natException.h"

natException::natException(ncTStr Src, ncTStr Desc, const natException* pCausedby)
	: m_Time(GetTickCount()), m_Source(Src), m_Description(Desc), m_pCausedby(pCausedby)
{
}

nuInt natException::GetTime() const
{
	return m_Time;
}

ncTStr natException::GetSource() const
{
	return m_Source.c_str();
}

ncTStr natException::GetDesc() const
{
	return m_Description.c_str();
}

const natException* natException::GetCausedbyException() const
{
	return m_pCausedby;
}

natWinException::natWinException(ncTStr Src, ncTStr Desc)
	: natException(Src, Desc, nullptr), m_LastErr(GetLastError())
{
	nTChar tErrnoStr[16] = { _T('\0') };
	_itot_s(m_LastErr, tErrnoStr, 10);
	m_Description = m_Description + _T(" ( LastErr=") + tErrnoStr + _T(" )");
}

DWORD natWinException::GetLastErr() const
{
	return m_LastErr;
}