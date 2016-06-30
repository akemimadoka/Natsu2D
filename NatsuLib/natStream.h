////////////////////////////////////////////////////////////////////////////////
///	@file	natStream.h
///	@brief	NatsuLib��
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include "natRefObj.h"
#include "natMultiThread.h"

namespace NatsuLib
{
	////////////////////////////////////////////////////////////////////////////////
	///	@brief	Ѱַ�ο�λ��
	////////////////////////////////////////////////////////////////////////////////
	enum class NatSeek
	{
		Beg,	///< @brief	����ͷ
		Cur,	///< @brief	��ǰ��λ��
		End		///< @brief	����β
	};

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	��
	////////////////////////////////////////////////////////////////////////////////
	struct natStream
		: natRefObj
	{
		virtual ~natStream() = default;

		///	@brief		�������Ĵ���
		virtual NatErr GetLastErr() const = 0;

		///	@brief		���Ƿ��д
		virtual nBool CanWrite() const = 0;

		///	@brief		���Ƿ�ɶ�
		virtual nBool CanRead() const = 0;

		///	@brief		���Ƿ�����·����С
		virtual nBool CanResize() const = 0;

		///	@brief		���Ƿ��Ѱַ
		virtual nBool CanSeek() const = 0;

		///	@brief		������Ĵ�С
		virtual nLen GetSize() const = 0;

		///	@brief		���·������Ĵ�С
		///	@note		���·���ɹ��Ժ�ָ��λ�����Ŀ�ͷ
		virtual nResult SetSize(nLen Size) = 0;

		///	@brief		��ö�дָ���λ��
		virtual nLen GetPosition() const = 0;

		///	@brief		���ö�дָ���λ��
		///	@param[in]	Origin	Ѱַ�ο�λ��
		///	@param[in]	Offset	ƫ��
		virtual nResult SetPosition(NatSeek Origin, nLong Offset) = 0;

		///	@brief		��ȡ�ֽ�����
		///	@param[out]	pData	���ݻ�����
		///	@param[in]	Length	��ȡ�ĳ���
		///	@return		ʵ�ʶ�ȡ����
		virtual nLen ReadBytes(nData pData, nLen Length) = 0;

		///	@brief		д���ֽ�����
		///	@param[in]	pData	���ݻ�����
		///	@param[in]	Length	д��ĳ���
		///	@return		ʵ��д�볤��
		virtual nLen WriteBytes(ncData pData, nLen Length) = 0;

		///	@brief		ˢ����
		///	@note		�����л�����Ƶ�����Ч��������
		virtual void Flush() = 0;

		///	@brief		������
		///	@note		�����������
		///	@warning	���ڶ��߳���ʹ��ʱ�������
		virtual void Lock() = 0;

		///	@brief		����������
		///	@warning	���ڶ��߳���ʹ��ʱ�������
		virtual nResult TryLock() = 0;

		///	@brief		������
		///	@note		���������Ѿ���Lock���߳ɹ�TryLock�Ժ�ʹ��
		///	@warning	���ڶ��߳���ʹ��ʱ�������
		virtual void Unlock() = 0;
	};

	class natMemoryStream;

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	NatsuLib�ļ���ʵ��
	////////////////////////////////////////////////////////////////////////////////
	class natFileStream
		: public natRefObjImpl<natStream>
	{
	public:
		natFileStream(ncTStr lpFilename, nBool bReadable, nBool bWritable);
		~natFileStream();

		NatErr GetLastErr() const override;
		nBool CanWrite() const override;
		nBool CanRead() const override;
		nBool CanResize() const override;
		nBool CanSeek() const override;
		nLen GetSize() const override;
		nResult SetSize(nLen Size) override;
		nLen GetPosition() const override;
		nResult SetPosition(NatSeek Origin, nLong Offset) override;
		nLen ReadBytes(nData pData, nLen Length) override;
		nLen WriteBytes(ncData pData, nLen Length) override;
		void Flush() override;
		void Lock() override;
		nResult TryLock() override;
		void Unlock() override;

		ncTStr GetFilename() const noexcept;
		HANDLE GetUnsafeHandle() const noexcept;
		natRefPointer<natMemoryStream> MapToMemoryStream();

	private:
		HANDLE m_hFile, m_hMappedFile = NULL;
		natRefPointer<natMemoryStream> m_pMappedFile;
		nTString m_Filename;
		nBool m_bReadable, m_bWritable;
		natCriticalSection m_Section;

	protected:
		NatErr m_LastErr;
	};

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	NatsuLib�ڴ���ʵ��
	////////////////////////////////////////////////////////////////////////////////
	class natMemoryStream
		: public natRefObjImpl<natStream>
	{
	public:
		natMemoryStream();
		natMemoryStream(ncData pData, nLen Length, nBool bReadable, nBool bWritable, nBool bResizable);
		~natMemoryStream();

		static natRefPointer<natMemoryStream> CreateFromExternMemory(nData pData, nLen Length, nBool bReadable, nBool bWritable);

		NatErr GetLastErr() const override;
		nBool CanWrite() const override;
		nBool CanRead() const override;
		nBool CanResize() const override;
		nBool CanSeek() const override;
		nLen GetSize() const override;
		nResult SetSize(nLen Size) override;
		nLen GetPosition() const override;
		nResult SetPosition(NatSeek Origin, nLong Offset) override;
		nLen ReadBytes(nData pData, nLen Length) override;
		nLen WriteBytes(ncData pData, nLen Length) override;
		void Flush() override;
		void Lock() override;
		nResult TryLock() override;
		void Unlock() override;

		nData GetInternalBuffer() noexcept;
		ncData GetInternalBuffer() const noexcept;

	private:
		nData m_pData;
		nLen m_Length;
		nLen m_CurPos;
		nBool m_bReadable;
		nBool m_bWritable;
		nBool m_bResizable;
		nBool m_bExtern;
		natCriticalSection m_Section;

		NatErr m_LastErr;
	};
}
