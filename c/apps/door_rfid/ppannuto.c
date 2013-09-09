#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "people.h"
#include "lights.h"
#include "effects.h"

#define MAX_ACTIVE_EXPLOSIONS 4

static uint32_t mix(uint32_t base, uint32_t mix_in) {
	return base + mix_in;
}

static inline uint32_t fade(uint32_t color, const float amt) {
	unsigned red, green, blue;

	red   = (GETRED(color)  - 1) * amt;
	green = (GETGREEN(color)- 1) * amt;
	blue  = (GETBLUE(color) - 1) * amt;

	return MAKECOLOR(red, green, blue);
}

static inline uint32_t fade2(uint32_t color, const float amt) {
	return fade(fade(color, amt), amt);
}

static bool explosion0(uint32_t lights[], int len, bool restartable) {
	static bool exploded = false;
	static unsigned color;
	static int center;
	static int count = 0;
	const float fade_amt = .9;

	if (!exploded && !restartable)
		return false;

	if (!exploded) {
		static int halver = 0;
		if ((halver++ % 2) != 0)
			return false;
		long int r = random();
		if ((r % 16) != 0)
			return false;

		color = lights_colors[(r >> 8) % LIGHTS_NUM_COLORS];

		center = ((r >> 16) % (len / 2)) + (len / 4);

		lights[center] = color;
		exploded = true;
		return true;
	}
	
	if (count == 0) {
		lights[center] = fade(color, fade_amt);
		lights[center+1] = color;
		lights[center-1] = color;
		count++;
		return true;
	}

	if (lights[center])
		lights[center] = fade2(lights[center], fade_amt);

	int i;
	if (lights[center + count] == 0)
		count++;
	if (lights[center + count] == 0) {
		exploded = false;
		count = 0;
		memset(lights, 0, len * sizeof(uint32_t));
		return false;
	}
	for (i = (center + count); ; i++) {
		if (lights[i+1] == 0) {
			lights[i+1] = fade(lights[i], fade_amt);
			lights[i] = fade2(lights[i], fade_amt);
			break;
		} else {
			lights[i] = fade(lights[i], fade_amt);
		}
	}
	for (i = (center - count); ; i--) {
		if (lights[i-1] == 0) {
			lights[i-1] = fade(lights[i], fade_amt);
			lights[i] = fade2(lights[i], fade_amt);
			break;
		} else {
			lights[i] = fade(lights[i], fade_amt);
		}
	}

	return true;
}

static bool explosion1(uint32_t lights[], int len, bool restartable) {
	static bool exploded = false;
	static unsigned color;
	static int center;
	static int count = 0;
	const float fade_amt = .8;

	if (!exploded && !restartable)
		return false;

	if (!exploded) {
		long int r = random();
		if ((r % 8) != 0)
			return false;

		color = lights_colors[(r >> 8) % LIGHTS_NUM_COLORS];

		center = ((r >> 16) % (len / 2)) + (len / 4);

		lights[center] = color;
		exploded = true;
		return true;
	}
	
	if (count == 0) {
		lights[center] = fade(color, fade_amt);
		lights[center+1] = color;
		lights[center-1] = color;
		count++;
		return true;
	}

	if (lights[center])
		lights[center] = fade2(lights[center], fade_amt);

	int i;
	if (lights[center + count] == 0)
		count++;
	if (lights[center + count] == 0) {
		exploded = false;
		count = 0;
		memset(lights, 0, len * sizeof(uint32_t));
		return false;
	}
	for (i = (center + count); ; i++) {
		if (lights[i+1] == 0) {
			lights[i+1] = fade(lights[i], fade_amt);
			lights[i] = fade2(lights[i], fade_amt);
			break;
		} else {
			lights[i] = fade(lights[i], fade_amt);
		}
	}
	for (i = (center - count); ; i--) {
		if (lights[i-1] == 0) {
			lights[i-1] = fade(lights[i], fade_amt);
			lights[i] = fade2(lights[i], fade_amt);
			break;
		} else {
			lights[i] = fade(lights[i], fade_amt);
		}
	}

	return true;
}

static bool explosion2(uint32_t lights[], int len, bool restartable) {
	static bool exploded = false;
	static unsigned color;
	static int center;
	static int count = 0;
	const float fade_amt = .6;

	if (!exploded && !restartable)
		return false;

	if (!exploded) {
		long int r = random();
		if ((r % 4) != 0)
			return false;

		color = lights_colors[(r >> 8) % LIGHTS_NUM_COLORS];

		center = ((r >> 16) % (len / 2)) + (len / 4);

		lights[center] = color;
		exploded = true;
		return true;
	}
	
	if (count == 0) {
		lights[center] = fade(color, fade_amt);
		lights[center+1] = color;
		lights[center-1] = color;
		count++;
		return true;
	}

	if (lights[center])
		lights[center] = fade2(lights[center], fade_amt);

	int i;
	if (lights[center + count] == 0)
		count++;
	if (lights[center + count] == 0) {
		exploded = false;
		count = 0;
		memset(lights, 0, len * sizeof(uint32_t));
		return false;
	}
	for (i = (center + count); ; i++) {
		if (lights[i+1] == 0) {
			lights[i+1] = fade(lights[i], fade_amt);
			lights[i] = fade2(lights[i], fade_amt);
			break;
		} else {
			lights[i] = fade(lights[i], fade_amt);
		}
	}
	for (i = (center - count); ; i--) {
		if (lights[i-1] == 0) {
			lights[i-1] = fade(lights[i], fade_amt);
			lights[i] = fade2(lights[i], fade_amt);
			break;
		} else {
			lights[i] = fade(lights[i], fade_amt);
		}
	}

	return true;
}

static bool explosion3(uint32_t lights[], int len, bool restartable) {
	static bool exploded = false;
	static unsigned color;
	static int center;
	static int count = 0;
	const float fade_amt = .95;

	if (!exploded && !restartable)
		return false;

	if (!exploded) {
		static int quarterer = 0;
		if ((quarterer++ % 4) != 0)
			return false;
		long int r = random();
		if ((r % 16) != 0)
			return false;

		color = lights_colors[(r >> 8) % LIGHTS_NUM_COLORS];

		center = ((r >> 16) % (len / 2)) + (len / 4);

		lights[center] = color;
		exploded = true;
		return true;
	}
	
	if (count == 0) {
		lights[center] = fade(color, fade_amt);
		lights[center+1] = color;
		lights[center-1] = color;
		count++;
		return true;
	}

	if (lights[center])
		lights[center] = fade2(lights[center], fade_amt);

	int i;
	if (lights[center + count] == 0)
		count++;
	if (lights[center + count] == 0) {
		exploded = false;
		count = 0;
		memset(lights, 0, len * sizeof(uint32_t));
		return false;
	}
	for (i = (center + count); ; i++) {
		if (lights[i+1] == 0) {
			lights[i+1] = fade(lights[i], fade_amt);
			lights[i] = fade2(lights[i], fade_amt);
			break;
		} else {
			lights[i] = fade(lights[i], fade_amt);
		}
	}
	for (i = (center - count); ; i--) {
		if (lights[i-1] == 0) {
			lights[i-1] = fade(lights[i], fade_amt);
			lights[i] = fade2(lights[i], fade_amt);
			break;
		} else {
			lights[i] = fade(lights[i], fade_amt);
		}
	}

	return true;
}

void ppannuto_enter (int len) {
	uint32_t lights[len];

	memset(lights, 0, len*sizeof(uint32_t));
	lights_set(lights, len);

	uint32_t explosions[MAX_ACTIVE_EXPLOSIONS][len];
	memset(explosions, 0, MAX_ACTIVE_EXPLOSIONS * len*sizeof(uint32_t));

	////////////////////////////////////////////////////////////////////
	
	int count = 125;
	while (true) {
		bool any_running = false;
		any_running |= explosion0(explosions[0], len, count > 0);
		any_running |= explosion1(explosions[1], len, count > 0);
		any_running |= explosion2(explosions[2], len, count > 0);
		any_running |= explosion3(explosions[3], len, count > 0);

		count--;
		if (((count < 0) && !any_running) || (count < -200))
			break;

		usleep(5000);

		unsigned i, j;
		for (i=0; i < len; i++) {
			lights[i] = 0;
			for (j=0; j < MAX_ACTIVE_EXPLOSIONS; j++) {
				lights[i] = mix(lights[i], explosions[j][i]);
			}
		}
		lights_set(lights, len);

	}
}
