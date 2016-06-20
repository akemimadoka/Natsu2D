#pragma once
#include "natType.h"
#include <Windows.h>
#include <vector>

namespace natUtil
{
	///	@brief		��ʽ���ַ���
	///	@warning	�����Ͱ�ȫ
	///	@param[in]	str		�ַ�����ʽ
	nTString FormatString(ncTStr lpStr, ...);
	nTString FormatStringv(ncTStr lpStr, const va_list vl);

	///	@brief	��ñ���ʱ��
	///	@return	����ʱ����Ϣ���ַ���
	inline nTString GetSysTime()
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		return FormatString(_T("%04d/%02d/%02d %02d:%02d:%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
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
	
	///	@brief	���ֽ�תUnicode
	std::wstring MultibyteToUnicode(ncStr Str, nuInt CodePage = CP_INSTALLED);
	///	@brief	���ַ�ת���ֽ�
	std::string WidecharToMultibyte(ncWStr Str, nuInt CodePage = CP_INSTALLED);

	///	@brief	�����Դ�ַ���
	///	@param[in]	ResourceID	��ԴID
	///	@param[in]	hInstance	ʵ�������Ĭ��ΪNULL
	nTString GetResourceString(DWORD ResourceID, HINSTANCE hInstance = NULL);

	///	@brief	�����Դ����
	///	@param[in]	ResourceID	��ԴID
	///	@param[in]	lpType		��Դ����
	///	@param[in]	hInstance	ʵ�������Ĭ��ΪNULL
	std::vector<nByte> GetResourceData(DWORD ResourceID, ncTStr lpType, HINSTANCE hInstance = NULL);

	///	@brief	�ַ����ָ��
	///	@param[in]	str			Ҫ�ָ���ַ���
	///	@param[in]	pattern		�ָ��ַ�
	///	@param[out]	container	�洢�����������Ӧʵ��emplace_back�����ܲ���Ϊ �ַ���, �ַ���ʼλ��, �ַ�������
	template <typename Char_T, typename Container>
	void split(std::basic_string<Char_T> const& str, std::basic_string<Char_T> const& pattern, Container& container)
	{
		typedef typename std::basic_string<Char_T>::size_type pos_t;
		const auto size = str.size();

		pos_t pos = 0;

		for (pos_t i = 0u; i < size; ++i)
		{
			auto currentchar = str[i];
			for (auto c : pattern)
			{
				if (currentchar == c)
				{
					container.emplace_back(str, pos, i - pos);

					pos = i + 1;
					break;
				}
			}
		}

		if (pos != size)
		{
			container.emplace_back(str, pos, size - pos);
		}
	}
}