#include <pebble.h>
#include "detail_page.c"
#include "loading_page.c"

static bool s_js_ready;
static bool s_launch_reason;
static AppTimer *s_timeout_timer;

int result_code;
int next_rise;
int next_duration;


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
  loading_window_set_text("Message received!");
  
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
    
  }
  Tuple *next_rise_tuple = dict_find(iter, MESSAGE_KEY_nextRiseTime);
  if(next_rise_tuple) {
    next_rise = next_rise_tuple->value->int32;
    // Update UI
    
  }
  Tuple *next_duration_tuple = dict_find(iter, MESSAGE_KEY_nextDuration);
  if(next_duration_tuple) {
    next_duration = next_duration_tuple->value->int32;
    // Update UI
    
  }

  // Launch details page
  if (next_rise_tuple && next_duration_tuple) {
      detail_window_push(next_rise, next_duration);
  }
}

static void prv_init(void) {
  // Largest expected inbox and outbox message sizes
  const uint32_t inbox_size = 128;
  const uint32_t outbox_size = 64;

  // Open AppMessage
  app_message_open(inbox_size, outbox_size);
  app_message_register_inbox_received(inbox_received_handler);

  loading_window_push();
}

static void prv_deinit(void) {
  //window_destroy(s_window);
}

int main(void) {
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window");

  app_event_loop();
  prv_deinit();
}
