#include "StdAfx.h"
#include <windows.h>
#include "clTimer.h"

clTimer::clTimer(void)
{
	_clocks=0.0E0;
	_start=0.0E0;
	i64 ifreq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&ifreq);
	_freq = (double) ifreq;
}

clTimer::~clTimer(void)
{

}

void clTimer::Start()
{
	i64 istart;
	QueryPerformanceCounter((LARGE_INTEGER*)&istart);
	_start = (double) istart;
}

void clTimer::Stop()
{
	double n = 0;

	i64 in;
	QueryPerformanceCounter((LARGE_INTEGER*)&in);
	n = (double) in;

	n -= _start;
	_start = 0.0E0;
	_clocks += n;
}

void clTimer::Reset()
{
	_clocks = 0.0E0;
}

double clTimer::GetElapsedTime()
{
	return _clocks / _freq;
}