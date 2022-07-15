#include <pebble.h>
#define BACKGROUND_COLOR PBL_IF_COLOR_ELSE(GColorGreen, GColorWhite)
#define ON_BACKGROUND_COLOR GColorBlack

static void detail_window_push(int riseTime, int duration);