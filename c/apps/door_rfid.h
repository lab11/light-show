#ifndef __DOOR_RFID_H__
#define __DOOR_RFID_H__

typedef void uniqname_enter (int len);

typedef struct {
	char* uniqname;
	uniqname_enter* enter_fn;
} rfid_action_t;

#endif
