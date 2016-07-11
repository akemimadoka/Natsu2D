////////////////////////////////////////////////////////////////////////////////
///	@file	n2dFontImpl.h
///	@brief	Natsu2D������
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\n2dFont.h"

#include <Windows.h>
#include <memory>
#include <unordered_map>
#include "..\RenderDevice\n2dTextureImpl.h"

struct n2dRenderDevice;
typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;

class n2dEngineImpl;
class n2dGraphics2DImpl;
class n2dRenderDeviceImpl;

////////////////////////////////////////////////////////////////////////////////
///	@brief		Natsu2D������ʵ��
///	@deprecated	ά���У�ע���ѹ�ʱ������ʹ��
////////////////////////////////////////////////////////////////////////////////
class n2dFontImpl final
	: public natRefObjImpl<n2dFont>
{
public:
	///	@brief	n2dFont���캯��
	///	@note	��ʼ��ʧ�ܻ��׳��쳣������ʹ��n2dFont::InitFont�����׳��쳣
	///	@see	n2dFontImpl::InitFont
	n2dFontImpl(int Height, int Width, int Escapement, int Orientation, int Weight, bool Italic, bool Underline, bool StrikeOut, int CharSet, int OutPrecision, int ClipPrecision, int Quality, int PitchAndFamily, LPCTSTR FaceName);
	explicit n2dFontImpl(n2dRenderDeviceImpl* pRenderer);
	~n2dFontImpl();

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
	nResult InitFont(int Height, int Width, int Escapement, int Orientation, int Weight, bool Italic, bool Underline, bool StrikeOut, int CharSet, int OutPrecision, int ClipPrecision, int Quality, int PitchAndFamily, ncTStr FaceName) override;
	
	nResult InitFont(n2dRenderDevice* pRenderer, ncTStr lpFontFile, nuInt iWidth, nuInt iHeight) override;
	nResult InitFont(n2dRenderDevice* pRenderer, natStream* pStream, nuInt iWidth, nuInt iHeight) override;

	///	@brief	��ʼ���ı�
	///	@param[in]	str		�ı�
	///	@param[in]	lStrlen	�ı�����
	///	@note	�ظ����ı������ظ���ʼ��
	nResult InitText(ncTStr str, nLen lStrlen) override;

	///	@brief	��������
	///	@param[in]	pGraphic	ͼԪ��Ⱦ��
	///	@param[in]	str			Ҫ���Ƶ��ı�
	///	@param[in]	x			Ҫ���Ƶ���x����
	///	@param[in]	y			Ҫ���Ƶ���y����
	///	@param[in]	z			Ҫ���Ƶ���z����
	nResult PrintFont(n2dGraphics2D* pGraphic, ncTStr str, nFloat x, nFloat y, nFloat z) override;

private:
	FT_Library m_pFTLib;
	FT_Face m_pFTFace;
	HFONT m_hFont;
	nData m_pFileBuf;
	std::shared_ptr<n2dImage> m_TmpImage;
	std::unordered_map<nWChar, n2dCharTexture> m_FontCache;

	n2dRenderDevice* m_pRenderer;
};