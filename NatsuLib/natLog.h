////////////////////////////////////////////////////////////////////////////////
///	@file	natLog.h
///	@brief	��־���
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include <fstream>

#include "natEvent.h"
#include "natType.h"

////////////////////////////////////////////////////////////////////////////////
///	@brief	��־��ʵ��
///	@note	�����࣬ʹ��getInstance�������ʵ�������̰߳�ȫ\n
///			Ĭ��ʹ��ȫ�ֱ���global::Logfile�趨��־�ļ���
///	@see	n2dGlobal::Logfile
////////////////////////////////////////////////////////////////////////////////
class natLog final
{
public:
	///	@brief	��־����
	enum class LogType
	{
		Msg,	///< @brief	��Ϣ
		Err,	///< @brief	����
		Warn	///< @brief	����
	};

	///	@brief	���ʵ��
	static natLog& GetInstance();
	
	template <typename ...Arg>
	void LogMsg(ncTStr Msg, Arg &&... arg);

	///	@brief	��¼��Ϣ
	void LogMsg(ncTStr Msg);

	template <typename ...Arg>
	void LogErr(ncTStr Err, Arg &&... arg);

	///	@brief	��¼����
	void LogErr(ncTStr Err);

	template <typename ...Arg>
	void LogWarn(ncTStr Warn, Arg &&... arg);

	///	@brief	��¼����
	void LogWarn(ncTStr Warn);

	template <typename ...Arg>
	void Log(LogType type, ncTStr content, Arg &&... arg);

	///	@brief	��¼
	void Log(LogType type, ncTStr content);

	///	@brief	�����־�ļ���
	ncTStr GetLogFile() const;

	///	@brief	���������־
	ncTStr GetLastLog() const;

	///	@brief	ע����־�����¼�������
	void RegisterLogUpdateEventFunc(natEvent<ncTStr>::EventHandle func);
private:
	explicit natLog(ncTStr const& logfile);
	~natLog();

	nTString m_LogFile;
	std::basic_ofstream<nTChar> m_fstr;
	nTString m_LastLog;

	natEvent<ncTStr> EventLogUpdate;

	static ncTStr ParseLogType(LogType logtype);
};