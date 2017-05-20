////////////////////////////////////////////////////////////////////////////////
///	@file	n2dFPSControllerImpl.h
///	@brief	FPS������
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../n2dEngine.h"

#include <natStopWatch.h>
#include <natType.h>
#include <natProperty.h>

////////////////////////////////////////////////////////////////////////////////
///	@brief	FPS������ʵ��
////////////////////////////////////////////////////////////////////////////////
class n2dFPSControllerImpl final
	: public natRefObjImpl<n2dFPSControllerImpl, n2dFPSController>
{
public:
	///	@brief	FPS���������캯��
	///	@param[in]	iFPSLimit	FPS����
	explicit n2dFPSControllerImpl(nuInt iFPSLimit);
	~n2dFPSControllerImpl() = default;

	///	@brief	����
	nDouble Update(natStopWatch& watch) override;

	///	@brief	���FPS����
	nuInt GetFPSLimit() const override;
	///	@brief	����FPS����
	void SetFPSLimit(nuInt FPSLimit) override;

	///	@brief	��õ�ǰFPS
	nDouble GetFPS() const override;

	///	@brief	����ܹ�֡��
	nuInt GetTotalFrame() const override;
	///	@brief	����ܹ�ʱ��
	nDouble GetTotalTime() const override;

	///	@brief	���ƽ��FPS
	nDouble GetAvgFPS() const override;
	///	@brief	������FPS
	nDouble GetMaxFPS() const override;

	///	@brief	��ǰFPS�����ԣ�
	Property<nDouble> FPS{ [this] { return this->GetFPS(); } };

	///	@brief	FPS���ƣ����ԣ�
	Property<nuInt> FPSLimit{ [this] { return this->GetFPSLimit(); }, [this](nuInt const& value) { this->SetFPSLimit(value); } };
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