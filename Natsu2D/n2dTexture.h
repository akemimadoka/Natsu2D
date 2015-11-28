#pragma once
#include "n2dInterface.h"
#include "n2dCommon.h"
#include <memory>

struct natStream;

///	@brief	图像结构体
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
///	@brief	纹理基类
////////////////////////////////////////////////////////////////////////////////
struct n2dTexture
	: n2dInterface
{
	virtual ~n2dTexture() = default;

	///	@brief	获得纹理ID
	virtual nuInt GetTextureID() const = 0;
};

struct n2dTexture2D
	: n2dTexture
{
	///	@brief	加载纹理
	///	@param[in]	filename	文件名
	///	@return	是否加载成功
	virtual nBool LoadTexture(nTString const& filename) = 0;

	virtual nBool LoadTexture(natStream* pStream, DWORD dwFileType) = 0;

	///	@brief	从Image加载纹理
	///	@param[in]	image	已加载的Image
	///	@return	是否加载成功
	virtual nBool LoadTexture(const std::shared_ptr<n2dImage>& image) = 0;
};