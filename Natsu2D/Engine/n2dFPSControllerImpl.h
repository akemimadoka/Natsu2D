////////////////////////////////////////////////////////////////////////////////
///	@file	n2dFPSControllerImpl.h
///	@brief	FPS控制器
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\n2dEngine.h"

#include <natStopWatch.h>
#include <natType.h>

////////////////////////////////////////////////////////////////////////////////
///	@brief	FPS控制器实现
////////////////////////////////////////////////////////////////////////////////
class n2dFPSControllerImpl final
	: public natRefObjImpl<n2dFPSController>
{
public:
	///	@brief	FPS控制器构造函数
	///	@param[in]	iFPSLimit	FPS限制
	explicit n2dFPSControllerImpl(nuInt iFPSLimit);
	~n2dFPSControllerImpl() = default;

	///	@brief	更新
	nDouble Update(natStopWatch& watch) override;

	///	@brief	获得FPS限制
	nuInt GetFPSLimit() const override;
	///	@brief	设置FPS限制
	void SetFPSLimit(nuInt FPSLimit) override;

	///	@brief	获得当前FPS
	nDouble GetFPS() const override;

	///	@brief	获得总共帧数
	nuInt GetTotalFrame() const override;
	///	@brief	获得总共时间
	nDouble GetTotalTime() const override;

	///	@brief	获得平均FPS
	nDouble GetAvgFPS() const override;
	///	@brief	获得最大FPS
	nDouble GetMaxFPS() const override;

	///	@brief	当前FPS（属性）
	__declspec(property(get=GetFPS))
	double FPS;

	///	@brief	FPS限制（属性）
	__declspec(property(get=GetFPSLimit, put=SetFPSLimit))
	nuInt FPSLimit;
private:
	nuInt m_TotalFrame;
	double m_TotalTime;
	double m_MaxFPS;
	double m_CurFPS;

	double m_FramesTime;
	nuInt m_FPSstat;

	nuInt m_FPSLimit;
	double m_FrameDelay;
};