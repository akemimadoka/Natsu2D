////////////////////////////////////////////////////////////////////////////////
///	@file	natMultiThread.h
///	@brief	���߳�֧��
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <Windows.h>

////////////////////////////////////////////////////////////////////////////////
///	@addtogroup	ϵͳ�ײ�֧��
///	@brief		�ṩ�漰ϵͳ�ں˲�����֧��
///	@{

////////////////////////////////////////////////////////////////////////////////
///	@brief	�̻߳���
///	@note	��̳и��ಢ��дThreadJob��ʵ�ֶ��߳�\n
///			��ȷ���߳�����ִ���ڼ䱣����Ч
////////////////////////////////////////////////////////////////////////////////
class natThread
{
public:
	///	@brief	���캯��
	///	@param[in]	Pause	�����߳�ʱ��ͣ
	explicit natThread(nBool Pause = true);
	virtual ~natThread();

	///	@brief		����߳̾��
	///	@warning	�����ֶ�ɾ��
	HANDLE GetHandle() const;

	///	@brief	�����߳�ִ��
	///	@return	�Ƿ�ɹ�
	nBool Resume();

	///	@brief	�����߳�ִ��
	///	@return	�Ƿ�ɹ�
	nBool Suspend();

	///	@brief	�ȴ��߳�ִ��
	///	@param[in]	WaitTime	�ȴ�ʱ��
	///	@return	�ȴ��߳�״̬
	DWORD Wait(nuInt WaitTime = INFINITE);

	///	@brief	�����߳�
	///	@param[in]	ExitCode	�˳���
	///	@return	�Ƿ�ɹ�
	nBool Terminate(nuInt ExitCode = nuInt(-1));

	///	@brief	����˳���
	nuInt GetExitCode() const;
protected:
	///	@brief	��д�˷�����ʵ���̹߳���
	virtual nuInt ThreadJob() = 0;

private:
	///	@brief	ִ���̵߳İ�װ����
	///	@param[in]	p	ָ��Thread���ָ��
	static DWORD WINAPI execute(void* p);
	HANDLE m_hThread;
	DWORD m_hThreadID;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	�ٽ���
///	@note	���ڿ���Ϊ��������Խ�����̳߳�ͻ
////////////////////////////////////////////////////////////////////////////////
class natCriticalSection final
{
public:
	natCriticalSection();
	~natCriticalSection();

	///	@brief	�����ٽ���
	///	@note	������������
	void Lock();
	///	@brief	���������ٽ���
	///	@note	���������߳�
	///	@return	�Ƿ�ɹ�
	nBool TryLock();
	///	@brief	�����ٽ���
	void UnLock();
private:
	CRITICAL_SECTION m_Section;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	Windows��Event��װ��
///	@note	ͨ��Event�Զ��̲߳�������ͬ��\n
///			���뱾�����Event����
////////////////////////////////////////////////////////////////////////////////
class natEventWrapper final
{
public:
	natEventWrapper(nBool AutoReset, nBool InitialState);
	~natEventWrapper();

	///	@brief		��þ��
	///	@warning	�����ֶ��ͷ�
	HANDLE GetHandle() const;

	///	@brief		����¼�
	///	@return		�Ƿ�ɹ�
	nBool Set();

	///	@brief		ȡ���¼����
	///	@return		�Ƿ�ɹ�
	nBool Reset();

	///	@brief		�¼�����
	///	@return		�Ƿ�ɹ�
	nBool Pulse() const;

	///	@brief		�ȴ��¼�
	///	@param[in]	WaitTime	�ȴ�ʱ��
	///	@return		�Ƿ�ɹ�
	nBool Wait(nuInt WaitTime = INFINITE) const;
private:
	HANDLE m_hEvent;
};

///	@}