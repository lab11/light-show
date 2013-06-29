#ifndef __BASE_H__
#define __BASE_H__

#define CLOCK RPI_V2_GPIO_P1_10
#define DATA  RPI_V2_GPIO_P1_26
#define LED0  RPI_V2_GPIO_P1_07
#define LED1  RPI_V2_GPIO_P1_12
#define LED2  RPI_V2_GPIO_P1_13


#define STRIP_LENGTH 32 //32 LEDs on this strip
extern long strip_colors[STRIP_LENGTH];

//Takes the current strip color array and pushes it out
void set ();

int init ();

#endif
