#include "people.h"
#include "lights.h"
#include "effects.h"

void nklugman_enter (int len) {
	effects_blink(LIGHTS_WHITE, LIGHTS_PURPLE, len);
}
