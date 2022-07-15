#include <pebble.h>

static Window *s_window;
static TextLayer *s_text_layer;
static bool s_js_ready;
static bool s_launch_reason;
static AppTimer *s_timeout_timer;

int result_code;
int next_rise;
int next_duration;

static void timout_timer_handler(void *context) {
  // The timer elapsed because no success was reported
  text_layer_set_text(s_text_layer, "Failed.");
}


static bool request_search() {
  // Declare the dictionary's iterator
  DictionaryIterator *out_iter;

  // Prepare the outbox buffer for this message
  AppMessageResult result = app_message_outbox_begin(&out_iter);

  if(result == APP_MSG_OK) {
    // Construct the message
    // A dummy value
    int value = 1;

    // Add an item to ask for ISS data
    dict_write_int(out_iter, MESSAGE_KEY_runSearch, &value, sizeof(int), true);

    // Send this message
    result = app_message_outbox_send();

    // Schedule the timeout timer
    // const int interval_ms = 5000;
    // s_timout_timer = app_timer_register(interval_ms, timout_timer_handler, NULL);

    // Check the result
    if(result != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
      return false;
    }
    return true;
  } else {
    // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
    return false;
  }
}

static void outbox_sent_handler(DictionaryIterator *iter, void *context) {
  // Successful message, the timeout is not needed anymore for this message
  //app_timer_cancel(*s_timout_timer);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  text_layer_set_text(s_text_layer, "Message received!");
  
  Tuple *ready_tuple = dict_find(iter, MESSAGE_KEY_JSReady);
  if(ready_tuple) {
    // PebbleKit JS is ready! Safe to send messages
    s_js_ready = true;

    request_search();
  }
  Tuple *result_code_tuple = dict_find(iter, MESSAGE_KEY_resultCode);
  if(result_code_tuple) {
    result_code = result_code_tuple->value->int32;
    // Update UI
    text_layer_set_text(s_text_layer, "Result code");
  }
  Tuple *next_rise_tuple = dict_find(iter, MESSAGE_KEY_nextRiseTime);
  if(next_rise_tuple) {
    next_rise = next_rise_tuple->value->int32;
    // Update UI
    text_layer_set_text(s_text_layer, "Next rise time");
  }
  Tuple *next_duration_tuple = dict_find(iter, MESSAGE_KEY_nextDuration);
  if(next_duration_tuple) {
    next_duration = next_duration_tuple->value->int32;
    // Update UI
    text_layer_set_text(s_text_layer, "Next rise duration");
  }
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  text_layer_set_text(s_text_layer, "initializing JS...");
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}

static void prv_init(void) {
  // Largest expected inbox and outbox message sizes
  const uint32_t inbox_size = 128;
  const uint32_t outbox_size = 64;

  // Open AppMessage
  app_message_open(inbox_size, outbox_size);
  app_message_register_inbox_received(inbox_received_handler);

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void prv_deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}
