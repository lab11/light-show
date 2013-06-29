/*
	Nathan Seidle
	SparkFun Electronics 2011

	This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

	Controlling an LED strip with individually controllable RGB LEDs. This stuff is awesome.

	The SparkFun (individually controllable) RGB strip contains a bunch of WS2801 ICs. These
	are controlled over a simple data and clock setup. The WS2801 is really cool! Each IC has its
	own internal clock so that it can do all the PWM for that specific LED for you. Each IC
	requires 24 bits of 'greyscale' data. This means you can have 256 levels of red, 256 of blue,
	and 256 levels of green for each RGB LED. REALLY granular.

	To control the strip, you clock in data continually. Each IC automatically passes the data onto
	the next IC. Once you pause for more than 500us, each IC 'posts' or begins to output the color data
	you just clocked in. So, clock in (24bits * 32LEDs = ) 768 bits, then pause for 500us. Then
	repeat if you wish to display something new.

	This example code will display bright red, green, and blue, then 'trickle' random colors down
	the LED strip.

	You will need to connect 5V/Gnd from the Arduino (USB power seems to be sufficient).

	For the data pins, please pay attention to the arrow printed on the strip. You will need to connect to
	the end that is the begining of the arrows (data connection)--->

	If you have a 4-pin connection:
	Blue = 5V
	Red = SDI
	Green = CKI
	Black = GND

	If you have a split 5-pin connection:
	2-pin Red+Black = 5V/GND
	Green = CKI
	Red = SDI
 */

#include <bcm2835.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define CLOCK RPI_V2_GPIO_P1_10
#define DATA  RPI_V2_GPIO_P1_26
#define LED0  RPI_V2_GPIO_P1_07
#define LED1  RPI_V2_GPIO_P1_12
#define LED2  RPI_V2_GPIO_P1_13


#define STRIP_LENGTH 32 //32 LEDs on this strip
long strip_colors[STRIP_LENGTH];

struct timespec reset_time = {0, 500000};



// Throws random colors down the strip array
void add_random(void) {
	int i;
	long new_color;

	//First, shuffle all the current colors down one spot on the strip
	for (i = (STRIP_LENGTH - 1); i>0; i--) {
		strip_colors[i] = strip_colors[i - 1];
	}

	//Now form a new RGB color
	new_color = 0;
	for (i=0; i<3; i++) {
		new_color <<= 8;
		new_color |= (rand() % 0xFF) & 0xFF; //Give me a number from 0 to 0xFF
	}

	strip_colors[0] = new_color; //Add the new random color to the strip
}


//Takes the current strip color array and pushes it out
void post_frame (void) {
	//Each LED requires 24 bits of data
	//MSB: R7, R6, R5..., G7, G6..., B7, B6... B0
	//Once the 24 bits have been delivered, the IC immediately relays these bits to its neighbor
	//Pulling the clock low for 500us or more causes the IC to post the data.

	int led;
	long color;
	uint8_t color_bit;
	long mask;

	for (led=0; led<STRIP_LENGTH; led++) {
		color = strip_colors[led]; //24 bits of color data

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
}


void loop() {
	//Pre-fill the color array with known values
	strip_colors[0] = 0xFF0000; //Bright Red
	strip_colors[1] = 0x00FF00; //Bright Green
	strip_colors[2] = 0x0000FF; //Bright Blue
	strip_colors[3] = 0x010000; //Faint red
	strip_colors[4] = 0x800000; //1/2 red (0x80 = 128 out of 256)
	post_frame(); //Push the current color frame to the strip

	sleep(2);

	while (1) {
		add_random();
		post_frame(); //Push the current color frame to the strip
		sleep(1);
	}
}

int main (int arc, char** argv) {
	int err;
	int i;

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

	// Init the string
	for(i=0; i<STRIP_LENGTH ; i++) {
		strip_colors[i] = 0x0000FF;
	}

	// Do some random
	srand(time(NULL));

	loop();

	return 0;
}
