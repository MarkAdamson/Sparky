#include <pebble.h>

#define KEY_POWER 0

static Window *s_main_window;

static ActionBarLayer *action_bar;

static TextLayer *s_power_layer;

static GFont s_power_font;

static GBitmap *s_icon_lamp;

static GBitmap *s_icon_power_off;
static GBitmap *s_icon_power_on;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(s_output_layer, "Select pressed!");
  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);

  // Send the message!
  app_message_outbox_send();
}
static void click_config_provider(void *context) {
  // Register the ClickHandlers
  //window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  //window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_icon_lamp = gbitmap_create_with_resource(RESOURCE_ID_IC_LAMP);
  
  s_icon_power_off = gbitmap_create_with_resource(RESOURCE_ID_IC_POWER_OFF);
  s_icon_power_on  = gbitmap_create_with_resource(RESOURCE_ID_IC_POWER_ON);
  
  // Initialize the action bar:
  action_bar = action_bar_layer_create();
  // Associate the action bar with the window:
  action_bar_layer_add_to_window(action_bar, window);
  // Set the click config provider:
  action_bar_layer_set_click_config_provider(action_bar,
                                             click_config_provider);
  
  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, s_icon_lamp);
  
  // Create power Layer
  s_power_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

  // Style the text
  text_layer_set_background_color(s_power_layer, GColorClear);
  text_layer_set_text_color(s_power_layer, GColorBlack);
  text_layer_set_text_alignment(s_power_layer, GTextAlignmentCenter);
  text_layer_set_text(s_power_layer, "...");

  // Create second custom font, apply it and add to Window
  s_power_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));
  text_layer_set_font(s_power_layer, s_power_font);
//   layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_power_layer));
}

static void main_window_unload(Window *window) {
  
  // Destroy weather elements
  text_layer_destroy(s_power_layer);
  fonts_unload_custom_font(s_power_font);
  
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {// Read tuples for data
  Tuple *power_tuple = dict_find(iterator, KEY_POWER);
  if (power_tuple && power_tuple->value->uint8) {
    action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, s_icon_power_on);
    window_set_background_color(s_main_window, GColorYellow);
    text_layer_set_text(s_power_layer, "On");
  } else {
    action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, s_icon_power_off);
    window_set_background_color(s_main_window, GColorLightGray);
    text_layer_set_text(s_power_layer, "Off");
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set the background color
  window_set_background_color(s_main_window, GColorLightGray);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
//   window_set_fullscreen(s_main_window, false);

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
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