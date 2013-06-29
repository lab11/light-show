#include <unistd.h>
#include <time.h>

#include "base.h"

struct timespec trace_gap_time = {0, 200000000};

int main (int argc, char** argv) {
	int dot = 0;
	int rising = 1;
	int i;

	init();

	while (1) {
		// Set all blue
		for (i=0; i<STRIP_LENGTH; i++) {
			strip_colors[i] = 0x0000FF;
		}
		// Set the tracer to yellow
		strip_colors[dot] = 0xFFFF00;
		set();

		if (rising == 1) {
			dot = (dot + 1) % STRIP_LENGTH;
			if (dot == 0) {
				dot = STRIP_LENGTH - 2;
				rising = 0;
			}
		} else {
			dot--;
			if (dot < 0) {
				dot = 1;
				rising = 1;
			}
		}

		nanosleep(&trace_gap_time, NULL);
	}

	return 0;
}
