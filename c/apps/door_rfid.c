#include <json/json.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "app.h"
#include "stream_receiver.h"
#include "effects.h"
#include "lights.h"

int stream_socket;

// Base query that wants a specific profile_id, which is the data from the
// door controller.
char query[] = "{\"profile_id\":\"U8H29zqH0i\"}";

int door_rfid_init () {
	stream_socket = streamer_connect(query);

	register_socket(stream_socket, door_rfid_update);

	return 0;
}

// Receive the data packet from the streamer and set the lights to a color
// depending on who entered and how.
void door_rfid_update (uint32_t* lights, int len) {
	json_object* d;
	json_object* type;
	const char* str;

	d = streamer_receive(stream_socket);
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
		effects_fade(LIGHTS_PURPLE, LIGHTS_WHITE, len);

	} else if (strcmp(str, "udp_failed") == 0) {
		// remove unlock with wrong password
		effects_fade(LIGHTS_RED, LIGHTS_WHITE, len);

	} else if (strcmp(str, "rfid") == 0) {
		// someone swiped
		json_object* json_uniqname;
		char* uniqname;

		json_uniqname = json_object_object_get(d, "uniqname");
		if (json_uniqname) {
			uniqname = json_object_get_string(json_uniqname);
			if (strcmp(uniqname, "bradjc") == 0) {
				//effects_fade(LIGHTS_BLUE, LIGHTS_WHITE, len);
				effects_grow(LIGHTS_BLUE, len);
			} else if (strcmp(uniqname, "nklugman") == 0) {
				effects_blink(LIGHTS_WHITE, LIGHTS_PURPLE, len);
			} else if (strcmp(uniqname, "wwhuang") == 0) {
				effects_fade(LIGHTS_TEAL, LIGHTS_RED, len);
			} else if (strcmp(uniqname, "zakir") == 0) {
				effects_grow(LIGHTS_ORANGE, len);
			} else {
				effects_grow(LIGHTS_RED, len);
			}
		}

	} else if (strcmp(str, "rfid_invalid") == 0) {
		// invalid rfid card
		effects_fade(LIGHTS_RED, LIGHTS_WHITE, len);
	}
}
