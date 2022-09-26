#include <pebble.h>
#define LOADING_BACKGROUND_COLOR COLOR_FALLBACK(GColorPictonBlue, GColorWhite)
#define LOADING_ON_BACKGROUND_COLOR GColorBlack

static Window *loading_window_push();