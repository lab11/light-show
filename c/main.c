#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>

#include "main.h"
#include "lights.h"
#include "effects.h"
#include "app.h"

#define MAX_APPS 100
#define APP_DURATION 60  // time in seconds
#define NUMBER_INITS (sizeof(init_fns) / sizeof(init_fn*))

init_fn*    init_fns[]   = {tracer_init,
                            door_rfid_init,
                            random_init,
                            cube_init,
                            coilcube_init};
const char* init_names[] = {"tracer", "door rfid", "random", "cube",
                            "coilcube"};
app_info    info[MAX_APPS] = {{CONTINUOUS_APP,
                              -1,
                              -1,
                              NULL,
                              {0, 0},
                              0}};

int number_of_apps = 0;

#define STRIP_LENGTH 350
uint32_t lights[STRIP_LENGTH];

// Id of the app that is being inited so we know who to set up the config
// info for.
int current_initer = -1;

// Call this from app_init() to add a socket to the select call.
// Whenever there is data on s, the update function u will be called.
int register_socket (int s, update_fn* u) {

	if (current_initer < 0) {
		// Error, no longer init time.
		return -1;
	}

	// Since the app called this function, we know some of the values for the
	// app info right off the bat.
	info[number_of_apps].type = ASYNCHRONOUS_APP;
	info[number_of_apps].init_id = current_initer;
	info[number_of_apps].socket = s;
	info[number_of_apps].updater = u;
	info[number_of_apps].period.tv_sec = 0;
	info[number_of_apps].period.tv_usec = 0;
	info[number_of_apps].app_periods = 0;

	number_of_apps++;

	return 0;
}

// Call this to register an app that wants to set the lights at a regular
// interval.
int register_continuous (struct timeval tv, update_fn* u) {

	if (current_initer < 0) {
		// Error, no longer init time.
		return -1;
	}

	// Clear any async settings and setup the continuous app
	info[number_of_apps].type = CONTINUOUS_APP;
	info[number_of_apps].init_id = current_initer;
	info[number_of_apps].socket = -1;
	info[number_of_apps].updater = u;
	info[number_of_apps].period = tv;

	// Determine the number of times we should call this app's update function
	// before moving to the next app.
	info[number_of_apps].app_periods = (APP_DURATION*1000000) /
	                                   (tv.tv_sec*1000000 + tv.tv_usec);

	number_of_apps++;

	return 0;
}

// Get the time now rounded down the nearest second
time_t seconds_now () {
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec;
}

int main () {
	int result;
	int i;
	time_t start=0, now=0;

	// Id of the continuous app that currently has access to the strand
	int current_app = -1;

	// Number of select() timeouts a particular application gets.
	// This is dependent on the period it requested when it registered.
	int app_periods = 0;

	struct timeval* select_timeout_ptr;
	struct timeval select_timeout = {0, 0};
	int max_socket;
	fd_set rfds;

	lights_init();
	effects_init();

	// Set all lights off
	for (i=0; i<STRIP_LENGTH; i++) {
		lights[i] = LIGHTS_BLACK;
	}
	lights_set(lights, STRIP_LENGTH);

	// Run all of the init functions
	for (i=0; i<NUMBER_INITS; i++) {
		current_initer = i;

		result = init_fns[i]();
		if (result != 0) {
			printf("Initialization step failed.\n");
			printf("Could not initialize: %s\n", init_names[i]);
			return 1;
		}
	}
	current_initer = -1;

	// Find a valid starting app.
	// If one is not found then current_app will be -1 and that's ok.
	for (i=0; i<number_of_apps; i++) {
		if (info[i].type == CONTINUOUS_APP) {
			current_app = i;
			break;
		}
	}

	start = seconds_now();

	while (1) {
		int ret;

		now = seconds_now();

		if ((app_periods == 0 || (now - start >= APP_DURATION)) &&
			current_app != -1) {
			// Need to move to the next application
			for (i=1; i<number_of_apps; i++) {
				if (info[(current_app+i) % number_of_apps].type == CONTINUOUS_APP) {
					current_app = (current_app+i) % number_of_apps;
					break;
				}
			}
			printf("[%i] Starting application: %s\n", (int) now,
				init_names[info[current_app].init_id]);
			app_periods = info[current_app].app_periods;
			info[current_app].updater(lights, STRIP_LENGTH);
			lights_set(lights, STRIP_LENGTH);
			start = now;
		}

		if (current_app == -1) {
			// no timeout
			select_timeout_ptr = NULL;
		} else {
			// If we successfully timed out last time then reset the time.
			// Otherwise, use the existing select_timeout
			if (select_timeout.tv_sec == 0 && select_timeout.tv_usec == 0) {
				select_timeout = info[current_app].period;
				select_timeout_ptr = &select_timeout;
			}
		}

		// Setup the select call
		FD_ZERO(&rfds);
		max_socket = 0;
		for (i=0; i<number_of_apps; i++) {
			if (info[i].type == ASYNCHRONOUS_APP) {
				FD_SET(info[i].socket, &rfds);
				if (info[i].socket > max_socket) {
					max_socket = info[i].socket;
				}
			}
		}
		ret = select(max_socket+1, &rfds, NULL, NULL, select_timeout_ptr);

		if (ret == -1) {
			// An error occurred with select
			fprintf(stderr, "Select error.\n");
			fprintf(stderr, "%s\n", strerror(errno));

		} else if (ret == 0) {
			// select timeout
			info[current_app].updater(lights, STRIP_LENGTH);
			lights_set(lights, STRIP_LENGTH);
			app_periods--;

		} else {
			// File drescriptor ready
			for (i=0; i<number_of_apps; i++) {
				if (info[i].type == ASYNCHRONOUS_APP) {
					if (FD_ISSET(info[i].socket, &rfds)) {
						info[i].updater(lights, STRIP_LENGTH);
					}
				}
			}
		}

	}

	return 0;
}
