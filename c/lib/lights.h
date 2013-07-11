#ifndef __BASE_H__
#define __BASE_H__

#include <stdint.h>

#define LEDSTRIP_FILENAME "/dev/ledstrip"

#define LED0  RPI_V2_GPIO_P1_07
#define LED1  RPI_V2_GPIO_P1_12
#define LED2  RPI_V2_GPIO_P1_13

#define LIGHTS_BLUE 0x0000FF
#define LIGHTS_YELLOW 0xFFFF00
#define LIGHTS_GREEN 0x00FF00
#define LIGHTS_RED 0xFF0000
#define LIGHTS_PURPLE 0x7F00FF
#define LIGHTS_WHITE 0xFFFFFF
#define LIGHTS_TEAL 0x008080
#define LIGHTS_BLACK 0x000000
#define LIGHTS_ORANGE 0xFFA500


//Takes the current strip color array and pushes it out
void lights_set (uint32_t* colors, uint16_t length);

int lights_init ();

#endif
