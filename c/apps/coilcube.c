#include <json/json.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "app.h"
#include "lights.h"
#include "stream_receiver.h"
#include "effects.h"

/* The coilcube application registers a callback whenever coilcube packets come
 * in
 *
 * @author: Brad Campbell <bradjc@umich.edu>
 */

int cc_socket;
char ccquery[] = "{\"profile_id\":\"7aiOPJapXF\", \"type\":\"coilcube_watts\"}";

time_t then = 0;

int coilcube_init () {
	struct timeval period = {0, 100000};

	cc_socket = streamer_connect(ccquery);

	register_continuous(period, coilcube_timeslot);
	register_socket(cc_socket, coilcube_pkt);

	return 0;
}

// Update the colors for the simple tracer application
void coilcube_timeslot (uint32_t* lights, int len) {
	struct timeval thentime;
	time_t now;

	gettimeofday(&thentime, NULL);
	now = thentime.tv_sec;

	if (now - then >= 1) {
		int i;
		// If it is the start of our turn turn the lights off
		for (i=0; i<len; i++) {
			lights[i] = LIGHTS_BLACK;
		}
	}

	then = now;
}

// Called when an incoming packet happens
void coilcube_pkt (uint32_t* lights, int len) {
	time_t now;
	struct timeval nowtime;
	json_object* pkt;
	json_object* type;
	const char* str;

	pkt = streamer_receive(cc_socket);
	if (pkt == NULL) return;

	// Check that it is still our time to update the lights
	gettimeofday(&nowtime, NULL);
	now = nowtime.tv_sec;
	if (now - then >= 1) {
		return;
	}

	type = json_object_object_get(pkt, "type");
	if (!type) return;
	str = json_object_get_string(type);

	if (strcmp(str, "coilcube_watts") == 0) {
		// got a converted cc packet I guess

		json_object* ccid;
		int64_t id;
		uint32_t color;

		ccid = json_object_object_get(pkt, "ccid");
		if (ccid) {
			id = json_object_get_int64(ccid);

			printf("got coilcube %li\n", id);

			color = 0xFFFFFF & id;
			effects_fade_fast(color, LIGHTS_BLACK, len);
		}
	}

}
