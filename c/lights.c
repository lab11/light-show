#include <bcm2835.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "lights.h"

struct timespec reset_time = {0, 500000};


//Takes the current strip color array and pushes it out
void lights_set (uint32_t* colors, uint16_t length) {
	//Each LED requires 24 bits of data
	//MSB: R7, R6, R5..., G7, G6..., B7, B6... B0
	//Once the 24 bits have been delivered, the IC immediately relays these
	//bits to its neighbor
	//Pulling the clock low for 500us or more causes the IC to post the data.
#ifndef DEBUG
	int led;
	uint32_t color;
	uint8_t color_bit;
	uint32_t mask;

	for (led=0; led<length; led++) {
		color = colors[led]; //24 bits of color data

		for (color_bit=0; color_bit<24; color_bit++) {
			bcm2835_gpio_clr(CLOCK); //Only change data when clock is low

			mask = 1L << color_bit;

			if (color & mask) {
				bcm2835_gpio_set(DATA);
			} else {
				bcm2835_gpio_clr(DATA);
			}

			bcm2835_gpio_set(CLOCK);
		}
	}

	//Pull clock low to put strip into reset/post mode
	bcm2835_gpio_clr(CLOCK);
	nanosleep(&reset_time, NULL);

#else // Debug mode
	int i;
	for (i=0; i<length; i++) {
		switch (colors[i]) {
			case LIGHTS_BLUE: printf("\e[48;5;21m "); break;
			case LIGHTS_YELLOW: printf("\e[48;5;11m "); break;
			case LIGHTS_GREEN: printf("\e[48;5;10m "); break;
			case LIGHTS_RED: printf("\e[48;5;9m "); break;
			case LIGHTS_PURPLE: printf("\e[48;5;13m "); break;
			default: printf("?"); break;
		}
	}
	printf("\r");
	fflush(stdout);
#endif
}

int lights_init () {
#ifndef DEBUG
	int err;

	// Init the GPIO library
	err = bcm2835_init();
	if (err == 0) {
		printf("Failed to initialize the bcm gpio.\n");
		return 1;
	}

	// Init the relevant GPIO pins
	bcm2835_gpio_fsel(CLOCK, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(DATA, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(LED0, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(LED1, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(LED2, BCM2835_GPIO_FSEL_OUTP);

	bcm2835_gpio_clr(LED0);
	bcm2835_gpio_clr(LED1);
	bcm2835_gpio_clr(LED2);

	return 0;
#else
	return 0;
#endif
}
