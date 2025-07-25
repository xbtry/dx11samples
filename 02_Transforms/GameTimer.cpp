#include "GameTimer.h"


GameTimer::GameTimer()
	: m_secondsPerCount(0.0), m_deltaTime(-1.0), m_baseTime(0), m_pausedTime(0),
	m_stopTime(0), m_prevTime(0), m_currTime(0), m_stopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_secondsPerCount = 1.0 / static_cast<double>(countsPerSec);
}

float GameTimer::GameTime() const
{
	if (m_stopped)
	{
		return static_cast<float>((m_stopTime - m_baseTime - m_pausedTime) * m_secondsPerCount);
	}
	else
	{
		return static_cast<float>((m_currTime - m_baseTime - m_pausedTime) * m_secondsPerCount);
	}
}

float GameTimer::DeltaTime() const
{
	return static_cast<float>(m_deltaTime);
}
void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_baseTime = currTime;
	m_prevTime = currTime;
	m_stopTime = 0;
	m_pausedTime = 0;
	m_stopped = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	if (m_stopped)
	{
		m_pausedTime += startTime - m_stopTime;
		m_prevTime = startTime;
		m_stopped = false;
	}
}

void GameTimer::Stop()
{
	if (!m_stopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_stopTime = currTime;
		m_stopped = true;
	}
}

void GameTimer::Tick()
{
	if (m_stopped)
	{
		m_deltaTime = 0.0;
		return;
	}
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_currTime = currTime;
	m_deltaTime = (m_currTime - m_prevTime) * m_secondsPerCount;
	m_prevTime = m_currTime;
	if (m_deltaTime < 0.0)
	{
		m_deltaTime = 0.0;
	}
}