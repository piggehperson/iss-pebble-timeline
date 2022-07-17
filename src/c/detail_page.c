#include <pebble.h>
#include "detail_page.h"

static Window *s_detail_window;
static StatusBarLayer *s_status_bar;
static ScrollLayer *s_scroll_layer;

static GDrawCommandImage *s_satellite_image;
static Layer *s_satellite_layer;


static int nextRiseTime;
static int nextDuration;

static void image_update_proc(Layer *layer, GContext *ctx) {
  // Set the origin offset from the context for drawing the image
  GPoint origin = GPoint(0, 0);

  // Draw the GDrawCommandImage to the GContext
  gdraw_command_image_draw(ctx, s_satellite_image, origin);
}

static void detail_window_load(Window *window) {
    window_set_background_color(window, DETAIL_BACKGROUND_COLOR);

    Layer *root_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(root_layer);

    // Create the StatusBarLayer
    s_status_bar = status_bar_layer_create();
    status_bar_layer_set_colors(s_status_bar, DETAIL_BACKGROUND_COLOR, DETAIL_ON_BACKGROUND_COLOR);
    status_bar_layer_set_separator_mode(s_status_bar, StatusBarLayerSeparatorModeNone);
    layer_add_child(root_layer, status_bar_layer_get_layer(s_status_bar));

    s_scroll_layer = scroll_layer_create(bounds);
    scroll_layer_set_content_size(s_scroll_layer, GSize(80, 25+50+80));
    scroll_layer_set_click_config_onto_window(s_scroll_layer, window);
    layer_add_child(root_layer, scroll_layer_get_layer(s_scroll_layer));

    s_satellite_layer = layer_create(GRect(0, 0, 25, 25));
    layer_set_update_proc(s_satellite_layer, image_update_proc);
    scroll_layer_add_child(s_scroll_layer, s_satellite_layer);
}

static void detail_window_unload(Window *window) {
    scroll_layer_destroy(s_scroll_layer);
    status_bar_layer_destroy(s_status_bar);
    layer_destroy(s_satellite_layer);
    gdraw_command_image_destroy(s_satellite_image);
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