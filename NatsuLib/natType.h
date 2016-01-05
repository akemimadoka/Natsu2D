////////////////////////////////////////////////////////////////////////////////
///	@file	natType.h
///	@brief	����NatsuLib�л����������͡����ֺ꼰��������
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cstdint>
#include <tchar.h>
#include <string>

////////////////////////////////////////////////////////////////////////////////
///	@addtogroup	Natsu�������������
///	@brief		Natsu������������Ͷ���
///	@{

typedef	bool				nBool;		///< @brief	�߼���
typedef char				nChar;		///< @brief	�ַ���
typedef	wchar_t				nWChar;		///< @brief	���ַ���
typedef	uint8_t				nByte;		///< @brief	�ֽ���
typedef int16_t				nShort;		///< @brief	16λ������
typedef	uint16_t			nuShort;	///< @brief	16λ�޷��Ŷ�����
typedef	int32_t				nInt;		///< @brief	32λ����
typedef	uint32_t			nuInt;		///< @brief	32λ�޷�������
typedef int64_t				nLong;		///< @brief	64λ������
typedef	uint64_t			nuLong;		///< @brief	64λ�޷��ų�����
typedef	float				nFloat;		///< @brief	�����ȸ�����
typedef	double				nDouble;	///< @brief	˫���ȸ�����
typedef	nChar*				nStr;		///< @brief	C����ַ���
typedef	const nChar*		ncStr;		///< @brief	C������ַ���
typedef	nWChar*				nWStr;		///< @brief	C�����ַ���
typedef	const nWChar*		ncWStr;		///< @brief	C��������ַ���
typedef	nByte*				nData;		///< @brief	�ڴ�����ָ��
typedef	const nByte*		ncData;		///< @brief	�����ڴ�����ָ��
typedef	nuLong				nLen;		///< @brief	������
typedef	nInt				nResult;	///< @brief	Ԥ���巵��ֵ
										///< @details
										///	nResult ���壺\n
										///	��  ��  λ	�� 0 - �ɹ� 1 - ʧ��\n
										///	30 - 16 λ	�� ����\n
										///	15 - 0  λ	�� ��������

#ifdef UNICODE
typedef nWChar				nTChar;		///< @brief	ͨ���ַ�
typedef	nWStr				nTStr;		///< @brief	ͨ���ַ���
typedef ncWStr				ncTStr;		///< @brief	����ͨ���ַ���
#else
typedef nChar				nTChar;		///< @brief	ͨ���ַ�
typedef	nStr				nTStr;		///< @brief	ͨ���ַ���
typedef	ncStr				ncTStr;		///< @brief	����ͨ���ַ���
#endif

typedef std::basic_string<nTChar, std::char_traits<nTChar>, std::allocator<nTChar> > nTString;	///< @brief	ͨ��string��

///	@}

////////////////////////////////////////////////////////////////////////////////
///	@addtogroup	Natsu�������
///	@brief		Natsu������궨��
///	@{

///	@brief	ǿ������
#define	NATINLINE			__forceinline
///	@brief	�ܾ�����
#define NATNOINLINE			__declspec(noinline)

///	@brief	�Ƿ�ɹ�
#define NATOK(x)			(((nResult)(x)) >= 0)
///	@brief	�Ƿ�ʧ��
#define	NATFAIL(x)			(((nResult)(x)) <  0)
///	@brief	����������
#define	NATMAKEERR(code)	((nResult) (0x80000000 | ((nInt)(code))))
///	@brief	״̬��
#define	NATRESULTCODE(code)	((code) & 0xFFFF)

///	@}

////////////////////////////////////////////////////////////////////////////////
///	@addtogroup	Natsu�ⳣ������
///	@brief		��������
///	@{

enum NatErr
{
	NatErr_OK			=	nResult(0),		///< @brief	����
	NatErr_Unknown		=	NATMAKEERR(1),	///< @brief	δ֪����
	NatErr_IllegalState	=	NATMAKEERR(2),	///< @brief	�Ƿ�״̬
	NatErr_InvalidArg	=	NATMAKEERR(3),	///< @brief	��Ч����
	NatErr_InternalErr	=	NATMAKEERR(4),	///< @brief	�ڲ�����
	NatErr_OutOfRange	=	NATMAKEERR(5),	///< @brief	������Χ
	NatErr_NotImpl		=	NATMAKEERR(6),	///< @brief	δʵ��
};

///	@}

////////////////////////////////////////////////////////////////////////////////
///	@addtogroup	Natsu�������������
///	@brief		Natsu�����������������
///	@{

///	@brief	��ȫɾ��ָ��
template <typename T>
NATINLINE void SafeDel(T*& ptr)
{
	delete ptr;
	ptr = nullptr;
}

///	@brief	��ȫɾ������
template <typename T>
NATINLINE void SafeDelArr(T*& ptr)
{
	delete[] ptr;
	ptr = nullptr;
}

///	@brief	��ȫ�ͷ�
template <typename T>
NATINLINE void SafeRelease(T*& ptr)
{
	if (ptr != nullptr)
	{
		ptr->Release();
		ptr = nullptr;
	}
}

///	@}