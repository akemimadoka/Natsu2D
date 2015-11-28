////////////////////////////////////////////////////////////////////////////////
///	@file	n2dUtil.h
///	@brief	����
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Natsu2D.h"
#include <natUtil.h>

struct n2dRenderDevice;

////////////////////////////////////////////////////////////////////////////////
///	@addtogroup	����
///	@{

///	@brief	�������ߺ���
namespace n2dUtil
{
	using namespace natUtil;

	///	@brief	ת���������굽��������
	///	@note	��ת����Ĭ���ӽǼ���
	///	@deprecated		����bug������ʹ��
	void N2DFUNC TransformCoord(n2dRenderDevice* pRenderer, GLfloat fScenex, GLfloat fSceney, GLint& iScrx, GLint& iScry);

	///	@brief	ת���������굽��������
	///	@note	��ת����Ĭ���ӽǼ���
	///	@deprecated		����bug������ʹ��
	void N2DFUNC TransformCoord(n2dRenderDevice* pRenderer, GLint iScrx, GLint iScry, GLfloat& fScenex, GLfloat& fSceney);
}

///	@}