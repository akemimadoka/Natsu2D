#pragma once
#include "natType.h"
#include <Windows.h>
#include <vector>

namespace natUtil
{
	///	@brief		格式化字符串
	///	@warning	非类型安全
	///	@param[in]	str		字符串格式
	nTString FormatString(ncTStr lpStr, ...);
	nTString FormatStringv(ncTStr lpStr, va_list vl);

	///	@brief	获得本地时间
	///	@return	包含时间信息的字符串
	inline nTString GetSysTime()
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		return FormatString(_T("%d/%d/%d %d:%d:%d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	}

	///	@brief	string转wstring
	inline std::wstring C2Wstr(std::string const& str)
	{
		return std::wstring(str.begin(), str.end());
	}
	///	@brief	wstring转string
	inline std::string W2Cstr(std::wstring const& str)
	{
		return std::string(str.begin(), str.end());
	}
	
	///	@brief	多字节转Unicode
	std::wstring MultibyteToUnicode(ncStr Str, nuInt CodePage = CP_INSTALLED);
	///	@brief	宽字符转多字节
	std::string WidecharToMultibyte(ncWStr Str, nuInt CodePage = CP_INSTALLED);

	///	@brief	获得资源字符串
	///	@param[in]	ResourceID	资源ID
	///	@param[in]	hInstance	实例句柄，默认为NULL
	nTString GetResourceString(DWORD ResourceID, HINSTANCE hInstance = NULL);

	///	@brief	获得资源数据
	///	@param[in]	ResourceID	资源ID
	///	@param[in]	lpType		资源类型
	///	@param[in]	hInstance	实例句柄，默认为NULL
	std::vector<nByte> GetResourceData(DWORD ResourceID, ncTStr lpType, HINSTANCE hInstance = NULL);

	///	@brief	字符串分割函数
	///	@note	char版
	///	@param[in]	str		要分割的字符串
	///	@param[in]	pattern	分割字符
	///	@return	分割结果，为std::vector
	std::vector<std::string> split(std::string const& str, std::string const& pattern);

	///	@brief	字符串分割函数
	///	@note	wchar版
	///	@param[in]	str		要分割的字符串
	///	@param[in]	pattern	分割字符
	///	@return	分割结果，为std::vector
	std::vector<std::wstring> split(std::wstring const& str, std::wstring const& pattern);
}