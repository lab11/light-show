#include "people.h"
#include "lights.h"
#include "effects.h"

void wwhuang_enter (int len) {
	effects_fade(LIGHTS_TEAL, LIGHTS_RED, len);
}
