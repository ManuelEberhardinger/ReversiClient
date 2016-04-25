#include "Timer.h"

#ifdef _WIN32
void Timer::start()
{
	//Frequenz holen 
	if (!QueryPerformanceFrequency((LARGE_INTEGER*)&g_Frequency))
		std::cout << "Performance Counter nicht vorhanden" << std::endl;

	//1. Messung 
	QueryPerformanceCounter((LARGE_INTEGER*)&g_CurentCount);
}

void Timer::stop()
{
	//2. Messung 
	QueryPerformanceCounter((LARGE_INTEGER*)&g_LastCount);
}

double Timer::getTime()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&g_LastCount);
	double dTimeDiff = (((double)(g_LastCount - g_CurentCount)) / ((double) g_Frequency));
	dTimeDiff *= 1000;
	return dTimeDiff;
}

std::ostream & operator<<(std::ostream& os, const Timer &t)
{
	double dTimeDiff = (((double)(t.g_LastCount - t.g_CurentCount)) / ((double)t.g_Frequency));
	dTimeDiff *= 1000;
	std::string ext = "ms";

	os << dTimeDiff << " " << ext;

	return os;
}
#else
void Timer::start() {
	id = CLOCK_MONOTONIC;
	clock_gettime(id, &ts_start);
}
void Timer::stop() {
	clock_gettime(id, &ts_end);
}

double Timer::getTime(){
	clock_gettime(id, &ts_end);
	unsigned long time = (ts_end.tv_sec - ts_start.tv_sec) * 1000000000 + ts_end.tv_nsec - ts_start.tv_nsec;
	time /= 1000; //us
	time /= 1000; //ms
	return (double) time;
}

std::ostream & operator<<(std::ostream& os, const Timer &t)
{
	unsigned long ms = (t.ts_end.tv_sec - t.ts_start.tv_sec) * 1000000000 + t.ts_end.tv_nsec - t.ts_start.tv_nsec;
	ms /= 1000;
	ms /= 1000;
	os << ms << " ms";
	return os;
}

#endif 