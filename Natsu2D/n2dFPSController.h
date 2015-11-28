#pragma once
#include "n2dInterface.h"

class natStopWatch;

struct n2dFPSController
	: n2dInterface
{
	///	@brief	更新
	virtual nDouble Update(natStopWatch& watch) = 0;

	///	@brief	获得FPS限制
	virtual nuInt GetFPSLimit() const = 0;
	///	@brief	设置FPS限制
	virtual void SetFPSLimit(nuInt FPSLimit) = 0;

	///	@brief	获得当前FPS
	virtual nDouble GetFPS() const = 0;

	///	@brief	获得总共帧数
	virtual nuInt GetTotalFrame() const = 0;
	///	@brief	获得总共时间
	virtual nDouble GetTotalTime() const = 0;

	///	@brief	获得平均FPS
	virtual nDouble GetAvgFPS() const = 0;
	///	@brief	获得最大FPS
	virtual nDouble GetMaxFPS() const = 0;

	///	@brief	当前FPS（属性）
	__declspec(property(get = GetFPS))
		nDouble FPS;

	///	@brief	FPS限制（属性）
	__declspec(property(get = GetFPSLimit, put = SetFPSLimit))
		nuInt FPSLimit;
};