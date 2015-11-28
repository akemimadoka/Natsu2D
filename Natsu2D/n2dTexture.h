#pragma once
#include "n2dInterface.h"
#include "n2dCommon.h"
#include <memory>

struct natStream;

///	@brief	ͼ��ṹ��
struct n2dImage final
{
	explicit n2dImage(GLenum ichannels = 0u, GLenum pixelFormat = 0u, ncData pdata = nullptr, nLong llength = 0l, nuInt iwidth = 0u, nuInt iheight = 0u)
		: Channels(ichannels),
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

	GLenum Channels;
	GLenum PixelFormat;
	nData data;
	nLong length;
	nuInt width;
	nuInt height;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	�������
////////////////////////////////////////////////////////////////////////////////
struct n2dTexture
	: n2dInterface
{
	virtual ~n2dTexture() = default;

	///	@brief	�������ID
	virtual nuInt GetTextureID() const = 0;
};

struct n2dTexture2D
	: n2dTexture
{
	///	@brief	��������
	///	@param[in]	filename	�ļ���
	///	@return	�Ƿ���سɹ�
	virtual nBool LoadTexture(nTString const& filename) = 0;

	virtual nBool LoadTexture(natStream* pStream, DWORD dwFileType) = 0;

	///	@brief	��Image��������
	///	@param[in]	image	�Ѽ��ص�Image
	///	@return	�Ƿ���سɹ�
	virtual nBool LoadTexture(const std::shared_ptr<n2dImage>& image) = 0;
};