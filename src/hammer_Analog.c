//IMPORTS
#include "hammer_Analog.h"
#include "pebble.h"
#include "string.h"
#include "stdlib.h"

//LAYERS 
Layer *simple_bg_layer;
Layer *date_layer;
Layer *hands_layer;
BitmapLayer *background;

//TEXT LAYERS
TextLayer *day_label;
TextLayer *num_label;
TextLayer *taps;

//HANDS
//static GBitmap *minute_arrow;
//static GBitmap *hour_arrow;
BitmapLayer *minuteLayer;
BitmapLayer *hourlayer;
//static RotBitmapLayer *minutehammer;
//static RotBitmapLayer *hourhammer;
static GPath *tick_paths[NUM_CLOCK_TICKS];

// buffers
char day_buffer[6];
char num_buffer[4];

//MISC
int tapcount = 0;
Window *window;
GBitmap *bacgroundimage;
GBitmap *hourhammerimage;
GBitmap *minutehammerimage;


/***************************************************************
*                       Time
***************************************************************/
static void bg_update_proc(Layer *layer, GContext *ctx) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
 
   graphics_context_set_fill_color(ctx, GColorBlack);
//  bacgroundimage = gbitmap_create_with_resource(CLOCKIMAGE);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  //graphics_draw_bitmap_in_rect(ctx, bacgroundimage, bounds);
  graphics_fill_rect(ctx, bounds, 11, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorWhite);
//  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
 //   gpath_draw_filled(ctx, tick_paths[i]);
//  }
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  const GPoint center = grect_center_point(&bounds);
  const int16_t secondHandLength = bounds.size.w / 2;

  GPoint secondHand;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  secondHand.y = (int16_t)(-cos_lookup(second_angle) * (int32_t)secondHandLength / TRIG_MAX_RATIO) + center.y;
  secondHand.x = (int16_t)(sin_lookup(second_angle) * (int32_t)secondHandLength / TRIG_MAX_RATIO) + center.x;

  // second hand
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, secondHand, center);
  
  minuteLayer = bitmap_layer_create(GRect(55, 85, 33, 66));
  hourlayer = bitmap_layer_create(GRect(55, 85, 33, 66));
  
  hourhammerimage = gbitmap_create_with_resource(RESOURCE_ID_HOURHAND_WHITE);
  minutehammerimage = gbitmap_create_with_resource(RESOURCE_ID_MINUTEHAND_WHITE);
  
  bitmap_layer_set_compositing_mode(minuteLayer, GCompOpAnd);
  bitmap_layer_set_compositing_mode(hourlayer, GCompOpAnd);
  
  bitmap_layer_set_bitmap(minuteLayer, minutehammerimage);
  bitmap_layer_set_bitmap(hourlayer, hourhammerimage);
  
  layer_add_child(layer, bitmap_layer_get_layer(minuteLayer));
  layer_add_child(layer, bitmap_layer_get_layer(hourlayer));
  
  
  

  // minute/hour hand
  //graphics_context_set_fill_color(ctx, GColorWhite);
  //graphics_context_set_stroke_color(ctx, GColorBlack);

  //gpath_rotate_to(minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  //rot_bitmap_layer_set_angle(minutehammer, TRIG_MAX_ANGLE * t->tm_min / 60);
  
  //gpath_draw_filled(ctx, minute_arrow);
  //gpath_draw_outline(ctx, minute_arrow);

  //path_rotate_to(hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  //rot_bitmap_layer_set_angle(hourhammer, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  //gpath_draw_filled(ctx, hour_arrow);
  //gpath_draw_outline(ctx, hour_arrow);

 /** // dot in the middle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornerNone);**/
}
/***************************************************************
*                       .js
***************************************************************/

static void date_update_proc(Layer *layer, GContext *ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  strftime(day_buffer, sizeof(day_buffer), "%a", t);
  text_layer_set_text(day_label, day_buffer);

  strftime(num_buffer, sizeof(num_buffer), "%d", t);
  text_layer_set_text(num_label, num_buffer);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}





static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
/***************************************************************
*                       Taps
***************************************************************/

void accel_tap_handler(AccelAxisType axis, int32_t direction) {

  //GUI confirmation of tap
  tapcount++;
  char* str;
  if(tapcount % 2 == 0) {
    str = "1";
  }
  else{
    str = "2";
  }
  text_layer_set_text(taps, str);
  
  
  //.js sending
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  // Add a key-value pair
  dict_write_uint8(iter, 0, 1);//1 to be replaced with something relating to the pebble ID

  // Send the message!
  app_message_outbox_send();
  
  
  //text_layer_set_text(day_label, tapcount)
}

/***************************************************************
*                       LOAD and UNLOAD
***************************************************************/
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  //init layers
  simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, simple_bg_layer);
  
  //background clock
  background = bitmap_layer_create(bounds);
  bacgroundimage = gbitmap_create_with_resource(RESOURCE_ID_CLOCKIMAGE);
  bitmap_layer_set_bitmap(background, bacgroundimage);
  bitmap_layer_set_alignment(background, GAlignBottom);
  bitmap_layer_set_compositing_mode(background, GCompOpAssign);
  layer_add_child(window_layer, bitmap_layer_get_layer(background));
  
  
  //init date layer -> a plain parent layer to create a date update proc
  date_layer = layer_create(bounds);
  layer_set_update_proc(date_layer, date_update_proc);
  layer_add_child(window_layer, date_layer);

  // init day
  day_label = text_layer_create(GRect(46, 114, 27, 20));
  text_layer_set_text(day_label, day_buffer);
  text_layer_set_background_color(day_label, GColorBlack);
  text_layer_set_text_color(day_label, GColorWhite);
  GFont norm18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  text_layer_set_font(day_label, norm18);
  
  //int tap label
  taps = text_layer_create(GRect(10, 10, 20, 20));
  text_layer_set_text(taps, "1");
  text_layer_set_background_color(taps, GColorBlack);
  text_layer_set_text_color(taps, GColorWhite);
  //GFont norm18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  text_layer_set_font(taps, norm18);
  layer_add_child(date_layer, text_layer_get_layer(taps));

  // init num
  num_label = text_layer_create(GRect(73, 114, 18, 20));

  text_layer_set_text(num_label, num_buffer);
 text_layer_set_background_color(num_label, GColorBlack);
 text_layer_set_text_color(num_label, GColorWhite);
  GFont bold18 = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(num_label, bold18);

  layer_add_child(date_layer, text_layer_get_layer(num_label));

  // init hands
  hands_layer = layer_create(bounds);
  layer_set_update_proc(hands_layer, hands_update_proc);
  layer_add_child(window_layer, hands_layer);
  
  
  
  
  //init .js communication
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void window_unload(Window *window) {
  layer_destroy(simple_bg_layer);
  layer_destroy(date_layer);
  text_layer_destroy(day_label);
  text_layer_destroy(num_label);
  text_layer_destroy(taps);
  layer_destroy(hands_layer);
  bitmap_layer_destroy(background);
}

/***************************************************************
*                       INT and DE INT
***************************************************************/
static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  day_buffer[0] = '\0';
  num_buffer[0] = '\0';

  // init hand paths
  //#define GRect(55.5, 85.5, 33, 66)((Grect){{(55.5),(85.5)},{(33),(66)}});
  GRect rect = GRect(55, 85, 33, 66);//off a little
  GPoint point = GPoint(72,95);
  //GRect rectMin = ((Grect){{(55.5),(85.5)},{(33),(66)}});
/**
    
  //minute_arrow = gbitmap
  minuteLayer = bitmap_layer_create(GRect(55, 85, 33, 66));
  hourlayer = bitmap_layer_create(GRect(55, 85, 33, 66)); //move out of there
  
  //bitmap_layer_set_bitmap(hourlayer, gbitmap_create_with_resource(RESOURCE_ID_HOURHAND_WHITE));
//  bitmap_layer_set_bitmap(minuteLayer, gbitmap_create_with_resource(RESOURCE_ID_MINUTEHAND_WHITE));
  minutehammer = rot_bitmap_layer_create(gbitmap_create_with_resource(RESOURCE_ID_HOURHAND_WHITE));
  hourhammer = rot_bitmap_layer_create(gbitmap_create_with_resource(RESOURCE_ID_MINUTEHAND_WHITE));
  //minutehammer = rot_bitmap_layer_create(minuteLayer);
  //hourhammer = rot_bitmap_layer_create(hourlayer);
  rot_bitmap_set_src_ic(minutehammer,point);
  rot_bitmap_set_src_ic(hourhammer,point);

 // Layer *window_layer = window_get_root_layer(window);
 // GRect bounds = layer_get_bounds(window_layer);
 //// const GPoint center = grect_center_point(&bounds);
   Layer *window_layer = window_get_root_layer(window);
   layer_add_child(window_layer, bitmap_layer_get_layer(minuteLayer));
    layer_add_child(window_layer, bitmap_layer_get_layer(hourlayer));
  
   
  **/
  //gpath_move_to(minute_arrow, center);
  //gpath_move_to(hour_arrow, center);

  // init clock face paths
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    tick_paths[i] = gpath_create(&ANALOG_BG_POINTS[i]);
  }

  // Push the window onto the stack
  const bool animated = true;
  window_stack_push(window, animated);

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  
  //taps
    accel_tap_service_subscribe(accel_tap_handler);
  
}

static void deinit(void) {
  //kill the hands
  //rot_bitmap_layer_destroy(minutehammer);
  ///rot_bitmap_layer_destroy(hourhammer);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "bit layer to be freed %p", minuteLayer);  //gbitmap_destroy(minute_arrow);
  //gbitmap_destroy(hour_arrow);
  bitmap_layer_destroy(minuteLayer);
  bitmap_layer_destroy(hourlayer);
  
  //kill the background
  //gbitmap_destroy(bitmap_layer_get_bitmap(background));
  bitmap_layer_destroy(background);
  
  //gpath_destroy(minute_arrow);
  //gpath_destroy(hour_arrow);

  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_destroy(tick_paths[i]);
  }

  tick_timer_service_unsubscribe();
   accel_tap_service_unsubscribe();
  window_destroy(window);
}

/***************************************************************
*                           Main
***************************************************************/
int main(void) {
  init();
  app_event_loop();
  deinit();
}