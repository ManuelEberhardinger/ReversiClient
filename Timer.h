#ifndef TIMER_H
#define TIMER_H

#include <string> 
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <ctime> 
#endif

class Timer
{
	friend std::ostream & operator<<(std::ostream& os, const Timer &t);
public:
	void start();
	void stop();
	double getTime(); //Gibt Zeit in ms zurück
private:
#ifdef _WIN32
	LONGLONG g_Frequency, g_CurentCount, g_LastCount;
#else
	timespec ts_start, ts_end;
	clockid_t id;
#endif

	
};
#endif 
