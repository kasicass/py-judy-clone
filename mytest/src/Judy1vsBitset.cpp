// Judy1 is a little bit faster than std::bitset
// and less memory consumption
//
// when MAX_COUNT = 10000000, bitset fails to init.
// use stack for memory?
#include <Judy.h>
#include <bitset>
#include "TickClock.hpp"

int main()
{
	const int MAX_COUNT = 5000000;
	TickClock clock;
	double t1, t2;
	int Rc_int;
	int i;

	// Judy1
	Pvoid_t addrArray = (Pvoid_t) NULL;
	clock.begin();
	for(i = 0; i < MAX_COUNT; i++) {
		J1S(Rc_int, addrArray, (Word_t) i);
	}

    for(i = 0; i < MAX_COUNT; i++) {
		J1T(Rc_int, addrArray, (Word_t) i);
		if(!Rc_int){
			printf("Something bad happened\n");
			return -1;
		}
    }
	clock.end();
	t1 = clock.elapsed();

	// stl::bitset
	bool value;
	std::bitset<MAX_COUNT> bitmap;
	for (i = 0; i < MAX_COUNT; i++) {
		bitmap.set(i);
	}

	for (i = 0; i < MAX_COUNT; i++) {
		value = bitmap[i];
		if(!value){
			printf("Something bad happened (bitset)\n");
			return -1;
		}
	}
	clock.end();
	t2 = clock.elapsed();

	printf("Judy1: %f\n", t1);
	printf("bitset: %f\n", t2);
    return 0;
}
