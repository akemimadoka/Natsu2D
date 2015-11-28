#include "stdafx.h"
#include "natUtil.h"
#include "natException.h"

namespace natUtil
{
#	ifdef UNICODE
	enum { FIXCHAR = 1 };
#	else
	enum { FIXCHAR = 2 };
#	endif

	nTString FormatString(LPCTSTR lpStr, ...)
	{
		va_list vl;
		va_start(vl, lpStr);

		int nSize = lstrlen(lpStr) + 1, nLen;
		nTStr buf = nullptr;

		try
		{
			do
			{
				nSize *= 2;
				delete[] buf;
				buf = new nTChar[nSize];
				memset(buf, 0, nSize * sizeof(nTChar));
				nLen = _vsntprintf_s(buf, _TRUNCATE, nSize - 1, lpStr, vl);
			} while (nLen < 0 || nSize - nLen <= FIXCHAR);
		}
		catch (std::bad_alloc&)
		{
			throw natException(_T("n2dUtil::FormatString"), _T("Allocate memory failed"));
		}

		va_end(vl);

		nTString tmpstr(buf);
		delete[] buf;
		return tmpstr;
	}

	nTString GetResourceString(DWORD ResourceID, HINSTANCE hInstance)
	{
		int nSize = 16, nLen;
		nTStr buf = nullptr;

		try
		{
			do
			{
				nSize *= 2;
				delete[] buf;
				buf = new nTChar[nSize];
				memset(buf, 0, nSize * sizeof(nTChar));
				nLen = LoadString(hInstance, ResourceID, buf, nSize - 1);
			} while (nLen < 0 || nSize - nLen <= FIXCHAR);
		}
		catch (std::bad_alloc&)
		{
			throw natException(_T("n2dUtil::GetResourceString"), _T("Allocate memory failed"));
		}

		nTString tmpstr(buf);
		delete[] buf;
		return tmpstr;
	}

	std::vector<nByte> GetResourceData(DWORD ResourceID, LPCTSTR lpType, HINSTANCE hInstance)
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

		DWORD err = GetLastError();

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