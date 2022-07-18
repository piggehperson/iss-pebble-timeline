#include <pebble.h>
#include "detail_page.h"

static Window *s_detail_window;
static StatusBarLayer *s_status_bar;

static ScrollLayer *s_scroll_layer;
static ContentIndicator *s_indicator;
static Layer *s_indicator_down_layer;

static TextLayer *s_next_flyover_layer;
static TextLayer *s_fly_times_layer;
static GDrawCommandImage *s_satellite_image;
static Layer *s_satellite_layer;
static TextLayer *s_location_layer;
static TextLayer *s_duration_layer;
static TextLayer *s_data_disclaimer_layer;


static int nextRiseTime;
static int nextDuration;

static void image_update_proc(Layer *layer, GContext *ctx) {
  // Set the origin offset from the context for drawing the image
  GPoint origin = GPoint(0, 0);

  // Draw the GDrawCommandImage to the GContext
  gdraw_command_image_draw(ctx, s_satellite_image, origin);
}

static GRect measure_text(
  char *text,
  GRect bounds,
  GFont font,
  GTextAlignment textAlignment
  ) {
    // Find the bounds of the scrolling text
    GSize text_size = graphics_text_layout_get_content_size(text, font, 
                    bounds, GTextOverflowModeWordWrap, textAlignment);
    
    GRect text_bounds = bounds;
    text_bounds.size.h = text_size.h;
    return text_bounds;
}

static void setup_text(
  TextLayer *textLayer,
  ScrollLayer *scrollLayer,
  GRect bounds, 
  char *text,
  GFont font,
  GTextAlignment textAlignment
  ) {
    // Create the TextLayer
    if (!textLayer) {
      textLayer = text_layer_create(bounds);
    }
    text_layer_set_overflow_mode(textLayer, GTextOverflowModeWordWrap);
    text_layer_set_font(textLayer, font);
    text_layer_set_text(textLayer, text);
    text_layer_set_text_alignment(textLayer, textAlignment);

    scroll_layer_add_child(scrollLayer, text_layer_get_layer(textLayer));
    text_layer_enable_screen_text_flow_and_paging(textLayer, 6);
}

static void detail_window_load(Window *window) {
    window_set_background_color(window, DETAIL_BACKGROUND_COLOR);

    Layer *root_layer = window_get_root_layer(window);
    GRect rawBounds = layer_get_bounds(root_layer);
    GRect insetBounds = GRect(
      LAYOUT_INSET_W,
      0,
      rawBounds.size.w - (LAYOUT_INSET_W * 2),
      rawBounds.size.h
    );

    int expected_content_height = 0;

    // Create the StatusBarLayer
    s_status_bar = status_bar_layer_create();
    status_bar_layer_set_colors(s_status_bar, DETAIL_BACKGROUND_COLOR, DETAIL_ON_BACKGROUND_COLOR);
    status_bar_layer_set_separator_mode(s_status_bar, StatusBarLayerSeparatorModeNone);
    layer_add_child(root_layer, status_bar_layer_get_layer(s_status_bar));

    // Create the ScrollLayer
    s_scroll_layer = scroll_layer_create(GRect(rawBounds.origin.x, rawBounds.origin.y + STATUS_BAR_LAYER_HEIGHT, rawBounds.size.w, rawBounds.size.h - STATUS_BAR_LAYER_HEIGHT));
    scroll_layer_set_click_config_onto_window(s_scroll_layer, window);
    scroll_layer_set_paging(s_scroll_layer, PBL_IF_ROUND_ELSE(true, false));
    layer_add_child(root_layer, scroll_layer_get_layer(s_scroll_layer));
    
    // Get the ContentIndicator from the ScrollLayer
    s_indicator = scroll_layer_get_content_indicator(s_scroll_layer);

    // Create a Layers to draw the down arrow
    s_indicator_down_layer = layer_create(
                            GRect(0, rawBounds.size.h - STATUS_BAR_LAYER_HEIGHT,
                                  rawBounds.size.w, STATUS_BAR_LAYER_HEIGHT));

    // Add these Layers as children after all other components to appear below
    layer_add_child(root_layer, s_indicator_down_layer);

    // Configure the properties of each indicator
    

    const ContentIndicatorConfig down_config = (ContentIndicatorConfig) {
      .layer = s_indicator_down_layer,
      .times_out = false,
      .alignment = GAlignCenter,
      .colors = {
        .foreground = DETAIL_ON_BACKGROUND_COLOR,
        .background = DETAIL_BACKGROUND_COLOR
      }
    };
    content_indicator_configure_direction(s_indicator, ContentIndicatorDirectionDown,
                                          &down_config);

    // Init flyover text layer
    char* text = "Next flyover";
    GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    GRect textBounds = GRect(insetBounds.origin.x, expected_content_height, insetBounds.size.w, 200);
    GRect measuredBounds = measure_text(text, textBounds, font, GTextAlignmentCenter);
    setup_text(s_next_flyover_layer, s_scroll_layer, GRect(measuredBounds.origin.x, measuredBounds.origin.y, measuredBounds.size.w, measuredBounds.size.h+4), text, font, GTextAlignmentCenter);
    expected_content_height = expected_content_height + measuredBounds.size.h + 4;

    // Init time text layer
    text = "08:00-08:30";
    font = fonts_get_system_font(FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM);
    textBounds = GRect(insetBounds.origin.x, expected_content_height, insetBounds.size.w, 200);
    measuredBounds = measure_text(text, textBounds, font, GTextAlignmentCenter);
    setup_text(s_fly_times_layer, s_scroll_layer, measuredBounds, text, font, GTextAlignmentCenter);
    expected_content_height = expected_content_height + measuredBounds.size.h + 4;

    // init image layer
    s_satellite_layer = layer_create(GRect((rawBounds.size.w /2) -40, expected_content_height, 80, 80));
    layer_set_update_proc(s_satellite_layer, image_update_proc);
    scroll_layer_add_child(s_scroll_layer, s_satellite_layer);
    expected_content_height = expected_content_height + 80;

    // Init location text layer
    text = "Location";
    font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    textBounds = GRect(insetBounds.origin.x, expected_content_height, insetBounds.size.w, 200);
    measuredBounds = measure_text(text, textBounds, font, GTextAlignmentCenter);
    setup_text(s_location_layer, s_scroll_layer, measuredBounds, text, font, GTextAlignmentCenter);
    expected_content_height = expected_content_height + measuredBounds.size.h + 4;

    // Init more details layer
    text = "Station will be visible in the sky for 30 minutes.";
    font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    textBounds = GRect(insetBounds.origin.x, expected_content_height, insetBounds.size.w, 200);
    measuredBounds = measure_text(text, textBounds, font, GTextAlignmentLeft);
    setup_text(s_duration_layer, s_scroll_layer, measuredBounds, text, font, PBL_IF_RECT_ELSE(GTextAlignmentLeft, GTextAlignmentCenter));
    expected_content_height = expected_content_height + measuredBounds.size.h + 4;


    scroll_layer_set_content_size(s_scroll_layer, GSize(rawBounds.size.w, expected_content_height + 8));
}

static void detail_window_unload(Window *window) {
    status_bar_layer_destroy(s_status_bar);
    scroll_layer_destroy(s_scroll_layer);
    layer_destroy(s_indicator_down_layer);

    text_layer_destroy(s_next_flyover_layer);
    text_layer_destroy(s_fly_times_layer);
    layer_destroy(s_satellite_layer);
    text_layer_destroy(s_location_layer);
    text_layer_destroy(s_duration_layer);
    text_layer_destroy(s_data_disclaimer_layer);
  
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

    s_satellite_image = gdraw_command_image_create_with_resource(RESOURCE_ID_SATELLITE_LARGE);
}