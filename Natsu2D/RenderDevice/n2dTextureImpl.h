////////////////////////////////////////////////////////////////////////////////
///	@file	n2dTextureImpl.h
///	@brief	纹理实现
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <memory>
#include <natType.h>
#include "..\n2dTexture.h"

struct natStream;

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
	nBool LoadTexture(const std::shared_ptr<n2dImage>& image) override;

	///	@brief	获得纹理ID
	nuInt GetTextureID() const override;
private:
	nuInt m_TextureID;

	bool LoadDDS(natStream* pStream);
};