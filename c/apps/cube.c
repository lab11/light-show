#include <stdlib.h>

#include "main.h"
#include "app.h"
#include "lights.h"

#define TOP_LEFT 85
#define TOP_RIGHT 278

uint32_t colors[3] = {LIGHTS_RED, LIGHTS_GREEN, LIGHTS_BLUE};

uint8_t orientation = 0;

int cube_init () {
	struct timeval period = {10, 0};

	register_continuous(period, cube_update);

	return 0;
}

void cube_update (uint32_t* lights, int len) {
	int i;

	for (i=0; i<TOP_LEFT; i++) {
		lights[i] = colors[orientation];
	}

	for (i=TOP_LEFT; i<TOP_RIGHT; i++) {
		lights[i] = colors[(orientation+1) % 3];
	}

	for (i=TOP_RIGHT; i<len; i++) {
		lights[i] = colors[(orientation+2) % 3];
	}

	orientation = (orientation+1) % 3;
}
