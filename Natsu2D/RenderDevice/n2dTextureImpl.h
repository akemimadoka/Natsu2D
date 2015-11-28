////////////////////////////////////////////////////////////////////////////////
///	@file	n2dTextureImpl.h
///	@brief	����ʵ��
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <memory>
#include <natType.h>
#include "..\n2dTexture.h"

struct natStream;

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