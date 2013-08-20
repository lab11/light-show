#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

typedef int init_fn (void);
typedef void update_fn (uint32_t* lights, int len);

typedef enum {
	ASYNCHRONOUS_APP,
	CONTINUOUS_APP
} app_type_e;

typedef struct {
	app_type_e type;
	int init_id;
	int socket;
	update_fn* updater;
	struct timeval period;
	int app_periods;
} app_info;


int register_socket (int s, update_fn* u);
int register_continuous (struct timeval tv, update_fn* u);

#endif
