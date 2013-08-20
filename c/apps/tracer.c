#include "main.h"
#include "app.h"
#include "lights.h"

// Register tracer as an application
int tracer_init () {
	struct timeval period = {0, 10000};

	register_continuous(period, tracer_update);

	return 0;
}

// Update the colors for the simple tracer application
void tracer_update (uint32_t* lights, int len) {
	static int dot = 0;
	static int rising = 1;
	int i;

	// Set all blue
	for (i=0; i<len; i++) {
		lights[i] = LIGHTS_BLUE;
		//lights[i] = LIGHTS_WHITE;
	}
	// Set the tracer to yellow
	lights[dot] = LIGHTS_YELLOW;

	if (rising == 1) {
		dot = (dot + 1) % len;
		if (dot == 0) {
			dot = len - 2;
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
