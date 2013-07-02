#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>

#include "lights.h"
#include "stream_receiver.h"

char query[] = "{}";

struct timespec trace_gap_time = {0, 200000000};
struct timeval trace_gap_time_tv = {0, 200000};


#define STRIP_LENGTH 32
uint32_t tracer_colors[STRIP_LENGTH];
uint32_t entrance_colors[STRIP_LENGTH];

int main (int argc, char** argv) {
	int dot = 0;
	int rising = 1;
	int i;
	int stream_socket;
	fd_set  rfds;

	lights_init();

	// Set all tracer lights blue
	for (i=0; i<STRIP_LENGTH; i++) {
		tracer_colors[i] = LIGHTS_BLUE;
	}

	// Connect to the streaming server
	stream_socket = streamer_connect(query);



	while (1) {
		int ret;

		// Setup the select call
		FD_ZERO(&rfds);
		FD_SET(stream_socket, &rfds);
		ret = select(stream_socket+1, &rfds, NULL, NULL, &trace_gap_time_tv);



		// Set all blue
		for (i=0; i<STRIP_LENGTH; i++) {
			tracer_colors[i] = 0x0000FF;
		}
		// Set the tracer to yellow
		tracer_colors[dot] = 0xFFFF00;
		lights_set(tracer_colors, STRIP_LENGTH);

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
