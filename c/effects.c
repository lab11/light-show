#include <stdint.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>

#include "lights.h"
#include "effects.h"

#define GETRED(x) ((x>>16)&0xFF)
#define GETGREEN(x) ((x>>8)&0xFF)
#define GETBLUE(x) (x&0xFF)
#define MAKECOLOR(r,g,b) (((r&0xFF)<<16)|((g&0xFF)<<8)|(b&0xFF))

struct timespec fade_time =  {0, 100000000};

void effects_fade (uint32_t start_color, uint32_t end_color, uint16_t num_lights) {
	uint32_t* l;
	uint32_t color;
	uint8_t r, g, b;
	uint8_t rs, gs, bs;
	uint8_t re, ge, be;
	uint8_t num_steps = 100;
	uint8_t i;
	uint16_t j;

	l = malloc(sizeof(uint32_t) * num_lights);

	rs = GETRED(start_color);
	gs = GETGREEN(start_color);
	bs = GETBLUE(start_color);
	re = GETRED(end_color);
	ge = GETGREEN(end_color);
	be = GETBLUE(end_color);

	for (i=0; i<=num_steps; i++) {
		r = interpolate(rs, re, i, num_steps);
		g = interpolate(gs, ge, i, num_steps);
		b = interpolate(bs, be, i, num_steps);

		color = MAKECOLOR(r, g, b);

		for (j=0; j<num_lights; j++) {
			l[j] = color;
		}

		lights_set(l, num_lights);
		nanosleep(&fade_time, NULL);
	}

	free(l);
}







// Internal helper functions

uint8_t interpolate (uint8_t hue_start,
                     uint8_t hue_end,
                     uint8_t step_idx,
                     uint8_t num_steps) {
	if (hue_end > hue_start) {
		return (((hue_end - hue_start) / num_steps) * step_idx) + hue_start;
	} else {
		return hue_start - (((hue_start - hue_end) / num_steps) * step_idx);
	}
}

