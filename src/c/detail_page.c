#include <pebble.h>
#include "detail_page.h"

static Window *s_detail_window;
static StatusBarLayer *s_status_bar;

static int nextRiseTime;
static int nextDuration;

static void detail_window_load(Window *window) {
    window_set_background_color(window, BACKGROUND_COLOR);

    Layer *root_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(root_layer);

    // Create the StatusBarLayer
    s_status_bar = status_bar_layer_create();

    // Set properties
    status_bar_layer_set_colors(s_status_bar, BACKGROUND_COLOR, ON_BACKGROUND_COLOR);
    status_bar_layer_set_separator_mode(s_status_bar, StatusBarLayerSeparatorModeNone);

    // Add to Window
    layer_add_child(root_layer, status_bar_layer_get_layer(s_status_bar));
}

static void detail_window_unload(Window *window) {

}



static void detail_window_push(int riseTime, int duration) {
    nextRiseTime = riseTime;
    nextDuration = duration;
    
    if(!s_detail_window) {
        s_detail_window = window_create();
        window_set_window_handlers(s_detail_window, (WindowHandlers) {
        .load = detail_window_load,
        .unload = detail_window_unload,
        });
        window_stack_push(s_detail_window, true);
    }
}