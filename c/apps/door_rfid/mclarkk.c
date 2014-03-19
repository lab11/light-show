#include "people.h"
#include "lights.h"
#include "effects.h"

void mclarkk_enter (int len) {
	effects_fade_fast(LIGHTS_BLUE, LIGHTS_TEAL, len);
	effects_fade_slow(LIGHTS_TEAL, LIGHTS_ORANGE, len);
}