#pragma once
#include "n2dInterface.h"

struct n2dRenderDevice;
struct natStream;
struct n2dGraphics2D;
struct n2dTexture2D;

////////////////////////////////////////////////////////////////////////////////
///	@brief	�ַ�����
////////////////////////////////////////////////////////////////////////////////
struct n2dCharTexture final
{
	n2dTexture2D*	CharTexture;	///< @brief	���ɵ��ַ�����
	nuInt			Width;			///< @brief	��
	nuInt			Height;			///< @brief	��

	nuInt			adv_x;
	nuInt			adv_y;
	nuInt			delta_x;
	nuInt			delta_y;

	n2dCharTexture()
		: CharTexture(nullptr),
		Width(0),
		Height(0),
		adv_x(0u),
		adv_y(0u),
		delta_x(0u),
		delta_y(0u)
	{
	}
};

struct n2dFont
	: n2dInterface
{
	///	@brief	��ʼ������
	///	@param[in]	Height			����߶�
	///	@param[in]	Width			������
	///	@param[in]	Escapement		��λ�������豸X��֮��ĽǶ�
	///	@param[in]	Orientation		ÿ���ַ��Ļ��ߺ��豸X��֮��ĽǶ�
	///	@param[in]	Weight			�����Ȩֵ�����ߵ�ֵʹ��������֣�ȡֵ[0, 1000]
	///	@param[in]	Italic			б��
	///	@param[in]	Underline		�»���
	///	@param[in]	StrikeOut		ɾ����
	///	@param[in]	CharSet			�ַ���
	///	@param[in]	OutPrecision	�������
	///	@param[in]	ClipPrecision	�ü�����
	///	@param[in]	Quality			�������
	///	@param[in]	PitchAndFamily	�������������
	///	@param[in]	FaceName		���������������Ȳ��ó���32���ַ�������\\0��
	///	@note	�ڹ���ʱ�����ã�֮������ٴ�ʹ�ø÷�����������
	///	@return	�Ƿ�ɹ���ʼ��
	virtual nResult InitFont(int Height, int Width, int Escapement, int Orientation, int Weight, bool Italic, bool Underline, bool StrikeOut, int CharSet, int OutPrecision, int ClipPrecision, int Quality, int PitchAndFamily, LPCTSTR FaceName) = 0;

	virtual nResult InitFont(n2dRenderDevice* pRenderer, ncTStr lpFontFile, nuInt iWidth, nuInt iHeight) = 0;
	virtual nResult InitFont(n2dRenderDevice* pRenderer, natStream* pStream, nuInt iWidth, nuInt iHeight) = 0;

	///	@brief	��ʼ���ı�
	///	@param[in]	str		�ı�
	///	@param[in]	lStrlen	�ı�����
	///	@note	�ظ����ı������ظ���ʼ��
	virtual nResult InitText(ncTStr str, nLen lStrlen) = 0;

	///	@brief	��������
	///	@param[in]	pGraphic	ͼԪ��Ⱦ��
	///	@param[in]	str			Ҫ���Ƶ��ı�
	///	@param[in]	x			Ҫ���Ƶ���x����
	///	@param[in]	y			Ҫ���Ƶ���y����
	///	@param[in]	z			Ҫ���Ƶ���z����
	virtual nResult PrintFont(n2dGraphics2D* pGraphic, ncTStr str, nFloat x, nFloat y, nFloat z) = 0;
};