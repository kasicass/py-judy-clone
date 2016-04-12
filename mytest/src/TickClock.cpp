#include "TickClock.hpp"

LARGE_INTEGER TickClock::s_freq;

TickClock::TickClock() {
	if (s_freq.QuadPart == 0) {
		QueryPerformanceFrequency(&s_freq);
	}
}

TickClock::~TickClock() {
}

void TickClock::begin() {
	QueryPerformanceCounter(&t1_);
}

void TickClock::end() {
	QueryPerformanceCounter(&t2_);
}

double TickClock::elapsed() {
	return double(t2_.QuadPart) / double(s_freq.QuadPart) - double(t1_.QuadPart) / double(s_freq.QuadPart);
}
