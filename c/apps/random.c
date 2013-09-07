#include <stdlib.h>

#include "main.h"
#include "app.h"
#include "lights.h"

void random_update (uint32_t* lights, int len) {
	int i;
	uint32_t new_color;

	//First, shuffle all the current colors down one spot on the strip
	for (i = (len - 1); i>0; i--) {
		lights[i] = lights[i - 1];
	}

	//Now form a new RGB color
	new_color = 0;
	for (i=0; i<3; i++) {
		new_color <<= 8;
		new_color |= (rand() % 0xFF) & 0xFF; //Give me a number from 0 to 0xFF
	}

	lights[0] = new_color; //Add the new random color to the strip
}


static int random_init () {
	struct timeval period = {0, 100000};

	register_continuous(period, random_update);

	return 0;
}

__attribute__ ((constructor))
static void register_random_init(void) {
	register_init_fn(random_init, "random");
}
