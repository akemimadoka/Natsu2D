#include "n2dTextureImpl.h"
#include "..\n2dCommon.h"
#include "OpenGL.h"
#include "..\include\CxImage\ximage.h"
#include "..\extern\nv_dds\nv_dds.h"
#include <algorithm>
#include <memory>
#include <natStream.h>

n2dImage* LoadPicture(natRefPointer<natStream> const& pStream, DWORD dTypeId)
{
	pStream->Lock();
	pStream->SetPosition(NatSeek::Beg, 0l);
	nLen lSize = pStream->GetSize();
	nData pBuf = new nByte[static_cast<nuInt>(lSize)];
	if (pStream->ReadBytes(pBuf, lSize) != lSize || pStream->GetLastErr() != NatErr_OK)
	{
		SafeDelArr(pBuf);
		return nullptr;
	}
	pStream->Unlock();

	CxImage ci(pBuf, static_cast<DWORD>(lSize), dTypeId);

	n2dImage* retimage = new n2dImage;

	retimage->width = ci.GetWidth();
	retimage->height = ci.GetHeight();
	auto colorType = ci.GetColorType();
	if (colorType == 2)
	{
		retimage->PixelFormat = GL_RGB;
		retimage->Channels = 3;
	}
	else if (colorType == 4)
	{
		retimage->PixelFormat = GL_RGBA;
		retimage->Channels = 4;
	}
	else
	{
		delete retimage;
		return nullptr;
	}

	nByte* tmpbuf = nullptr;
	long lRet;
	ci.Encode2RGBA(tmpbuf, lRet, true);

	retimage->data = new nByte[lRet];
	memcpy_s(retimage->data, lRet, tmpbuf, lRet);
	retimage->length = lRet;

	ci.FreeMemory(tmpbuf);
	SafeDelArr(pBuf);

	return retimage;
}

nv_dds::CDDSImage* LoadDDSData(natRefPointer<natStream> const& pStream)
{
	nv_dds::CDDSImage* image = new nv_dds::CDDSImage;

	if (!image->load(pStream))
	{
		delete image;
		return nullptr;
	}

	return image;
}

n2dTexture2DImpl::n2dTexture2DImpl(): m_TextureID(UINT_MAX)
{
}

n2dTexture2DImpl::~n2dTexture2DImpl()
{
	glDeleteTextures(1, &m_TextureID);
}

nBool n2dTexture2DImpl::LoadTexture(nTString const& filename)
{
	auto tName = filename.substr(filename.find_last_of(_T('.')) + 1u);
	natFileStream* pStream = new natFileStream(filename.c_str(), true, false);

	if (lstrcmpi(tName.c_str(), _T("dds")) == 0)
	{
		return LoadDDS(pStream);
	}

	return LoadTexture(pStream, CxImage::GetTypeIdFromName(tName.c_str()));
}

nBool n2dTexture2DImpl::LoadTexture(natStream* pStream, DWORD dwFileType)
{
	return LoadTexture(std::shared_ptr<n2dImage>(LoadPicture(pStream, dwFileType)));
}

nBool n2dTexture2DImpl::LoadTexture(const std::shared_ptr<n2dImage>& image)
{
	if (image == nullptr)
	{
		return false;
	}

	glGenTextures(1, &m_TextureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, image->PixelFormat, image->width, image->height, 0, image->PixelFormat, GL_UNSIGNED_BYTE, image->data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	return true;
}

nuInt n2dTexture2DImpl::GetTextureID() const
{
	return m_TextureID;
}

nBool n2dTexture2DImpl::LoadDDS(natStream* pStream)
{
	nv_dds::CDDSImage* image = LoadDDSData(pStream);
	if (image == nullptr)
	{
		return false;
	}

	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);

	glCompressedTexImage2D(GL_TEXTURE_2D, 0, image->get_format(), image->get_width(), image->get_height(), 0, image->get_size(), *image);

	for (int idx = 0; idx < image->get_num_mipmaps(); ++idx)
	{
		glCompressedTexImage2D(GL_TEXTURE_2D, idx + 1, image->get_format(), image->get_mipmap(idx).get_width(), image->get_mipmap(idx).get_height(), 0, image->get_mipmap(idx).get_size(), image->get_mipmap(idx));
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	return true;
}