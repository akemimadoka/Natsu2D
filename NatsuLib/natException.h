////////////////////////////////////////////////////////////////////////////////
///	@file	natException.h
///	@brief	�쳣���ͷ�ļ�
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "natType.h"
#include <Windows.h>

////////////////////////////////////////////////////////////////////////////////
///	@brief	NatsuLib�쳣����
///	@note	�쳣�ɴ�������������ʹ�ÿ����׳��쳣�Ĵ��룬ʹ��pCausedbyʵ���쳣��
////////////////////////////////////////////////////////////////////////////////
class natException
{
public:
	natException(ncTStr Src, ncTStr Desc, const natException* pCausedby = nullptr);

	virtual ~natException() = default;

	nuInt GetTime() const;
	ncTStr GetSource() const;
	ncTStr GetDesc() const;
	const natException* GetCausedbyException() const;

protected:
	nuInt m_Time;
	nTString m_Source;
	nTString m_Description;
	const natException* m_pCausedby = nullptr;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	NatsuLib WinAPI�����쳣
///	@note	���Զ�����LastErr��Ϣ
////////////////////////////////////////////////////////////////////////////////
class natWinException
	: public natException
{
public:
	natWinException(ncTStr Src, ncTStr Desc);

	DWORD GetLastErr() const;

private:
	DWORD m_LastErr;
};