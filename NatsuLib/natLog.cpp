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

natLog::natLog(ncTStr const& logfile)
	: m_LogFile(logfile),
	m_fstr(m_LogFile)
{
	LogMsg(_T("Log start."));
}

natLog::~natLog()
{
	m_fstr.close();
}

ncTStr natLog::ParseLogType(LogType logtype)
{
	switch (logtype)
	{
	case LogType::Msg:
		return _T("Message");
	case LogType::Err:
		return _T("Error");
	case LogType::Warn:
		return _T("Warning");
	default:
		return _T("Unknown");
	}
}

NATNOINLINE natLog& natLog::GetInstance()
{
	static natLog instance(n2dGlobal::Logfile);
	return instance;
}

template <typename ...Arg>
void natLog::LogMsg(ncTStr Msg, Arg &&... arg)
{
	Log(LogType::Msg, Msg, std::forward<Arg>(arg)...);
}

void natLog::LogMsg(ncTStr Msg)
{
	Log(LogType::Msg, Msg);
}

template <typename ... Arg>
void natLog::LogErr(ncTStr Err, Arg &&... arg)
{
	Log(LogType::Err, Err, std::forward<Arg>(arg)...);
}

void natLog::LogErr(ncTStr Err)
{
	Log(LogType::Err, Err);
}

template <typename ... Arg>
void natLog::LogWarn(ncTStr Warn, Arg &&... arg)
{
	Log(LogType::Warn, Warn, std::forward<Arg>(arg)...);
}

void natLog::LogWarn(ncTStr Warn)
{
	Log(LogType::Warn, Warn);
}

template <typename ... Arg>
void natLog::Log(LogType type, ncTStr content, Arg &&... arg)
{
	m_LastLog = natUtil::FormatString(natUtil::FormatString(_T("[%s] [%s] %s"), natUtil::GetSysTime().c_str(), ParseLogType(type), content).c_str(), std::forward<Arg>(arg)...);
	m_fstr << GetLastLog() << std::endl;
	EventLogUpdate(GetLastLog());
}

void natLog::Log(LogType type, ncTStr content)
{
	m_LastLog = natUtil::FormatString(_T("[%s] [%s] %s"), natUtil::GetSysTime().c_str(), ParseLogType(type), content).c_str();
	m_fstr << GetLastLog() << std::endl;
	EventLogUpdate(GetLastLog());
}

ncTStr natLog::GetLogFile() const
{
	return m_LogFile.c_str();
}

ncTStr natLog::GetLastLog() const
{
	return m_LastLog.c_str();
}

void natLog::RegisterLogUpdateEventFunc(natEvent<ncTStr>::EventHandle func)
{
	EventLogUpdate += func;
}
