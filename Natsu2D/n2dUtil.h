////////////////////////////////////////////////////////////////////////////////
///	@file	n2dUtil.h
///	@brief	工具
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Natsu2D.h"
#include <natUtil.h>

struct n2dRenderDevice;

////////////////////////////////////////////////////////////////////////////////
///	@addtogroup	杂项
///	@{

///	@brief	包含工具函数
namespace n2dUtil
{
	using namespace natUtil;

	///	@brief	转换场景坐标到窗口坐标
	///	@note	此转换按默认视角计算
	///	@deprecated		存在bug，请勿使用
	void N2DFUNC TransformCoord(n2dRenderDevice* pRenderer, GLfloat fScenex, GLfloat fSceney, GLint& iScrx, GLint& iScry);

	///	@brief	转换窗口坐标到场景坐标
	///	@note	此转换按默认视角计算
	///	@deprecated		存在bug，请勿使用
	void N2DFUNC TransformCoord(n2dRenderDevice* pRenderer, GLint iScrx, GLint iScry, GLfloat& fScenex, GLfloat& fSceney);
}

///	@}