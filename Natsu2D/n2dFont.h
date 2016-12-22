////////////////////////////////////////////////////////////////////////////////
///	@file	n2dFont.h
///	@brief	Natsu2D����
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "n2dInterface.h"
#include "natMath.h"

struct n2dTexture;
struct n2dRenderDevice;
namespace NatsuLib
{
	struct natStream;
}
struct n2dGraphics2D;
struct n2dTexture2D;

////////////////////////////////////////////////////////////////////////////////
/// @brief	Natsu2D����
////////////////////////////////////////////////////////////////////////////////
struct n2dFont
	: n2dInterface
{

	/// @brief	ʹ�������ļ����г�ʼ��
	/// @param[in]	pRenderer	��Ⱦ�豸
	/// @param[in]	lpFontFile	�����ļ���
	/// @param[in]	iWidth		�����ȣ����أ�
	/// @param[in]	iHeight		����߶ȣ����أ�
	virtual nResult InitFont(n2dRenderDevice* pRenderer, nStrView lpFontFile, nuInt iWidth, nuInt iHeight) = 0;
	/// @brief	ʹ�������ļ����г�ʼ��
	/// @param[in]	pRenderer	��Ⱦ�豸
	/// @param[in]	pStream		������
	/// @param[in]	iWidth		�����ȣ����أ�
	/// @param[in]	iHeight		����߶ȣ����أ�
	virtual nResult InitFont(n2dRenderDevice* pRenderer, natStream* pStream, nuInt iWidth, nuInt iHeight) = 0;

	///	@brief	��ʼ���ı�
	///	@param[in]	str		�ı�
	///	@param[in]	lStrlen	�ı�����
	///	@note	�ظ����ı������ظ���ʼ��
	virtual nResult InitText(nStrView str, nLen lStrlen) = 0;

	nResult InitText(nStrView str)
	{
		return InitText(str, str.size());
	}

	template <size_t n>
	nResult InitText(const nTChar (&str)[n])
	{
		return InitText(str, n - 1);
	}

	nResult InitText(nString const& str)
	{
		return InitText(str, str.size());
	}

	/// @brief	ʹ����ɫ��������
	/// @param[in]	pGraphic	ͼԪ��Ⱦ��
	/// @param[in]	str			Ҫ���Ƶ��ַ���
	/// @param[in]	x			Ҫ���Ƶ���x����
	/// @param[in]	y			Ҫ���Ƶ���y����
	/// @param[in]	scale		����ϵ��
	/// @param[in]	color		������ɫ
	virtual nResult PrintFont(n2dGraphics2D* pGraphic, nStrView str, nFloat x, nFloat y, nFloat scale, natVec3<> const& color) = 0;

	/// @brief	ʹ�������������
	/// @param[in]	pGraphic	ͼԪ��Ⱦ��
	/// @param[in]	str			Ҫ���Ƶ��ַ���
	/// @param[in]	x			Ҫ���Ƶ���x����
	/// @param[in]	y			Ҫ���Ƶ���y����
	/// @param[in]	scale		����ϵ��
	/// @param[in]	pTexture	��������
	virtual nResult PrintFont(n2dGraphics2D* pGraphic, nStrView str, nFloat x, nFloat y, nFloat scale, n2dTexture* pTexture) = 0;
};
