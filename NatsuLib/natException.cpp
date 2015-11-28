#include "stdafx.h"
#include "natException.h"

natException::natException(nTString const& Src, nTString const& Desc, const natException* pCausedby)
	: m_Time(GetTickCount()), m_Source(Src), m_Description(Desc), m_pCausedby(pCausedby)
{
}

nuInt natException::GetTime() const
{
	return m_Time;
}

nTString const& natException::GetSource() const
{
	return m_Source;
}

nTString const& natException::GetDesc() const
{
	return m_Description;
}

const natException* natException::GetCausedbyException() const
{
	return m_pCausedby;
}

natWinException::natWinException(nTString const& Src, nTString const& Desc)
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