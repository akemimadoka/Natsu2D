////////////////////////////////////////////////////////////////////////////////
///	@file	n2dTextureImpl.h
///	@brief	纹理实现
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <natType.h>
#include "OpenGL.h"
#include "..\n2dRenderDevice.h"

namespace NatsuLib
{
	struct natStream;
}

struct n2dImage final
{
	explicit n2dImage(GLenum internalFormat = 0u, GLenum pixelFormat = 0u, ncData pdata = nullptr, nLong llength = 0l, nuInt iwidth = 0u, nuInt iheight = 0u)
		: InternalFormat(internalFormat),
		PixelFormat(pixelFormat),
		length(llength),
		width(iwidth),
		height(iheight)
	{
		if (pdata)
		{
			data.assign(pdata, pdata + length);
		}
	}

	GLenum InternalFormat;
	GLenum PixelFormat;
	std::vector<nByte> data;
	nLong length;
	nuInt width;
	nuInt height;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	2D纹理
////////////////////////////////////////////////////////////////////////////////
class n2dTexture2DImpl final
	: public natRefObjImpl<n2dTexture2D>
{
public:
	n2dTexture2DImpl();
	~n2dTexture2DImpl();

	///	@brief	加载纹理
	///	@param[in]	filename	文件名
	///	@return	是否加载成功
	nBool LoadTexture(nTString const& filename) override;

	nBool LoadTexture(natStream* pStream, DWORD dwFileType) override;

	///	@brief	从Image加载纹理
	///	@param[in]	image	已加载的Image
	///	@return	是否加载成功
	nBool LoadTexture(n2dImage const& image) override;

	///	@brief	获得纹理ID
	nuInt GetTextureID() const override;
private:
	nuInt m_TextureID;

	bool LoadDDS(natStream* pStream);
};