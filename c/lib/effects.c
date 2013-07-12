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

struct timespec fade_time = {0, 100000000};
struct timespec blink_time = {0, 100000000};
struct timespec grow_time = {1, 0};

void effects_fade (uint32_t start_color, uint32_t end_color, uint16_t num_lights) {
	uint32_t* l;
	uint32_t color;
	uint8_t num_steps = 100;
	uint16_t i, j;

	l = malloc(sizeof(uint32_t) * num_lights);

	for (i=0; i<=num_steps; i++) {
		color = _effects_interpolate_color(start_color, end_color, i, num_steps);

		for (j=0; j<num_lights; j++) {
			l[j] = color;
		}

		lights_set(l, num_lights);
		nanosleep(&fade_time, NULL);
	}

	free(l);
}

void effects_blink (uint32_t color1, uint32_t color2, uint16_t num_lights) {
	uint32_t* l;
	uint16_t i, j;
	uint8_t num_steps = 25;

	l = malloc(sizeof(uint32_t) * num_lights);

	for (i=0; i<num_steps; i++) {
		for (j=0; j<num_lights; j++) {
			l[j] = (i%2) ? color2 : color1;
		}
		lights_set(l, num_lights);
		nanosleep(&blink_time, NULL);
	}
	free(l);
}

void effects_grow (uint32_t color, uint16_t num_lights) {
	uint32_t* l;
	uint16_t i, j;
	uint16_t num_blocks = num_lights / 8; // 8 lights per block

	l = malloc(sizeof(uint32_t) * num_lights);

	// Turn off all lights first
	for (j=0; j<num_lights; j++) {
		l[j] = LIGHTS_BLACK;
	}

	for (i=0; i<num_blocks; i++) {
		uint32_t c = _effects_interpolate_color(color, LIGHTS_WHITE, i, num_blocks);
		for (j=(i*8); j<((i+1)*8); j++) {
			l[j] = c;
		}
		lights_set(l, num_lights);
		nanosleep(&grow_time, NULL);
	}
	
	free(l);
}







// Internal helper functions

uint8_t _effects_interpolate (uint8_t hue_start,
		                      uint8_t hue_end,
		                      uint8_t step_idx,
		                      uint8_t num_steps) {
	if (hue_end > hue_start) {
		return (((hue_end - hue_start) / num_steps) * step_idx) + hue_start;
	} else {
		return hue_start - (((hue_start - hue_end) / num_steps) * step_idx);
	}
}

uint32_t _effects_interpolate_color (uint32_t color_start,
                                     uint32_t color_end,
                                     uint8_t step_idx,
                                     uint8_t num_steps) {
	uint8_t rs, gs, bs;
	uint8_t re, ge, be;
	uint8_t r, g, b;

	rs = GETRED(color_start);
	gs = GETGREEN(color_start);
	bs = GETBLUE(color_start);
	re = GETRED(color_end);
	ge = GETGREEN(color_end);
	be = GETBLUE(color_end);

	r = _effects_interpolate(rs, re, step_idx, num_steps);
	g = _effects_interpolate(gs, ge, step_idx, num_steps);
	b = _effects_interpolate(bs, be, step_idx, num_steps);

	return MAKECOLOR(r, g, b);
}
