////////////////////////////////////////////////////////////////////////////////
///	@file	natLog.h
///	@brief	��־���
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include <fstream>

#include "natEvent.h"
#include "..\NatsuLib\natType.h"

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
	void LogMsg(nTString const& Msg, Arg &&... arg);

	///	@brief	��¼��Ϣ
	void LogMsg(nTString const& Msg);

	template <typename ...Arg>
	void LogErr(nTString const& Err, Arg &&... arg);

	///	@brief	��¼����
	void LogErr(nTString const& Err);

	template <typename ...Arg>
	void LogWarn(nTString const& Warn, Arg &&... arg);

	///	@brief	��¼����
	void LogWarn(nTString const& Warn);

	template <typename ...Arg>
	void Log(LogType type, nTString const& content, Arg &&... arg);

	///	@brief	��¼
	void Log(LogType type, nTString const& content);

	///	@brief	�����־�ļ���
	nTString GetLogFile() const;

	///	@brief	���������־
	nTString GetLastLog() const;

	///	@brief	ע����־�����¼�������
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