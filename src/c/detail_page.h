#include <pebble.h>
#define DETAIL_BACKGROUND_COLOR COLOR_FALLBACK(GColorGreen, GColorWhite)
#define DETAIL_ON_BACKGROUND_COLOR GColorBlack

#define LAYOUT_INSET_W PBL_IF_ROUND_ELSE(20, 6)

static void detail_window_push(int riseTime, int duration);