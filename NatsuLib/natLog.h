////////////////////////////////////////////////////////////////////////////////
///	@file	natLog.h
///	@brief	日志相关
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include <fstream>

#include "natEvent.h"
#include "..\NatsuLib\natType.h"

////////////////////////////////////////////////////////////////////////////////
///	@brief	日志类实现
///	@note	单例类，使用getInstance方法获得实例，非线程安全\n
///			默认使用全局变量global::Logfile设定日志文件名
///	@see	n2dGlobal::Logfile
////////////////////////////////////////////////////////////////////////////////
class natLog final
{
public:
	///	@brief	日志类型
	enum class LogType
	{
		Msg,	///< @brief	消息
		Err,	///< @brief	错误
		Warn	///< @brief	警告
	};

	///	@brief	获得实例
	static natLog& GetInstance();
	
	template <typename ...Arg>
	void LogMsg(nTString const& Msg, Arg &&... arg);

	///	@brief	记录信息
	void LogMsg(nTString const& Msg);

	template <typename ...Arg>
	void LogErr(nTString const& Err, Arg &&... arg);

	///	@brief	记录错误
	void LogErr(nTString const& Err);

	template <typename ...Arg>
	void LogWarn(nTString const& Warn, Arg &&... arg);

	///	@brief	记录警告
	void LogWarn(nTString const& Warn);

	template <typename ...Arg>
	void Log(LogType type, nTString const& content, Arg &&... arg);

	///	@brief	记录
	void Log(LogType type, nTString const& content);

	///	@brief	获得日志文件名
	nTString GetLogFile() const;

	///	@brief	获得最新日志
	nTString GetLastLog() const;

	///	@brief	注册日志更新事件处理函数
	void RegisterLogUpdateEventFunc(natEvent<nTString>::EventHandle func);
private:
	explicit natLog(nTString const& logfile);
	~natLog();

	nTString m_LogFile;
	std::basic_ofstream<nTChar> m_fstr;
	nTString m_LastLog;

	natEvent<nTString> EventLogUpdate;

	static ncWStr ParseLogType(LogType logtype);
};