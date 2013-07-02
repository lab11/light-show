#ifndef __BASE_H__
#define __BASE_H__

#include <stdint.h>

#define CLOCK RPI_V2_GPIO_P1_10
#define DATA  RPI_V2_GPIO_P1_26
#define LED0  RPI_V2_GPIO_P1_07
#define LED1  RPI_V2_GPIO_P1_12
#define LED2  RPI_V2_GPIO_P1_13

#define LIGHTS_BLUE 0x0000ff


//Takes the current strip color array and pushes it out
void lights_set (uint32_t* colors, uint16_t length);

int lights_init ();

#endif
