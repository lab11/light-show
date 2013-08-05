#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <json/json.h>
#include <errno.h>

#include "lights.h"

struct timespec sleep_time = {10, 0};


#define STRIP_LENGTH 350
uint32_t c[STRIP_LENGTH] = {0};


int main (int argc, char** argv) {
	int result;
	int i, j;

	result = lights_init();
	if (result != 0) {
		return 1;
	}

	// Set all tracer lights off
	for (i=0; i<STRIP_LENGTH; i++) {
		c[i] = LIGHTS_BLACK;
	}

	for (i=0; i<35; i++) {
		for (j=i; j<350; j+=35) {
			c[j] = LIGHTS_WHITE;
		}
		lights_set(c, STRIP_LENGTH);
		nanosleep(&sleep_time, NULL);
	}


	return 0;
}
