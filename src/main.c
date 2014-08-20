#include <pebble.h>
#include <pebble_app_info.h>
#include <pebble_fonts.h>
#include <stdio.h>
#include "turkish_time.h"
  
Window *window;
TextLayer *text_layer,*batterytext_layer;
TextLayer *fuzzy_text_layer_1,*fuzzy_text_layer_2,*fuzzy_text_layer_3;
static TextLayer *calendar_day_text_layer,*calendar_day_no_text_layer;
static TextLayer *calendar_month_text_layer;
char buffer[] = "00:00";

#define LINE_BUFFER_SIZE 50
static char daybuffer[LINE_BUFFER_SIZE];
static char daynobuffer[LINE_BUFFER_SIZE];
static char monthbuffer[LINE_BUFFER_SIZE];

typedef struct {
  TextLayer *layer[2];
  } TextLine;

typedef struct {
  char line1[LINE_BUFFER_SIZE];
  char line2[LINE_BUFFER_SIZE];
  char line3[LINE_BUFFER_SIZE];
} TheTime;

TextLine line1;
TextLine line2;
TextLine line3;

static TheTime cur_time;
static TheTime new_time;

GFont text_font;
GFont text_font_light;
GFont bar_font;
GFont fuzzy_text_font;

struct tm *t;
time_t temp;
//WatchItSelf-Here
void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  int dakika = tick_time->tm_min;
  int saat = tick_time->tm_hour;
  if (!(dakika %5) || (dakika == 58) || (dakika == 2)) {
    fuzzy_time(saat, dakika, new_time.line1, new_time.line2, new_time.line3);
    text_layer_set_text(fuzzy_text_layer_1, new_time.line1);
    text_layer_set_text(fuzzy_text_layer_2, new_time.line2);
    text_layer_set_text(fuzzy_text_layer_3, new_time.line3);
    }
  text_layer_set_text(text_layer, buffer);
}
void update_fuzzy_watch() {
  temp = time(NULL);
  t = localtime(&temp);
  tick_handler(t, MINUTE_UNIT);

  fuzzy_time(t->tm_hour, t->tm_min, cur_time.line1, cur_time.line2, cur_time.line3);
  text_layer_set_text(fuzzy_text_layer_1, cur_time.line1);
  text_layer_set_text(fuzzy_text_layer_2, cur_time.line2);
  text_layer_set_text(fuzzy_text_layer_3, cur_time.line3);
  if((!(t->tm_min %5) || t->tm_min == 58 || t->tm_min == 2)) {
    fuzzy_time(t->tm_hour, t->tm_min, new_time.line1, new_time.line2, new_time.line3);
	   //update hour only if changed
  	if(strcmp(new_time.line1,cur_time.line1) != 0){
  		text_layer_set_text(fuzzy_text_layer_1, new_time.line1);
  	}
  	  //update min1 only if changed
  	if(strcmp(new_time.line2,cur_time.line2) != 0){
  		text_layer_set_text(fuzzy_text_layer_2, new_time.line2);
  	}
  	  //update min2 only if changed happens on
  	if(strcmp(new_time.line3,cur_time.line3) != 0){
  		text_layer_set_text(fuzzy_text_layer_3, new_time.line3);
    }
}
  strftime(daybuffer, sizeof(daybuffer), "%e.%B %A", t);
  text_layer_set_text(calendar_day_text_layer, daybuffer);
  /*
  strftime(daynobuffer, sizeof(daynobuffer), "%e", t);
  text_layer_set_text(calendar_day_no_text_layer, daynobuffer);
  
  strftime(monthbuffer, sizeof(monthbuffer), "%B", t);
  text_layer_set_text(calendar_month_text_layer, monthbuffer);*/
}
//Battery
//Taken from Maneki Neko Watch - thanks to pi-king
static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100 ";
  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "+%d", charge_state.charge_percent);
  } else {
	  snprintf(battery_text, sizeof(battery_text), "%% %d", charge_state.charge_percent);
	  if (charge_state.charge_percent==20){
	  	vibes_double_pulse();
	  }else if(charge_state.charge_percent==10){
	  	vibes_long_pulse();
	  }
  }
  text_layer_set_text(batterytext_layer, (battery_text));
}
void window_load(Window *window)
{
  Layer *rootLayer = window_get_root_layer(window);
    
  fuzzy_text_layer_1 = text_layer_create(GRect(0, 24, 144, 60));
  text_layer_set_text_color(fuzzy_text_layer_1, GColorClear);
  text_layer_set_background_color(fuzzy_text_layer_1, GColorBlack);
  text_layer_set_font(fuzzy_text_layer_1, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(fuzzy_text_layer_1, GTextAlignmentCenter);
  layer_add_child(rootLayer,(Layer*) fuzzy_text_layer_1);
  
  
  fuzzy_text_layer_2 = text_layer_create(GRect(0, 60, 144, 90));
  text_layer_set_text_color(fuzzy_text_layer_2, GColorClear);
  text_layer_set_background_color(fuzzy_text_layer_2, GColorBlack);
  text_layer_set_font(fuzzy_text_layer_2, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(fuzzy_text_layer_2, GTextAlignmentCenter);
  layer_add_child(rootLayer,(Layer*) fuzzy_text_layer_2);
  
  fuzzy_text_layer_3 = text_layer_create(GRect(0, 100, 144, 90));
  text_layer_set_text_color(fuzzy_text_layer_3, GColorClear);
  text_layer_set_background_color(fuzzy_text_layer_3, GColorBlack);
  text_layer_set_font(fuzzy_text_layer_3, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(fuzzy_text_layer_3, GTextAlignmentCenter);
  layer_add_child(rootLayer,(Layer*) fuzzy_text_layer_3);
  
  text_layer = text_layer_create(GRect(0, 150, 144, 168));
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorClear);
  text_layer_set_font(text_layer,fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(text_layer, GTextAlignmentLeft);
  layer_add_child(rootLayer, (Layer*) text_layer);
  
  //Battery Layer - thanks to pi-king
  //Text
  batterytext_layer = text_layer_create(GRect(90,150,54,18));
  text_layer_set_text_color(batterytext_layer, GColorClear);
  text_layer_set_background_color(batterytext_layer, GColorBlack);
  text_layer_set_font(batterytext_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(batterytext_layer, GTextAlignmentRight);
  layer_add_child(rootLayer,(Layer*) text_layer_get_layer(batterytext_layer));
  
  handle_battery(battery_state_service_peek());
  //Calendar
  calendar_day_text_layer = text_layer_create(GRect(0,0,144,24));
  text_layer_set_text_color(calendar_day_text_layer, GColorWhite);
  text_layer_set_background_color(calendar_day_text_layer, GColorBlack);
  text_layer_set_font(calendar_day_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(calendar_day_text_layer, GTextAlignmentCenter);
  layer_add_child(rootLayer,(Layer*) text_layer_get_layer(calendar_day_text_layer));
  //Initial Watch
  temp = time(NULL);
  t = localtime(&temp);
  tick_handler(t, MINUTE_UNIT);
}
void window_unload(Window *window)
{
  //We will safely destroy window's elements here!
  text_layer_destroy(text_layer);
  text_layer_destroy(fuzzy_text_layer_1);
  text_layer_destroy(fuzzy_text_layer_2);
  text_layer_destroy(fuzzy_text_layer_3);
  text_layer_destroy(batterytext_layer);
  text_layer_destroy(calendar_day_text_layer);
}
void init()
{
  //Initialize the app event here!
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
  update_fuzzy_watch();
  battery_state_service_subscribe(&handle_battery);
}
void deinit()
{
  //De-initiallize elements here to save memory
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  window_stack_remove(window, false);
  window_destroy(window);
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}