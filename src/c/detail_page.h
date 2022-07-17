#include <pebble.h>
#define DETAIL_BACKGROUND_COLOR PBL_IF_COLOR_ELSE(GColorGreen, GColorWhite)
#define DETAIL_ON_BACKGROUND_COLOR GColorBlack

static void detail_window_push(int riseTime, int duration);