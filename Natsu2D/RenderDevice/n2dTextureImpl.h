////////////////////////////////////////////////////////////////////////////////
///	@file	n2dTextureImpl.h
///	@brief	����ʵ��
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <memory>
#include <natType.h>
#include "OpenGL.h"
#include "..\n2dRenderDevice.h"

struct natStream;

struct n2dImage final
{
	explicit n2dImage(GLenum internalFormat = 0u, GLenum pixelFormat = 0u, ncData pdata = nullptr, nLong llength = 0l, nuInt iwidth = 0u, nuInt iheight = 0u)
		: InternalFormat(internalFormat),
		PixelFormat(pixelFormat),
		data(nullptr),
		length(llength),
		width(iwidth),
		height(iheight)
	{
		data = new nByte[static_cast<nuInt>(length)];
		memcpy_s(data, static_cast<rsize_t>(length), pdata, static_cast<rsize_t>(llength));
	}

	~n2dImage()
	{
		SafeDelArr(data);
	}

	GLenum InternalFormat;
	GLenum PixelFormat;
	nData data;
	nLong length;
	nuInt width;
	nuInt height;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	2D����
////////////////////////////////////////////////////////////////////////////////
class n2dTexture2DImpl final
	: public natRefObjImpl<n2dTexture2D>
{
public:
	n2dTexture2DImpl();
	~n2dTexture2DImpl();

	///	@brief	��������
	///	@param[in]	filename	�ļ���
	///	@return	�Ƿ���سɹ�
	nBool LoadTexture(nTString const& filename) override;

	nBool LoadTexture(natStream* pStream, DWORD dwFileType) override;

	///	@brief	��Image��������
	///	@param[in]	image	�Ѽ��ص�Image
	///	@return	�Ƿ���سɹ�
	nBool LoadTexture(const std::shared_ptr<n2dImage>& image) override;

	///	@brief	�������ID
	nuInt GetTextureID() const override;
private:
	nuInt m_TextureID;

	bool LoadDDS(natStream* pStream);
};