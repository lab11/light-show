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
#include "effects.h"

// Base query that wants a profile_id key and a seq_no key. When the tcp
// streamer improves, this query can just query for the correct profile.
char query[] = "{\"profile_id\":\"U8H29zqH0\",\"seq_no\":1}";

struct timespec trace_gap_time = {0, 200000000};
struct timespec fade_gap_time =  {0, 20000000};
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
		effects_fade(LIGHTS_PURPLE, LIGHTS_WHITE, STRIP_LENGTH);

	} else if (strcmp(str, "udp_failed") == 0) {
		// remove unlock with wrong password
		effects_fade(LIGHTS_RED, LIGHTS_WHITE, STRIP_LENGTH);

	} else if (strcmp(str, "rfid") == 0) {
		// someone swiped
		json_object* json_uniqname;
		char* uniqname;

		json_uniqname = json_object_object_get(d, "uniqname");
		if (json_uniqname) {
			uniqname = json_object_get_string(json_uniqname);
			if (strcmp(uniqname, "bradjc") == 0) {
				effects_fade(LIGHTS_BLUE, LIGHTS_WHITE, STRIP_LENGTH);
			} else {
				effects_fade(LIGHTS_GREEN, LIGHTS_WHITE, STRIP_LENGTH);
			}
		}

	} else if (strcmp(str, "rfid_invalid") == 0) {
		// invalid rfid card
		effects_fade(LIGHTS_RED, LIGHTS_WHITE, STRIP_LENGTH);
	}
}

int main (int argc, char** argv) {
	int result;
	int i;
	int stream_socket;
	fd_set  rfds;

	result = lights_init();
	if (result != 0) {
		return 1;
	}

	// Set all tracer lights blue
	for (i=0; i<STRIP_LENGTH; i++) {
		tracer_colors[i] = LIGHTS_BLUE;
	}

	// Connect to the streaming server
	stream_socket = streamer_connect(query);

	while (1) {
		int ret;

		// Setup the select call
		trace_gap_time_tv.tv_sec = 0;
		trace_gap_time_tv.tv_usec = 200000;
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
