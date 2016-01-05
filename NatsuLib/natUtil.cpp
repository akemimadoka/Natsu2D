#include "stdafx.h"
#include "natUtil.h"
#include "natException.h"

namespace natUtil
{

	enum : nuInt
	{
		FIXCHAR = 
#	ifdef UNICODE
		1u
#	else
		2u
#	endif
	};

	nTString FormatString(ncTStr lpStr, ...)
	{
		va_list vl;
		va_start(vl, lpStr);

		return FormatStringv(lpStr, vl);
	}

	nTString FormatStringv(ncTStr lpStr, va_list vl)
	{
		int nLen;
		std::vector<nTChar> tBuf(lstrlen(lpStr) + 1u);

		try
		{
			do
			{
				tBuf.resize(tBuf.size() * 2u);
				nLen = _vsntprintf_s(tBuf.data(), _TRUNCATE, tBuf.size() - 1, lpStr, vl);
			} while (nLen < 0 || tBuf.size() - nLen <= FIXCHAR);
		}
		catch (std::bad_alloc&)
		{
			throw natException(_T("n2dUtil::FormatString"), _T("Allocate memory failed"));
		}

		return tBuf.data();
	}

	std::wstring MultibyteToUnicode(ncStr Str, nuInt CodePage)
	{
		nInt Num = MultiByteToWideChar(CodePage, 0, Str, -1, nullptr, 0);
		std::vector<nWChar> tBuffer(Num);
		MultiByteToWideChar(CodePage, 0, Str, -1, tBuffer.data(), Num);

		return tBuffer.data();
	}

	std::string WidecharToMultibyte(ncWStr Str, nuInt CodePage)
	{
		nInt Num = WideCharToMultiByte(CodePage, 0, Str, -1, nullptr, 0, nullptr, FALSE);
		std::vector<nChar> tBuffer(Num);
		WideCharToMultiByte(CodePage, 0, Str, -1, tBuffer.data(), Num, nullptr, FALSE);

		return tBuffer.data();
	}

	nTString GetResourceString(DWORD ResourceID, HINSTANCE hInstance)
	{
		int nLen;
		std::vector<nTChar> tBuf(16u);

		try
		{
			do
			{
				tBuf.resize(tBuf.size() * 2u);
				nLen = LoadString(hInstance, ResourceID, tBuf.data(), tBuf.size() - 1u);
			} while (nLen < 0 || tBuf.size() - nLen <= FIXCHAR);
		}
		catch (std::bad_alloc&)
		{
			throw natException(_T("n2dUtil::GetResourceString"), _T("Allocate memory failed"));
		}

		return tBuf.data();
	}

	std::vector<nByte> GetResourceData(DWORD ResourceID, ncTStr lpType, HINSTANCE hInstance)
	{
		HRSRC hRsrc = FindResource(hInstance, MAKEINTRESOURCE(ResourceID), lpType);
		if (hRsrc != NULL)
		{
			DWORD dwSize = SizeofResource(hInstance, hRsrc);
			if (dwSize != 0ul)
			{
				HGLOBAL hGlobal = LoadResource(hInstance, hRsrc);
				if (hGlobal != NULL)
				{
					nData pBuffer = static_cast<nData>(LockResource(hGlobal));
					if (pBuffer != nullptr)
					{
						return std::vector<nByte>(pBuffer, pBuffer + dwSize);
					}
				}
			}
		}

		return std::vector<nByte>();
	}

	// 分割字符串实现
	template <typename T>
	std::vector<T> _splithelper(T str, T const& pattern)
	{
		typename T::size_type pos;
		std::vector<T> result;
		str += pattern;//扩展字符串以方便操作
		auto size = str.size();

		for (size_t i = 0u; i<size; ++i)
		{
			pos = str.find(pattern, i);
			if (pos < size)
			{
				result.push_back(str.substr(i, pos - i));
				i = pos + pattern.size() - 1;
			}
		}
		return result;
	}

	std::vector<std::string> split(std::string const& str, std::string const& pattern)
	{
		return _splithelper(str, pattern);
	}

	std::vector<std::wstring> split(std::wstring const& str, std::wstring const& pattern)
	{
		return _splithelper(str, pattern);
	}
}