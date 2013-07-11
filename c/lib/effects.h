#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include <stdint.h>

void effects_fade (uint32_t start_color, uint32_t end_color, uint16_t num_lights);

// Internal helper functions

uint8_t interpolate (uint8_t hue_start,
                     uint8_t hue_end,
                     uint8_t step_idx,
                     uint8_t num_steps);

#endif
