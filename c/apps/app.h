#ifndef __APP_H__
#define __APP_H__

int tracer_init ();
void tracer_update (uint32_t* lights, int len);

int door_rfid_init ();
void door_rfid_update (uint32_t* lights, int len);

#endif
