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
#include "stream_receiver.h"

// Base query that wants a profile_id key and a seq_no key. When the tcp
// streamer improves, this query can just query for the correct profile.
char query[] = "{\"profile_id\":\"U8H29zqH0\",\"seq_no\":1}";

struct timespec trace_gap_time = {0, 200000000};
struct timespec fade_gap_time = {0, 20000000};
struct timeval trace_gap_time_tv = {0, 200000};


#define STRIP_LENGTH 32
uint32_t tracer_colors[STRIP_LENGTH];
uint32_t entrance_colors[STRIP_LENGTH];

// Update the colors for the simple tracer application
void tracer_update () {
	static int dot = 0;
	static int rising = 1;
	int i;

	// Set all blue
	for (i=0; i<STRIP_LENGTH; i++) {
		tracer_colors[i] = LIGHTS_BLUE;
	}
	// Set the tracer to yellow
	tracer_colors[dot] = LIGHTS_YELLOW;
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
}

// Set the lights to a certain color and then fade to white
void fade_to_white (uint32_t color) {
	int i;
	uint8_t r, g, b;

	for (i=0; i<STRIP_LENGTH; i++) {
		entrance_colors[i] = color;
	}
	lights_set(entrance_colors, STRIP_LENGTH);
	nanosleep(&fade_gap_time, NULL);

	while (1) {
		r = (color >> 16) & 0xFF;
		g = (color >> 8) & 0xFF;
		b = color & 0xFF;

		if (!r && !g && !b) break;

		if (r) r--;
		if (g) g--;
		if (b) b--;

		color = (r << 16) | (g << 8) | b;

		for (i=0; i<STRIP_LENGTH; i++) {
			entrance_colors[i] = color;
		}
		lights_set(entrance_colors, STRIP_LENGTH);
		nanosleep(&fade_gap_time, NULL);
	}
}

// Receive the data packet from the streamer and set the lights to a color
// depending on who entered and how.
void entry_update (int socket) {
	json_object* d;
	json_object* type;
	const char* str;

	d = streamer_receive(socket);
	if (d == NULL) {
		// Got a bad packet of some sort.
		fprintf(stderr, "Bad streamer packet.\n");
		return;
	}

	type = json_object_object_get(d, "type");
	if (!type) return;
	str = json_object_get_string(type);
	if (strcmp(str, "udp") == 0) {
		// remote unlock
		fade_to_white(LIGHTS_PURPLE);

	} else if (strcmp(str, "udp_invalid")) {
		// remove unlock with wrong password
		fade_to_white(LIGHTS_RED);

	} else if (strcmp(str, "rfid")) {
		// someone swiped
		fade_to_white(LIGHTS_BLUE);

	} else if (strcmp(str, "rfid_invalid")) {
		// invalid rfid card
		fade_to_white(LIGHTS_RED);
	}
}

int main (int argc, char** argv) {
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

		if (ret == -1) {
			// An error occurred with select
			fprintf(stderr, "Select error.\n");
			fprintf(stderr, "%s\n", strerror(errno));
		
		} else if (ret == 0) {
			// select timeout
			tracer_update();
		
		} else {
			// File drescriptor ready
			if (ret > 1) {
				// Uh oh, more than one descriptor ready, not sure what happened
				fprintf(stderr, "More than 1 fd ready: %i\n", ret);
			} else {
				if (FD_ISSET(stream_socket, &rfds)) {
					entry_update(stream_socket);
				} else {
					fprintf(stderr, "Some other file descriptor ready??\n");
				}
			}
		}
	}

	return 0;
}
