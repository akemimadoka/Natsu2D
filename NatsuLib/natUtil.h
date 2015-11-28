#pragma once
#include "natType.h"
#include <Windows.h>
#include <vector>

namespace natUtil
{
	///	@brief		��ʽ���ַ���
	///	@warning	�����Ͱ�ȫ
	///	@param[in]	str		�ַ�����ʽ
	nTString FormatString(LPCTSTR str, ...);

	///	@brief	��ñ���ʱ��
	///	@return	����ʱ����Ϣ���ַ���
	inline nTString GetSysTime()
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		return FormatString(_T("%d/%d/%d %d:%d:%d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	}

	///	@brief	stringתwstring
	inline std::wstring C2Wstr(std::string const& str)
	{
		return std::wstring(str.begin(), str.end());
	}
	///	@brief	wstringתstring
	inline std::string W2Cstr(std::wstring const& str)
	{
		return std::string(str.begin(), str.end());
	}

	///	@brief	�����Դ�ַ���
	///	@param[in]	ResourceID	��ԴID
	///	@param[in]	hInstance	ʵ�������Ĭ��Ϊ��ǰ���
	nTString GetResourceString(DWORD ResourceID, HINSTANCE hInstance = NULL);

	std::vector<nByte> GetResourceData(DWORD ResourceID, LPCTSTR lpType, HINSTANCE hInstance = NULL);

	///	@brief	�ַ����ָ��
	///	@note	char��
	///	@param[in]	str		Ҫ�ָ���ַ���
	///	@param[in]	pattern	�ָ��ַ�
	///	@return	�ָ�����Ϊstd::vector
	std::vector<std::string> split(std::string const& str, std::string const& pattern);

	///	@brief	�ַ����ָ��
	///	@note	wchar��
	///	@param[in]	str		Ҫ�ָ���ַ���
	///	@param[in]	pattern	�ָ��ַ�
	///	@return	�ָ�����Ϊstd::vector
	std::vector<std::wstring> split(std::wstring const& str, std::wstring const& pattern);
}