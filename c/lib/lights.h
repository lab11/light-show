#ifndef __BASE_H__
#define __BASE_H__

#include <stdint.h>

#define LEDSTRIP_FILENAME "/dev/ledstrip"

#define LED0  RPI_V2_GPIO_P1_07
#define LED1  RPI_V2_GPIO_P1_12
#define LED2  RPI_V2_GPIO_P1_13

#define GETRED(x) ((x>>16)&0xFF)
#define GETGREEN(x) ((x>>8)&0xFF)
#define GETBLUE(x) (x&0xFF)
#define MAKECOLOR(r,g,b) (((r&0xFF)<<16)|((g&0xFF)<<8)|(b&0xFF))

#define LIGHTS_BLACK 0x000000
#define LIGHTS_BLUE 0x0000FF
#define LIGHTS_YELLOW 0xFFFF00
#define LIGHTS_GREEN 0x00FF00
#define LIGHTS_RED 0xFF0000
#define LIGHTS_PURPLE 0x7F00FF
#define LIGHTS_WHITE 0xFFFFFF
#define LIGHTS_TEAL 0x008080
#define LIGHTS_ORANGE 0xFFA500
#define LIGHTS_MINT 0x9BEBA2
#define LIGHTS_WINTER_LAKE 0x7386BA

static const uint32_t lights_colors[] = {
	LIGHTS_BLUE,
	LIGHTS_YELLOW,
	LIGHTS_GREEN,
	LIGHTS_RED,
	LIGHTS_PURPLE,
	LIGHTS_WHITE,
	LIGHTS_TEAL,
	LIGHTS_ORANGE,
	LIGHTS_MINT,
	LIGHTS_WINTER_LAKE,
};
#define LIGHTS_NUM_COLORS ((sizeof(lights_colors)) / (sizeof(uint32_t)))

//Takes the current strip color array and pushes it out
void lights_set (uint32_t* colors, uint16_t length);

void lights_off (uint16_t length);
int lights_init ();

#endif
