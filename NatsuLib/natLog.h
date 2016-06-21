////////////////////////////////////////////////////////////////////////////////
///	@file	natLog.h
///	@brief	��־���
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include <fstream>

#include "natEvent.h"
#include "natType.h"
#include "natUtil.h"

////////////////////////////////////////////////////////////////////////////////
///	@brief	��־��ʵ��
///	@note	�����࣬ʹ��getInstance�������ʵ�������̰߳�ȫ\n
///			Ĭ��ʹ��ȫ�ֱ���global::Logfile�趨��־�ļ���
///	@see	n2dGlobal::Logfile
////////////////////////////////////////////////////////////////////////////////
class natLog final
{
public:
	class EventLogUpdated final
		: public natEventBase
	{
	public:
		explicit EventLogUpdated(ncTStr data)
			: m_Data(data)
		{
		}

		bool CanCancel() const noexcept override
		{
			return false;
		}

		ncTStr GetData() const noexcept
		{
			return m_Data;
		}

	private:
		ncTStr m_Data;
	};

	///	@brief	��־����
	enum class LogType
	{
		Msg,	///< @brief	��Ϣ
		Err,	///< @brief	����
		Warn	///< @brief	����
	};

	///	@brief	���ʵ��
	static natLog& GetInstance();
	
	///	@brief	��¼��Ϣ
	template <typename ...Arg>
	void LogMsg(ncTStr Msg, Arg &&... arg)
	{
		Log(LogType::Msg, Msg, std::forward<Arg>(arg)...);
	}

	///	@brief	��¼����
	template <typename ... Arg>
	void LogErr(ncTStr Err, Arg &&... arg)
	{
		Log(LogType::Err, Err, std::forward<Arg>(arg)...);
	}

	///	@brief	��¼����
	template <typename ... Arg>
	void LogWarn(ncTStr Warn, Arg &&... arg)
	{
		Log(LogType::Warn, Warn, std::forward<Arg>(arg)...);
	}

	///	@brief	��¼
	template <typename ... Arg>
	void Log(LogType type, ncTStr content, Arg &&... arg)
	{
		UpdateLastLog(std::move(natUtil::FormatString(natUtil::FormatString(_T("[%s] [%s] %s"), natUtil::GetSysTime().c_str(), ParseLogType(type), content), std::forward<Arg>(arg)...)));
	}

	///	@brief	�����־�ļ���
	ncTStr GetLogFile() const;

	///	@brief	���������־
	ncTStr GetLastLog() const;

	///	@brief	ע����־�����¼�������
	void RegisterLogUpdateEventFunc(natEventBus::EventListenerFunc func);
private:
	explicit natLog(ncTStr const& logfile);
	~natLog();

	void UpdateLastLog(nTString&& log);

	nTString m_LogFile;
	std::basic_ofstream<nTChar> m_fstr;
	nTString m_LastLog;

	static ncTStr ParseLogType(LogType logtype);
};