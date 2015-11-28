#pragma once
#include "n2dInterface.h"

class natStopWatch;

struct n2dFPSController
	: n2dInterface
{
	///	@brief	����
	virtual nDouble Update(natStopWatch& watch) = 0;

	///	@brief	���FPS����
	virtual nuInt GetFPSLimit() const = 0;
	///	@brief	����FPS����
	virtual void SetFPSLimit(nuInt FPSLimit) = 0;

	///	@brief	��õ�ǰFPS
	virtual nDouble GetFPS() const = 0;

	///	@brief	����ܹ�֡��
	virtual nuInt GetTotalFrame() const = 0;
	///	@brief	����ܹ�ʱ��
	virtual nDouble GetTotalTime() const = 0;

	///	@brief	���ƽ��FPS
	virtual nDouble GetAvgFPS() const = 0;
	///	@brief	������FPS
	virtual nDouble GetMaxFPS() const = 0;

	///	@brief	��ǰFPS�����ԣ�
	__declspec(property(get = GetFPS))
		nDouble FPS;

	///	@brief	FPS���ƣ����ԣ�
	__declspec(property(get = GetFPSLimit, put = SetFPSLimit))
		nuInt FPSLimit;
};