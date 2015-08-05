#include<Windows.h>

class Time{
private:
	double m_secondsPerCount;
	double m_deltaTimer;

	_int64 m_baseTime;
	_int64 m_pausedTime;
	_int64 m_stopTime;
	_int64 m_prevTime;
	_int64 m_currTime;

	bool m_stopped;

public:
	Time() : m_secondsPerCount(0.0), m_deltaTimer(-1.0), m_baseTime(0), m_pausedTime(0), m_currTime(0), m_prevTime(0), m_stopped(false)
	{
		_int64 m_countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&m_countsPerSec);
		m_secondsPerCount = 1.0 / (double)m_countsPerSec;
	}; 
	float DeltaTimer()const		{ return m_deltaTimer; } // in seconds

	//Call before message loop
	void Reset()
	{
		_int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_baseTime = currTime;
		m_prevTime = currTime;
		m_stopTime = 0;
		m_stopped = false;
	}

	//Call when unpaused
	void Start()
	{
		_int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		if (m_stopped)
		{
			m_pausedTime += (startTime - m_stopTime);
			m_prevTime = startTime;
			m_stopTime = 0;
			m_stopped = false;
		}
	}
	//Call when paused
	void Stop()
	{
		if (m_stopped)
		{
			_int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
			m_stopTime = currTime;
			m_stopped = false;
		}
	}

	//Call every frame
	void Tick() 
	{
		if (m_stopped)
		{
			m_deltaTimer = 0.0;
			return;
		}

		_int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_currTime = currTime;

		//time difference between frames
		m_deltaTimer = (m_currTime - m_prevTime)*m_secondsPerCount;

		//prepare for next frame
		m_prevTime = m_currTime;

		//m_deltaTimer must not be a negative value
		if (m_deltaTimer < 0.0)		m_deltaTimer = 0.0;
	}

	float TotalTime()const
	{
		if (m_stopped)	{ return (float)(((m_stopTime - m_pausedTime) - m_baseTime)*m_secondsPerCount); }
		else { return (float)(((m_currTime - m_pausedTime) - m_baseTime)*m_secondsPerCount); }
	}

	bool GetStoped()
	{
		return m_stopped;
	}

};