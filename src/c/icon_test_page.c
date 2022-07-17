#include <pebble.h>
#include "icon_test_page.h"

static Window *s_icon_test_window;
static StatusBarLayer *s_status_bar;
static ScrollLayer *s_scroll_layer;

static GDrawCommandImage *s_image_tiny;
static GDrawCommandImage *s_image_small;
static GDrawCommandImage *s_image_large;
static Layer *s_canvas_tiny;
static Layer *s_canvas_small;
static Layer *s_canvas_large;

static void update_tiny(Layer *layer, GContext *ctx) {
  // Set the origin offset from the context for drawing the image
  GPoint origin = GPoint(0, 0);

  // Draw the GDrawCommandImage to the GContext
  gdraw_command_image_draw(ctx, s_image_tiny, origin);
}
static void update_small(Layer *layer, GContext *ctx) {
  // Set the origin offset from the context for drawing the image
  GPoint origin = GPoint(0, 0);

  // Draw the GDrawCommandImage to the GContext
  gdraw_command_image_draw(ctx, s_image_small, origin);
}
static void update_large(Layer *layer, GContext *ctx) {
  // Set the origin offset from the context for drawing the image
  GPoint origin = GPoint(0, 0);

  // Draw the GDrawCommandImage to the GContext
  gdraw_command_image_draw(ctx, s_image_large, origin);
}

static void icon_test_window_load(Window *window) {
    window_set_background_color(window, ICON_TEST_BACKGROUND_COLOR);

    Layer *root_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(root_layer);

    // Create the StatusBarLayer
    s_status_bar = status_bar_layer_create();
    status_bar_layer_set_colors(s_status_bar, ICON_TEST_BACKGROUND_COLOR, ICON_TEST_ON_BACKGROUND_COLOR);
    status_bar_layer_set_separator_mode(s_status_bar, StatusBarLayerSeparatorModeNone);
    layer_add_child(root_layer, status_bar_layer_get_layer(s_status_bar));

    s_scroll_layer = scroll_layer_create(bounds);
    scroll_layer_set_content_size(s_scroll_layer, GSize(80, 25+50+80));
    scroll_layer_set_click_config_onto_window(s_scroll_layer, window);
    layer_add_child(root_layer, scroll_layer_get_layer(s_scroll_layer));

    s_canvas_tiny = layer_create(GRect(0, 0, 25, 25));
    s_canvas_small = layer_create(GRect(0, 25, 50, 50));
    s_canvas_large = layer_create(GRect(0, 75, 80, 80));
    layer_set_update_proc(s_canvas_tiny, update_tiny);
    layer_set_update_proc(s_canvas_small, update_small);
    layer_set_update_proc(s_canvas_large, update_large);
    scroll_layer_add_child(s_scroll_layer, s_canvas_tiny);
    scroll_layer_add_child(s_scroll_layer, s_canvas_small);
    scroll_layer_add_child(s_scroll_layer, s_canvas_large);
}

static void icon_test_window_unload(Window *window) {
    layer_destroy(s_canvas_tiny);
    layer_destroy(s_canvas_small);
    layer_destroy(s_canvas_large);
    gdraw_command_image_destroy(s_image_tiny);
    gdraw_command_image_destroy(s_image_small);
    gdraw_command_image_destroy(s_image_large);
    scroll_layer_destroy(s_scroll_layer);
    status_bar_layer_destroy(s_status_bar);
}

static void icon_test_window_push() {
    if(!s_icon_test_window) {
        s_icon_test_window = window_create();
        window_set_window_handlers(s_icon_test_window, (WindowHandlers) {
        .load = icon_test_window_load,
        .unload = icon_test_window_unload,
        });
        window_stack_push(s_icon_test_window, true);
    }

    s_image_tiny = gdraw_command_image_create_with_resource(RESOURCE_ID_SATELLITE_TINY);
    s_image_small = gdraw_command_image_create_with_resource(RESOURCE_ID_SATELLITE_SMALL);
    s_image_large = gdraw_command_image_create_with_resource(RESOURCE_ID_SATELLITE_LARGE);
}