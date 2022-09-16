#include <pebble.h>
#define LOADING_BACKGROUND_COLOR PBL_IF_COLOR_ELSE(GColorPictonBlue, GColorWhite)
#define LOADING_ON_BACKGROUND_COLOR GColorBlack

static Window *loading_window_push();