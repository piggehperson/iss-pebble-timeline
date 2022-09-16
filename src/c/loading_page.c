#include <pebble.h>
#include "loading_page.h"

static Window *s_loading_window;
static StatusBarLayer *s_loading_status_bar;
static TextLayer *s_loading_text_layer;

static GDrawCommandImage *s_constellation_image;

static void loading_window_set_text(char *text) {
    //TODO
    char *dest = "";
    strncpy(dest, text, strlen(text));
    text_layer_set_text(s_loading_text_layer, dest);
}

static void loading_window_load(Window *window) {
    window_set_background_color(window, LOADING_BACKGROUND_COLOR);

    Layer *root_layer = window_get_root_layer(window);
    // Create the StatusBarLayer
    s_loading_status_bar = status_bar_layer_create();
    status_bar_layer_set_colors(s_loading_status_bar, LOADING_BACKGROUND_COLOR, LOADING_ON_BACKGROUND_COLOR);
    status_bar_layer_set_separator_mode(s_loading_status_bar, StatusBarLayerSeparatorModeNone);
    layer_add_child(root_layer, status_bar_layer_get_layer(s_loading_status_bar));

    s_loading_text_layer = text_layer_create(GRect(0, 0, layer_get_bounds(root_layer).size.w, 40));
}

static void loading_window_unload(Window *window) {
    gdraw_command_image_destroy(s_constellation_image);
    status_bar_layer_destroy(s_loading_status_bar);
    window_stack_remove(s_loading_window, false);
    text_layer_destroy(s_loading_text_layer);
}



static Window *loading_window_push() {
    if(!s_loading_window) {
        s_loading_window = window_create();
        window_set_window_handlers(s_loading_window, (WindowHandlers) {
        .load = loading_window_load,
        .unload = loading_window_unload,
        });
        window_stack_push(s_loading_window, true);
    }

    s_constellation_image = gdraw_command_image_create_with_resource(RESOURCE_ID_SATELLITE_LARGE);

    return s_loading_window;
}