#pragma once

#include <Windows.h>

class TickClock {
public:
	TickClock();
	~TickClock();

	void begin();
	void end();
	double elapsed();

private:
	LARGE_INTEGER t1_;
	LARGE_INTEGER t2_;

private:
	static LARGE_INTEGER s_freq;
};