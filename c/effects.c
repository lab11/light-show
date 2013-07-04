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

//	r = rs;
//	g = gs;
//	b = bs;

	for (i=0; i<=num_steps; i++) {
		r = interpolate(rs, re, i, num_steps);
		g = interpolate(gs, ge, i, num_steps);
		b = interpolate(bs, be, i, num_steps);

	//	if (r < 255) r++;
	//	if (g < 255) g++;
	//	if (b < 255) b++;

		color = MAKECOLOR(r, g, b);
		//color = 0x0ff7373;

		for (j=0; j<num_lights; j++) {
			l[j] = color;
		}

		lights_set(l, num_lights);
		nanosleep(&fade_time, NULL);
	}

	//lights_set(l, num_lights);
	//nanosleep(&fade_time, NULL);

	free(l);
/*
	uint32_t l[32] = {LIGHTS_YELLOW};
	l[0] = LIGHTS_BLUE;
	l[1] = LIGHTS_RED;
	l[2] = LIGHTS_GREEN;
	l[3] = LIGHTS_PURPLE;
	l[4] = LIGHTS_WHITE;
	l[5] = LIGHTS_YELLOW;
	/*l[6] = 0x00ffaaaa;
	l[7] = 0x00ffafaf;
	l[8] = 0x00ffb4b4;
	l[9] = 0x00ffb9b9;
	l[10] = 0x00ffbebe;
	l[11] = 0x00ffc3c3;
	l[12] = 0x00ffc8c8;
	l[13] = 0x00ffcdcd;
	l[14] = 0x00ffd2d2;
	l[15] = 0x00ffd7d7;
	l[16] = 0x00ffdcdc;
	l[17] = 0x00ffe1e1;
	l[18] = 0x00ffe6e6;
	l[19] = 0x00ffebeb;
	l[20] = 0x00fff0f0;
	l[21] = 0x00ff0000;
	l[22] = 0x00ff00FF;
	l[23] = 0x0FF8787;
	l[24] = 0xFF8F8F;
	l[25] = 0xFF9797;
	l[26] = 0xFF9F9F;
	l[27] = 0xFFA7A7;
	l[28] = 0xFFAFAF;
	l[29] = 0xFFB7B7;
	l[30] = 0xFFBFBF;
	l[31] = 0xFFC7C7;
	l[0] = 0xFFCFCF;
	l[1] = 0xFFD7D7;
	l[2] = 0xFFDFDF;
	l[3] = 0xFFE7E7;
	l[4] = 0xFFEFEF;
	l[5] = 0xFFF7F7;
	l[6] = 0xFFFFFF;

	lights_set(l, num_lights);
	nanosleep(&fade_time, NULL);*/
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

