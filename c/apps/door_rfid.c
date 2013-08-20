#include <json/json.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "app.h"
#include "stream_receiver.h"
#include "effects.h"
#include "lights.h"
#include "door_rfid.h"
#include "people.h"

int stream_socket;

// Base query that wants a specific profile_id, which is the data from the
// door controller.
char query[] = "{\"profile_id\":\"U8H29zqH0i\"}";

#define NUMBER_PEOPLE sizeof(uniqname_actions) / sizeof(rfid_action_t)
rfid_action_t uniqname_actions[] = {{"bradjc", bradjc_enter},
                                    {"nklugman", nklugman_enter},
                                    {"wwhuang", wwhuang_enter},
                                    {"zakir", zakir_enter}};

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
		effects_fade_slow(LIGHTS_PURPLE, LIGHTS_WHITE, len);

	} else if (strcmp(str, "udp_failed") == 0) {
		// remove unlock with wrong password
		effects_fade_slow(LIGHTS_RED, LIGHTS_WHITE, len);

	} else if (strcmp(str, "rfid") == 0) {
		// someone swiped
		json_object* json_uniqname;
		char* uniqname;

		json_uniqname = json_object_object_get(d, "uniqname");
		if (json_uniqname) {
			uint8_t match = 0;
			int i;
			uniqname = json_object_get_string(json_uniqname);
			for (i=0; i<NUMBER_PEOPLE; i++) {
				if (strcmp(uniqname, uniqname_actions[i].uniqname) == 0) {
					uniqname_actions[i].enter_fn(len);
					match = 1;
				}
			}
			if (!match) {
				effects_grow(LIGHTS_RED, len);
			}
		}

	} else if (strcmp(str, "rfid_invalid") == 0) {
		// invalid rfid card
		effects_fade_slow(LIGHTS_RED, LIGHTS_WHITE, len);
	}
}
