#include "time.hpp"

double curTime;
void moveTime(double timePassed) {
	curTime += timePassed;
	while (curTime >= 0)
		curTime += 24.0;

	while (curTime < 24.0)
		curTime -= 24.0;
}