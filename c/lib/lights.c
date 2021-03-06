#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lights.h"

struct timespec reset_time = {0, 500000};

int ledstrip_file;


//Takes the current strip color array and pushes it out
void lights_set (uint32_t* colors, uint16_t length) {
	//Each LED requires 24 bits of data
	//MSB: R7, R6, R5..., G7, G6..., B7, B6... B0
	//Once the 24 bits have been delivered, the IC immediately relays these
	//bits to its neighbor
	//Pulling the clock low for 500us or more causes the IC to post the data.
#ifndef DEBUG
	ssize_t result;
	int i;
	uint8_t* outbuf;

	// Allocate a byte buffer long enough to hold the outgoing color values
	outbuf = malloc(length*3);

	for (i=0; i<length; i++) {
		outbuf[i*3]     = (colors[i] >> 16) & 0xFF;
		outbuf[(i*3)+1] = (colors[i] >> 8) & 0xFF;
		outbuf[(i*3)+2] = colors[i] & 0xFF;
	}

	result = write(ledstrip_file, outbuf, length*3);
	if (result == -1) {
		fprintf(stderr, "Failed to write to ledstrip.\n");
		fprintf(stderr, "%s\n", strerror(errno));
	} else if (result != length*3) {
		fprintf(stderr, "Only sent %i bytes to the led strip", result);
		fprintf(stderr, " (intended to send %i).\n", length);
		fprintf(stderr, "Did you not send a multiple of 3??\n");
	}

	free(outbuf);

#else // Debug mode
	int i;
	for (i=0; i<length; i++) {
		switch (colors[i]) {
			case LIGHTS_BLUE: printf("\e[48;5;21m "); break;
			case LIGHTS_YELLOW: printf("\e[48;5;11m "); break;
			case LIGHTS_GREEN: printf("\e[48;5;10m "); break;
			case LIGHTS_RED: printf("\e[48;5;9m "); break;
			case LIGHTS_PURPLE: printf("\e[48;5;13m "); break;
			default: printf("?"); break;
		}
	}
	printf("\r");
	fflush(stdout);
#endif
}

void lights_off (uint16_t length) {
	uint32_t* l;
	uint16_t i;

	l = malloc(length * sizeof(uint32_t));

	for (i=0; i<length; i++) {
		l[i] = LIGHTS_BLACK;
	}
	lights_set(l, length);

	free(l);
}

int lights_init () {
#ifndef DEBUG
	int err;

	ledstrip_file = open(LEDSTRIP_FILENAME, O_WRONLY, NULL);
	if (ledstrip_file == -1) {
		fprintf(stderr, "Could not open ledstrip file.\n");
		fprintf(stderr, "%s\n", strerror(errno));
		return 1;
	}

#endif

	return 0;
}
