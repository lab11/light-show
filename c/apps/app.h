#ifndef __APP_H__
#define __APP_H__

int tracer_init ();
void tracer_update (uint32_t* lights, int len);

int door_rfid_init ();
void door_rfid_update (uint32_t* lights, int len);

int random_init ();
void random_update (uint32_t* lights, int len);

int cube_init ();
void cube_update (uint32_t* lights, int len);

int coilcube_init ();
void coilcube_timeslot (uint32_t* lights, int len);
void coilcube_pkt (uint32_t* lights, int len);

#endif
