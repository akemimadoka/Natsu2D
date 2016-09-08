#include "n2dFPSControllerImpl.h"
#include <thread>

n2dFPSControllerImpl::n2dFPSControllerImpl(nuInt iFPSLimit)
	: m_TotalFrame(0u),
	m_TotalTime(0.),
	m_MaxFPS(0.),
	m_CurFPS(0.),
	m_FramesTime(0.),
	m_FPSstat(0u),
	m_FPSLimit(iFPSLimit),
	m_FrameDelay(iFPSLimit ? 1./iFPSLimit: 0)
{
}

nDouble n2dFPSControllerImpl::Update(natStopWatch& watch)
{
	auto tElpasedTime = watch.GetElpased();

	if (m_FPSLimit && tElpasedTime < m_FrameDelay)
	{
		auto tSleepTime = static_cast<nuInt>((m_FrameDelay - tElpasedTime) * 1000.);

		// Sleep限速
		if (tSleepTime > 1)
			std::this_thread::sleep_for(std::chrono::milliseconds(tSleepTime));

		// 自旋精准限速
		while ((tElpasedTime = watch.GetElpased()) < m_FrameDelay)
			std::this_thread::yield();
	}

	tElpasedTime = watch.GetElpased();
	watch.Reset();

	++m_TotalFrame;
	m_TotalTime += tElpasedTime;

	m_FramesTime += tElpasedTime;
	++m_FPSstat;

	if (m_FramesTime > 1.)
	{
		m_CurFPS = m_FPSstat / m_FramesTime;
		m_FPSstat = 0u;
		m_FramesTime = 0.;

		if (m_CurFPS > m_MaxFPS)
			m_MaxFPS = m_CurFPS;
	}

	return tElpasedTime;
}

nuInt n2dFPSControllerImpl::GetFPSLimit() const
{
	return m_FPSLimit;
}

void n2dFPSControllerImpl::SetFPSLimit(nuInt FPSLimit)
{
	m_FPSLimit = FPSLimit;
	m_FrameDelay = m_FPSLimit ? 1. / m_FPSLimit : 0;
}

nDouble n2dFPSControllerImpl::GetFPS() const
{
	return m_CurFPS;
}

nuInt n2dFPSControllerImpl::GetTotalFrame() const
{
	return m_TotalFrame;
}

nDouble n2dFPSControllerImpl::GetTotalTime() const
{
	return m_TotalTime;
}

nDouble n2dFPSControllerImpl::GetAvgFPS() const
{
	return m_FramesTime != 0. ? m_TotalFrame / m_TotalTime : 0;
}

nDouble n2dFPSControllerImpl::GetMaxFPS() const
{
	return m_MaxFPS;
}
