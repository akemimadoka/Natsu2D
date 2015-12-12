#include "stdafx.h"
#include "natStream.h"
#include "natException.h"
#include "natUtil.h"

natFileStream::natFileStream(ncTStr lpFilename, nBool bReadable, nBool bWritable)
	: m_Filename(lpFilename), m_bReadable(bReadable), m_bWritable(bWritable), m_LastErr(NatErr_OK)
{
	m_hFile = CreateFile(
		lpFilename,
		(bReadable ? GENERIC_READ : 0) | (bWritable ? GENERIC_WRITE : 0),
		FILE_SHARE_READ,
		NULL,
		bWritable ? OPEN_ALWAYS : OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		throw natException(_T("natFileStream::natFileStream"), natUtil::FormatString(_T("Open file \"%s\" failed"), lpFilename));
	}
}

NatErr natFileStream::GetLastErr() const
{
	return m_LastErr;
}

nBool natFileStream::CanWrite() const
{
	return m_bWritable;
}

nBool natFileStream::CanRead() const
{
	return m_bReadable;
}

nBool natFileStream::CanResize() const
{
	return m_bWritable;
}

nLen natFileStream::GetSize() const
{
	return GetFileSize(m_hFile, NULL);
}

nResult natFileStream::SetSize(nLen Size)
{
	if (!m_bWritable)
	{
		return m_LastErr = NatErr_IllegalState;
	}

	LARGE_INTEGER tVal;
	tVal.QuadPart = Size;

	if (SetFilePointer(m_hFile, tVal.LowPart, &tVal.HighPart, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		return m_LastErr = NatErr_InternalErr;
	}

	if (SetEndOfFile(m_hFile) == FALSE)
	{
		return m_LastErr = NatErr_InternalErr;
	}

	return m_LastErr = static_cast<NatErr>(SetPosition(NatSeek::Beg, 0l));
}

nLen natFileStream::GetPosition() const
{
	LARGE_INTEGER tVal;
	tVal.QuadPart = 0;

	tVal.LowPart = SetFilePointer(m_hFile, tVal.LowPart, &tVal.HighPart, FILE_CURRENT);
	return tVal.QuadPart;
}

nResult natFileStream::SetPosition(NatSeek Origin, nLong Offset)
{
	nuInt tOrigin;
	switch (Origin)
	{
	case NatSeek::Beg:
		tOrigin = FILE_BEGIN;
		break;
	case NatSeek::Cur:
		tOrigin = FILE_CURRENT;
		break;
	case NatSeek::End:
		tOrigin = FILE_END;
		break;
	default:
		return m_LastErr = NatErr_InvalidArg;
	}

	LARGE_INTEGER tVal;
	tVal.QuadPart = Offset;

	if (SetFilePointer(m_hFile, tVal.LowPart, &tVal.HighPart, tOrigin) == INVALID_SET_FILE_POINTER)
	{
		return m_LastErr = NatErr_InternalErr;
	}

	return m_LastErr = NatErr_OK;
}

nLen natFileStream::ReadBytes(nData pData, nLen Length)
{
	DWORD tReadBytes = 0ul;
	if (!m_bReadable)
	{
		m_LastErr = NatErr_IllegalState;
		return tReadBytes;
	}

	if (Length == 0ul)
	{
		return tReadBytes;
	}

	if (pData == nullptr)
	{
		m_LastErr = NatErr_InvalidArg;
		return tReadBytes;
	}

	if (ReadFile(m_hFile, pData, static_cast<DWORD>(Length), static_cast<LPDWORD>(&tReadBytes), NULL) == FALSE)
	{
		m_LastErr = NatErr_InternalErr;
		return tReadBytes;
	}

	if (tReadBytes != Length)
	{
		m_LastErr = NatErr_OutOfRange;
	}

	return tReadBytes;
}

nLen natFileStream::WriteBytes(ncData pData, nLen Length)
{
	DWORD tWriteBytes = 0ul;
	if (!m_bWritable)
	{
		m_LastErr = NatErr_IllegalState;
		return tWriteBytes;
	}

	if (Length == 0ul)
	{
		return tWriteBytes;
	}

	if (pData == nullptr)
	{
		m_LastErr = NatErr_InvalidArg;
		return tWriteBytes;
	}

	if (WriteFile(m_hFile, pData, static_cast<DWORD>(Length), &tWriteBytes, NULL) == FALSE)
	{
		m_LastErr = NatErr_InternalErr;
		return tWriteBytes;
	}

	if (tWriteBytes != Length)
	{
		m_LastErr = NatErr_OutOfRange;
	}

	return tWriteBytes;
}

void natFileStream::Lock()
{
	m_Section.Lock();
}

nResult natFileStream::TryLock()
{
	return m_LastErr = m_Section.TryLock() ? NatErr_OK : NatErr_IllegalState;
}

void natFileStream::Unlock()
{
	m_Section.UnLock();
}

nTString natFileStream::GetFilename() const
{
	return m_Filename;
}

natFileStream::~natFileStream()
{
	CloseHandle(m_hFile);
}

natMemoryStream::natMemoryStream(ncData pData, nLen Length, nBool bReadable, nBool bWritable, nBool bResizable)
	: m_pData(nullptr), m_Length(0ul), m_CurPos(0u), m_bReadable(bReadable), m_bWritable(bWritable), m_bResizable(bResizable)
{
	try
	{
		m_pData = new nByte[static_cast<nuInt>(Length)];
		m_Length = Length;
		if (pData)
		{
			memcpy_s(m_pData, static_cast<rsize_t>(Length), pData, static_cast<rsize_t>(Length));
		}
		else
		{
			memset(m_pData, 0, static_cast<size_t>(m_Length));
		}
	}
	catch (std::bad_alloc&)
	{
		throw natException(_T("natMemoryStream::natMemoryStream"), _T("Failed to allocate memory"));
	}
}

NatErr natMemoryStream::GetLastErr() const
{
	return m_LastErr;
}

nBool natMemoryStream::CanWrite() const
{
	return m_bWritable;
}

nBool natMemoryStream::CanRead() const
{
	return m_bReadable;
}

nBool natMemoryStream::CanResize() const
{
	return m_bResizable;
}

nLen natMemoryStream::GetSize() const
{
	return m_Length;
}

nResult natMemoryStream::SetSize(nLen Size)
{
	if (!m_bResizable)
	{
		return m_LastErr = NatErr_IllegalState;
	}

	if (Size == m_Length)
	{
		return m_LastErr = NatErr_OK;
	}

	nData tpData;
	try
	{
		tpData = new nByte[static_cast<nuInt>(Size)];
	}
	catch (std::bad_alloc&)
	{
		return m_LastErr = NatErr_InternalErr;
	}

	for (size_t i = 0u; i < Size; ++i)
	{
		tpData[i] = i < m_Length ? m_pData[i] : nByte(0u);
	}

	SafeDelArr(m_pData);
	m_pData = tpData;
	m_Length = Size;

	if (m_CurPos > m_Length)
	{
		m_CurPos = m_Length;
	}

	return m_LastErr = NatErr_OK;
}

nLen natMemoryStream::GetPosition() const
{
	return m_CurPos;
}

nResult natMemoryStream::SetPosition(NatSeek Origin, nLong Offset)
{
	switch (Origin)
	{
	case NatSeek::Beg:
		if (m_Length - Offset < 0 || Offset < 0)
			return m_LastErr = NatErr_InvalidArg;
		m_CurPos = Offset;
		break;
	case NatSeek::Cur:
		if (m_Length - m_CurPos - Offset > 0 || m_CurPos + Offset < 0)
			return m_LastErr = NatErr_InvalidArg;
		m_CurPos += Offset;
		break;
	case NatSeek::End:
		if (Offset > 0 || m_Length + Offset < 0)
			return m_LastErr = NatErr_InvalidArg;
		m_CurPos = m_Length + Offset;
		break;
	default:
		return m_LastErr = NatErr_InvalidArg;
	}

	return m_LastErr = NatErr_OK;
}

nLen natMemoryStream::ReadBytes(nData pData, nLen Length)
{
	nLen tReadBytes = 0ul;

	if (!m_bReadable)
	{
		m_LastErr = NatErr_IllegalState;
		return tReadBytes;
	}

	if (Length == 0ul)
	{
		m_LastErr = NatErr_OK;
		return tReadBytes;
	}

	if (pData == nullptr)
	{
		m_LastErr = NatErr_InvalidArg;
		return tReadBytes;
	}

	tReadBytes = min(Length, m_Length - m_CurPos);
	memcpy_s(pData + m_CurPos, static_cast<rsize_t>(m_Length - m_CurPos), m_pData, static_cast<rsize_t>(tReadBytes));
	m_CurPos += tReadBytes;

	if (tReadBytes != Length)
	{
		m_LastErr = NatErr_OutOfRange;
	}

	return tReadBytes;
}

nLen natMemoryStream::WriteBytes(ncData pData, nLen Length)
{
	nLen tWriteBytes = 0ul;

	if (!m_bWritable)
	{
		m_LastErr = NatErr_IllegalState;
		return tWriteBytes;
	}

	if (Length == 0ul)
	{
		m_LastErr = NatErr_OK;
		return tWriteBytes;
	}

	if (pData == nullptr)
	{
		m_LastErr = NatErr_InvalidArg;
		return tWriteBytes;
	}

	tWriteBytes = min(Length, m_Length - m_CurPos);
	memcpy_s(m_pData, static_cast<rsize_t>(tWriteBytes), pData + m_CurPos, static_cast<rsize_t>(tWriteBytes));
	m_CurPos += tWriteBytes;

	if (tWriteBytes != Length)
	{
		m_LastErr = NatErr_OutOfRange;
	}

	return tWriteBytes;
}

void natMemoryStream::Lock()
{
	m_Section.Lock();
}

nResult natMemoryStream::TryLock()
{
	return m_LastErr = m_Section.TryLock() ? NatErr_OK : NatErr_IllegalState;
}

void natMemoryStream::Unlock()
{
	m_Section.UnLock();
}

natMemoryStream::natMemoryStream()
	: m_pData(nullptr), m_Length(0ul), m_CurPos(0u), m_bReadable(false), m_bWritable(false), m_bResizable(false)
{
}

natMemoryStream::~natMemoryStream()
{
	if (!m_bExtern)
	{
		SafeDelArr(m_pData);
	}
}

natStream* natMemoryStream::CreateFromExternMemory(nData pData, nLen Length, nBool bReadable, nBool bWritable)
{
	natMemoryStream* pStream = new natMemoryStream;

	pStream->m_pData = pData;
	pStream->m_Length = Length;
	pStream->m_bReadable = bReadable;
	pStream->m_bWritable = bWritable;
	pStream->m_bExtern = true;

	return pStream;
}