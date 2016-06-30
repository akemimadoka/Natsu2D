#include "n2dTextureImpl.h"
#include "..\include\CxImage\ximage.h"
#include "..\extern\nv_dds\nv_dds.h"
#include <natStream.h>
#include <natLog.h>
#include <memory>

namespace
{
	std::unique_ptr<n2dImage> LoadPicture(natStream* pStream, DWORD dTypeId)
	{
		pStream->Lock();
		//pStream->SetPosition(NatSeek::Beg, 0l);
		nLen lSize = pStream->GetSize() - pStream->GetPosition();
		std::vector<nByte> Buf(static_cast<nuInt>(lSize));
		if (pStream->ReadBytes(Buf.data(), lSize) != lSize || pStream->GetLastErr() != NatErr_OK)
		{
			return nullptr;
		}
		pStream->Unlock();

		CxImage ci(Buf.data(), static_cast<DWORD>(lSize), dTypeId);

		auto retimage = std::make_unique<n2dImage>();

		retimage->width = ci.GetWidth();
		retimage->height = ci.GetHeight();
		auto colorType = ci.GetColorType();
		switch (colorType)
		{
		case 2:
			retimage->PixelFormat = GL_RGB;
			retimage->InternalFormat = GL_RGB;
			break;
		case 4:
			retimage->PixelFormat = GL_RGBA;
			retimage->InternalFormat = GL_RGBA;
			break;
		default:
			return nullptr;
		}

		nByte* tmpbuf = nullptr;
		long lRet;
		ci.Encode2RGBA(tmpbuf, lRet, true);

		retimage->data.assign(tmpbuf, tmpbuf + lRet);
		retimage->length = lRet;

		ci.FreeMemory(tmpbuf);

		return retimage;
	}

	std::unique_ptr<nv_dds::CDDSImage> LoadDDSData(natStream* pStream)
	{
		auto image = std::make_unique<nv_dds::CDDSImage>();

		if (!image->load(pStream))
		{
			return nullptr;
		}

		return image;
	}
}

n2dTexture2DImpl::n2dTexture2DImpl() : m_TextureID(0u)
{
}

n2dTexture2DImpl::~n2dTexture2DImpl()
{
	glDeleteTextures(1, &m_TextureID);
}

nBool n2dTexture2DImpl::LoadTexture(nTString const& filename)
{
	auto tName = filename.substr(filename.find_last_of(_T('.')) + 1u);

	auto pStream = make_ref<natFileStream>(filename.c_str(), true, false);

	if (lstrcmpi(tName.c_str(), _T("dds")) == 0)
	{
		return LoadDDS(pStream);
	}

	return LoadTexture(pStream, CxImage::GetTypeIdFromName(tName.c_str()));
}

nBool n2dTexture2DImpl::LoadTexture(natStream* pStream, DWORD dwFileType)
{
	return LoadTexture(*LoadPicture(pStream, dwFileType));
}

nBool n2dTexture2DImpl::LoadTexture(n2dImage const& image)
{
	glGenTextures(1, &m_TextureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, image.InternalFormat, image.width, image.height, 0, image.PixelFormat, GL_UNSIGNED_BYTE, image.data.data());

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
	auto image = LoadDDSData(pStream);
	if (!image)
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

	image->clear();

	return true;
}