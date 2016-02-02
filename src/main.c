#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_activity_layer;
int mins = 0;
int secs = 0;

static GBitmap *action_icon_plus;
static GBitmap *action_icon_minus;
static ActionBarLayer *action_bar;

int activity_count = 0;
char *activity_buf[] = {"Meditation", "Coding", "Exercise"};

static void logicForVibration() {
  secs = secs + 1;
  if (secs == 60) 
  {
    mins = mins + 1;
    secs = 0;
    vibes_short_pulse();
  }
  if (mins %2 == 0 && secs == 1)
  {
    vibes_short_pulse();
  }
  
  if (mins %5 == 0 && ( secs ==  1 || secs == 2 || secs == 3 || secs == 4))
  {
    vibes_long_pulse();
    //vibes_short_pulse();
    //vibes_long_pulse();
  }
  
  if (mins == 9 && secs < 10)
  {
    vibes_short_pulse();
  }
  
  if (mins == 19 && secs < 20)
  {
    vibes_short_pulse();
  }
  
  if (mins == 29 && secs < 30)
  {
    vibes_short_pulse();
  }
  
  if (mins == 39 && secs < 40)
  {
    vibes_short_pulse();
  }
  
  if (mins == 49 && secs < 50)
  {
    vibes_short_pulse();
  }
  
  if (mins == 59 && secs < 60)
  {
    vibes_short_pulse();
  }
}

static void send_int(int key, int value) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_int(iter, key, &value, sizeof(int), true);
  app_message_outbox_send();
}

static void increment_click_handler(ClickRecognizerRef recognizer, void *context) {
  tick_timer_service_unsubscribe();
  // send the activity and time info
  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  
  // Add a key-value pair
  static char buffer[] = "00:00";
  char tmp[] = "00";
  snprintf(buffer, 8, "%d", mins);
  strcat(buffer, ":");
  snprintf(tmp, 3, "%d", secs);
  strcat(buffer, tmp);
  dict_write_cstring(iter, activity_count, buffer);
  dict_write_end (iter);
 
  // Send the message!
  app_message_outbox_send();
}

static void update_time() {
  
  // Create a long-lived buffer
  static char buffer[] = "00:00";
  char tmp[] = "00";
  
  logicForVibration();
  
  snprintf(buffer, 8, "%d", mins);
  strcat(buffer, ":");
  snprintf(tmp, 3, "%d", secs);
  strcat(buffer, tmp);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void decrement_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (activity_count >= 2) activity_count = -1;
  activity_count = activity_count + 1;
  text_layer_set_text(s_activity_layer, activity_buf[activity_count]);
  }

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  
   // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  }

static void click_config_provider(void *context) {
  const uint16_t repeat_interval_ms = 50;
  window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, (ClickHandler) increment_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, (ClickHandler) decrement_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_SELECT, repeat_interval_ms, (ClickHandler) select_click_handler);
}

void out_sent_handler(DictionaryIterator *sent, void *context)
  {
  
}

static void out_fail_handler(DictionaryIterator *failed, AppMessageResult reason, void* context)
  {}

static void in_received_handler(DictionaryIterator *iter, void* context)
  {}

void in_drop_handler(AppMessageResult reason, void *context)
  {}


static void main_window_load(Window *window) {
  action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar, window);
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider);
  
  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, action_icon_plus);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, action_icon_minus);
  
  // Create Activity TextLayer
  s_activity_layer = text_layer_create(GRect(0, 15, 144, 50));
  text_layer_set_background_color(s_activity_layer, GColorClear);
  text_layer_set_text_color(s_activity_layer, GColorBlack);
  text_layer_set_text(s_activity_layer, activity_buf[activity_count]);

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_activity_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_activity_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_activity_layer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00:00");

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  update_time();
  
  

}
static void main_window_unload(Window *window) {
 text_layer_destroy(s_time_layer);
}
  


static void init() {
  
   // Register message handlers
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_drop_handler);
  app_message_register_outbox_failed(out_fail_handler);
  
  // Init buffers
  app_message_open(64, 64);
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
 
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
}



static void deinit() {
  // Destroy Window
   window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

