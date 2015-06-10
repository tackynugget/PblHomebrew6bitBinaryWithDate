#include <pebble.h>
#include <stdio.h>

Window *s_main_window;
TextLayer *text_layer;
//The current binary time
char binary_time[36], binary_hours[9], binary_minutes[9], binary_seconds[9], date_text[9];

//Convert integer into a 8 digit binary number
void getBin(int num, char *str)
{
  *(str+8) = '\0';
  int mask = 0x20 << 1;
  while(mask >>= 1)
    *str++ = !!(mask & num) + '0';
}

//Update the time and print it to the screen
static void update_time() {
  //Get the time and split it into decimal variables (hour, minute, time)
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  int hours = tick_time->tm_hour, minutes = tick_time->tm_min, seconds = tick_time->tm_sec;

  //If needed, convert the hours to reflect a 12 hour clock. AM/PM is not displayed
  //  on this watchface at this time
  if(clock_is_24h_style() == false && hours > 12)
      hours = hours - 12;
  
  //Use the decimal times to reassign the global binary time variables
  getBin(hours, binary_hours);
  getBin(minutes, binary_minutes);
  getBin(seconds, binary_seconds);
  
  //Combine the binary time variables to create the full time string.
  //  strcpy also serves to clear the previous string assigned to binary_time
  //  and reassign it to the beginning of what will be the new string.
  
  //Set the date text to later be printed to the date layer and therefore the screen
  strftime(date_text, sizeof(date_text), "\n%m%d%y", tick_time); 
    
  //THIS IS WHERE TO PUT THE DATE
  //Print the date text to the date layer and therefore the screen
  strcpy(binary_time, date_text);
  strcat(binary_time, "\n");
  strcat(binary_time, binary_hours);
  strcat(binary_time, "\n");
  strcat(binary_time, binary_minutes);
  strcat(binary_time, "\n");
  strcat(binary_time, binary_seconds);
  
  //Set the text layer to display the binary time, thereby printing it to the string.
  text_layer_set_text(text_layer, binary_time);
}

//Generate the main window
static void main_window_load(Window *window) {
  //Generate the text layer that will display the binary time
  text_layer = text_layer_create(GRect(0, -43, 144, 200));
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorGreen);
  text_layer_set_text(text_layer, "0DATE0\n0HOUR0\n0MINS0\n0SECS0");
  text_layer_set_font(text_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Share_Tech_Mono_40)));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);

  //Add the text layer to the window
   layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
}

//Destroy the main window
static void main_window_unload(Window *window) {
  text_layer_destroy(text_layer);
//  text_layer_destroy(date_layer);
}

//Calls the actions to occur every second
static void second_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
//  update_date();
}

//Initialize objects and processes
static void handle_init(void) {
  //Run the time updater every second
  tick_timer_service_subscribe(SECOND_UNIT, second_tick_handler);
  
  //Generate the main window
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
}

//Deinitialize everything
static void handle_deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
