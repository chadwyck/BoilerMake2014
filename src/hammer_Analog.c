//IMPORTS
#include "hammer_Analog.h"
#include "pebble.h"
#include "string.h"
#include "stdlib.h"

//LAYERS 
Layer *simple_bg_layer;
Layer *hands_layer;
BitmapLayer *background;

//TEXT LAYERS
TextLayer *num_label;

//HANDS
RotBitmapLayer *minutehammer;
RotBitmapLayer *hourhammer;
static GPath *tick_paths[NUM_CLOCK_TICKS];

// buffers
char num_buffer[4];

//MISC
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
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_fill_rect(ctx, bounds, 11, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorWhite);

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

          /***************************************************************
        *                       rot bitmap layer for the hammers
        ***************************************************************/
  rot_bitmap_set_compositing_mode(hourhammer, GCompOpOr);
  rot_bitmap_set_compositing_mode(minutehammer, GCompOpOr);
  
  rot_bitmap_set_src_ic(hourhammer, GPoint(14, 12));
  rot_bitmap_set_src_ic(minutehammer, GPoint(14, 12));
  
  layer_add_child(layer, (Layer*) hourhammer);
  layer_add_child(layer, (Layer*) minutehammer);
  
  rot_bitmap_layer_set_angle(minutehammer, (TRIG_MAX_ANGLE * t->tm_min / 60+TRIG_MAX_ANGLE/2) % TRIG_MAX_ANGLE);
  rot_bitmap_layer_set_angle(hourhammer, ((TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6)+TRIG_MAX_ANGLE/2)%TRIG_MAX_ANGLE);
  
   // dot in the middle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornerNone);
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
  bitmap_layer_set_alignment(background, GAlignCenter);
  bitmap_layer_set_compositing_mode(background, GCompOpAssign);
  layer_add_child(window_layer, bitmap_layer_get_layer(background));
  
  // init num
  num_label = text_layer_create(GRect(73, 114, 18, 20));

  text_layer_set_text(num_label, num_buffer);
  text_layer_set_background_color(num_label, GColorBlack);
  text_layer_set_text_color(num_label, GColorWhite);
  GFont bold18 = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(num_label, bold18);

  // init hands
  hands_layer = layer_create(bounds);
  layer_set_update_proc(hands_layer, hands_update_proc);
  layer_add_child(window_layer, hands_layer);
}

static void window_unload(Window *window) {
  layer_destroy(simple_bg_layer);
  text_layer_destroy(num_label);
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

  num_buffer[0] = '\0';

  hourhammerimage = gbitmap_create_with_resource(RESOURCE_ID_HOUR_HAND);
  minutehammerimage = gbitmap_create_with_resource(RESOURCE_ID_MINUTE_HAND);

  hourhammer = rot_bitmap_layer_create(hourhammerimage);
  minutehammer = rot_bitmap_layer_create(minutehammerimage);

  // init clock face paths
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    tick_paths[i] = gpath_create(&ANALOG_BG_POINTS[i]);
  }

  // Push the window onto the stack
  const bool animated = true;
  window_stack_push(window, animated);

// tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void deinit(void) {
  
  rot_bitmap_layer_destroy(minutehammer);
  rot_bitmap_layer_destroy(hourhammer);

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
