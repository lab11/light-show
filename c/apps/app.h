#ifndef __APP_H__
#define __APP_H__

void tracer_update (uint32_t* lights, int len);

void door_rfid_update (uint32_t* lights, int len);

void random_update (uint32_t* lights, int len);

void cube_update (uint32_t* lights, int len);

void coilcube_timeslot (uint32_t* lights, int len);
void coilcube_pkt (uint32_t* lights, int len);

#endif
