#include "stdafx.h"
#include "natException.h"
#include "natUtil.h"

natException::natException(ncTStr Src, ncTStr Desc, ...) noexcept
	: m_Time(GetTickCount()), m_Source(Src)
{
	va_list vl;
	va_start(vl, Desc);
	m_Description = natUtil::FormatStringv(Desc, vl);
	va_end(vl);
}

natException::natException(ncTStr Src, ncTStr Desc, va_list vl) noexcept
	: m_Time(GetTickCount()), m_Source(Src), m_Description(natUtil::FormatStringv(Desc, vl))
{
}

nuInt natException::GetTime() const noexcept
{
	return m_Time;
}

ncTStr natException::GetSource() const noexcept
{
	return m_Source.c_str();
}

ncTStr natException::GetDesc() const noexcept
{
	return m_Description.c_str();
}

natWinException::natWinException(ncTStr Src, ncTStr Desc, ...) noexcept
	: m_LastErr(GetLastError())
{
	m_Time = GetTickCount();
	m_Source = Src;
	va_list vl;
	va_start(vl, Desc);
	m_Description = natUtil::FormatStringv(Desc, vl) + natUtil::FormatString(_T(" ( LastErr = %d)"), m_LastErr);
	va_end(vl);
}

natWinException::natWinException(ncTStr Src, DWORD LastErr, ncTStr Desc, ...) noexcept
	: m_LastErr(LastErr)
{
	m_Time = GetTickCount();
	m_Source = Src;
	va_list vl;
	va_start(vl, Desc);
	m_Description = natUtil::FormatStringv(Desc, vl) + natUtil::FormatString(_T(" ( LastErr = %d)"), m_LastErr);
	va_end(vl);
}

DWORD natWinException::GetErrNo() const noexcept
{
	return m_LastErr;
}