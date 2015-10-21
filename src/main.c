#include <pebble.h>
static Window *s_main_window;    // указатель на основное окно
static TextLayer *s_time_layer;  // указатель на текстовый слой с временем
static TextLayer *s_date_layer;  // указатель на текстовый слой с датой
static GFont s_time_font;        // указатель на загруженные фонт
//static BitmapLayer *s_background_layer;  // указатель на слой фона
//static GBitmap *s_background_bitmap;     // указатель на битмап (картинку с bmp)

static void update_time() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  static char buffer[] = "00:00";  // создаем буффер для сохранения времени

  // записываем текущие часы и минуты в строковый буффер
  if(clock_is_24h_style() == true) {
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);  // Use 24 hour format
    //strncpy(buffer, "00:00", sizeof("00:00"));
  } else {
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);  // Use 12 hour format
  }

  text_layer_set_text(s_time_layer, buffer);  // выводим время на текстовый слой
}

static void update_date() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  static char buffer[] = "00.00.00";  // создаем буффер для сохранения

  strftime(buffer, sizeof("00.00.00"), "%I:%M", tick_time);  // Use 12 hour format

  text_layer_set_text(s_date_layer, buffer);  // выводим время на текстовый слой
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  update_date();
}

static void main_window_load(Window *window) {
//  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);  // получаем битмап из ресурса
//  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));                   // создаем слой для вывода битмапа
//  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);                  // выводим битмап на слой
//  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));// добавляем слой в окно
  
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIMES_NUMBER_56));  // загружаем фонт из ресурса
  s_time_layer = text_layer_create(GRect(0, 47, 144, 150));    // создаем текстовый слой
  text_layer_set_background_color(s_time_layer, GColorClear);  // устанавливаем цвет фона
  text_layer_set_text_color(s_time_layer, GColorBlack);        // устанавливаем цвет шрифта
  text_layer_set_text(s_time_layer, "00:00");                  // выводим текст на слой

  text_layer_set_font(s_time_layer, s_time_font);                     // устанавливаем какой фонт будет использоваться при выводе текста на этот слой
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);  // устанваливаем выравнивание

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));  // добавляем слой с временем в окно


  
  s_date_layer = text_layer_create(GRect(0, 100, 144, 150));    // создаем текстовый слой
  text_layer_set_background_color(s_date_layer, GColorClear);  // устанавливаем цвет фона
  text_layer_set_text_color(s_date_layer, GColorBlack);        // устанавливаем цвет шрифта
  text_layer_set_text(s_date_layer, "00.00.00");                  // выводим текст на слой

  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  //text_layer_set_font(s_date_layer, s_time_font);                     // устанавливаем какой фонт будет использоваться при выводе текста на этот слой
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);  // устанваливаем выравнивание

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));  // добавляем слой с временем в окно
}

static void main_window_unload(Window *window) {
//  gbitmap_destroy(s_background_bitmap);     // Destroy GBitmap
//  bitmap_layer_destroy(s_background_layer); // Destroy BitmapLayer
  fonts_unload_custom_font(s_time_font);    // Unload GFont
  text_layer_destroy(s_time_layer);         // уничтожаем слой для времени
}

static void init() {
  s_main_window = window_create();  // создаем элемент основного окна и присваиваем на указатель

  // устанавливаем обработчики для управления элементами внутри окна
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,          // установка
    .unload = main_window_unload       // обработчиков
  });
  
  window_stack_push(s_main_window, true);  // отображаем окно на часах, с animated=true
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);  // регистрируем свою процедуру tick_handler в TickTimerService
  update_time();  // делаем принудительный вызов для отображения времени при старте
}

static void deinit() {
  window_destroy(s_main_window);  // уничтожаем окно
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}