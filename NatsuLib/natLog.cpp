#include "stdafx.h"
#include "natLog.h"
#include "natUtil.h"

//#include <boost\format.hpp>

#pragma warning (disable:4996) 

///	@brief	全局变量
namespace n2dGlobal
{
	///	@brief	默认日志文件名
	ncTStr Logfile = _T("Log.log");
}

natLog::natLog(nTString const& logfile)
	: m_LogFile(logfile),
	m_fstr(m_LogFile)
{
	LogMsg(_T("Log start."));
}

natLog::~natLog()
{
	m_fstr.close();
}

ncWStr natLog::ParseLogType(LogType logtype)
{
	switch (logtype)
	{
	case LogType::Msg:
		return L"Message";
	case LogType::Err:
		return L"Error";
	case LogType::Warn:
		return L"Warning";
	default:
		return L"Unknown";
	}
}

NATNOINLINE natLog& natLog::GetInstance()
{
	static natLog instance(n2dGlobal::Logfile);
	return instance;
}

template <typename ...Arg>
void natLog::LogMsg(nTString const & Msg, Arg &&... arg)
{
	Log(LogType::Msg, Msg, std::forward<Arg>(arg)...);
}

void natLog::LogMsg(nTString const& Msg)
{
	Log(LogType::Msg, Msg);
}

template <typename ... Arg>
void natLog::LogErr(nTString const& Err, Arg &&... arg)
{
	Log(LogType::Err, Err, std::forward<Arg>(arg)...);
}

void natLog::LogErr(nTString const & Err)
{
	Log(LogType::Err, Err);
}

template <typename ... Arg>
void natLog::LogWarn(nTString const& Warn, Arg &&... arg)
{
	Log(LogType::Warn, Warn, std::forward<Arg>(arg)...);
}

void natLog::LogWarn(nTString const & Warn)
{
	Log(LogType::Warn, Warn);
}

template <typename ... Arg>
void natLog::Log(LogType type, nTString const& content, Arg &&... arg)
{
	m_LastLog = natUtil::FormatString(natUtil::FormatString(_T("[%s] [%s] %s"), natUtil::GetSysTime().c_str(), ParseLogType(type), content.c_str()).c_str(), std::forward<Arg>(arg)...);
	m_fstr << GetLastLog() << std::endl;
	EventLogUpdate(GetLastLog());
}

void natLog::Log(LogType type, nTString const& content)
{
	m_LastLog = natUtil::FormatString(_T("[%s] [%s] %s"), natUtil::GetSysTime().c_str(), ParseLogType(type), content.c_str());
	m_fstr << GetLastLog() << std::endl;
	EventLogUpdate(GetLastLog());
}

nTString natLog::GetLogFile() const
{
	return m_LogFile;
}

nTString natLog::GetLastLog() const
{
	return m_LastLog;
}

void natLog::RegisterLogUpdateEventFunc(natEvent<nTString>::EventHandle func)
{
	EventLogUpdate += func;
}
