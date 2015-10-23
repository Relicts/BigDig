#include <pebble.h>
static Window *s_main_window;    // указатель на основное окно
static TextLayer *s_time_layer;  // указатель на текстовый слой с временем
static TextLayer *s_date_layer;  // указатель на текстовый слой с датой
static TextLayer *s_mess_layer;  // указатель на текстовый слой с сообщением
static Layer *s_bg_layer;        // указатель на слой фона
static GFont s_time_font;        // указатель на загруженные фонт
//static BitmapLayer *s_background_layer;  // указатель на слой фона
//static GBitmap *s_background_bitmap;     // указатель на битмап (картинку с bmp)
static GColor backcolor;         // цвет фона
static GColor forecolor;         // цвет шрифта
static GCompOp compop;           // режим композитинга
static int watchface_theme;      // тема циферблата 

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

  static char buffer[] = "RR.RR.RRRR";  // создаем буффер для сохранения
  
  // http://www.cplusplus.com/reference/ctime/strftime/
  strftime(buffer, sizeof("00.00.0000"), "%d.%m.%Y", tick_time);

  text_layer_set_text(s_date_layer, buffer);  // выводим время на текстовый слой
}

static void update_mess() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  static char buffer[] = "RRRRRRRRRRR";  // создаем буффер для сохранения
  strftime(buffer, sizeof("RRRRRRRRRRR"), "%B", tick_time);
  text_layer_set_text(s_mess_layer, buffer); 
}

static void update_bg(Layer *layer, GContext* ctx) {
  GRect bounds = layer_get_bounds(layer);

  graphics_context_set_fill_color(ctx, backcolor);     // Цвет фона
  graphics_context_set_compositing_mode(ctx, compop);  // Режим композитинга
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);     // Заливаем слой

  graphics_draw_rect(ctx, GRect(10, 20, 100, 120));
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  update_date();
  update_mess();
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);   // получаем ссылку на основной слой окна циферблата
  GRect window_bounds = layer_get_bounds(window_layer);  // получаем размеры всего окна циферблата
  switch (watchface_theme) {
    case 0:
      backcolor = GColorBlack;        // устанавливаем цвет фона - черный
      forecolor = GColorWhite;        // устанавливаем цвет шрифта - белый
      compop = GCompOpAssignInverted; // Режим композитинга - инвернтный
      break;
    case 1:
      backcolor = GColorWhite;        // устанавливаем цвет фона - белый
      forecolor = GColorBlack;        // устанавливаем цвет шрифта - черный
      compop = GCompOpAssign;         // Режим композитинга - нормальный
      break;
    default:
      backcolor = GColorClear;        // устанавливаем цвет фона - белый
      forecolor = GColorBlack;        // устанавливаем цвет шрифта - черный
      compop = GCompOpAssign;         // Режим композитинга - нормальный
  }  
  s_bg_layer = layer_create(window_bounds);      // создаем слой фона
  layer_add_child(window_layer, s_bg_layer);     // добавляем слой фона к основному слою 
  layer_set_update_proc(s_bg_layer, update_bg);  // прикрепляем обработчик слоя фона
//  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);  // получаем битмап из ресурса
//  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));                   // создаем слой для вывода битмапа
//  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);                  // выводим битмап на слой
//  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));// добавляем слой в окно
  // инициализация слоя для времени
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OSWALDBOLD_NUMBER_56));  // загружаем фонт из ресурса
  s_time_layer = text_layer_create(GRect(0, 27, 144, 150));    // создаем текстовый слой
  text_layer_set_background_color(s_time_layer, GColorClear);    // устанавливаем цвет фона
  text_layer_set_text_color(s_time_layer, forecolor);          // устанавливаем цвет шрифта
  text_layer_set_text(s_time_layer, "00:00");                  // выводим текст на слой
  text_layer_set_font(s_time_layer, s_time_font);                     // устанавливаем какой фонт будет использоваться при выводе текста на этот слой
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);  // устанваливаем выравнивание
  layer_add_child(s_bg_layer, text_layer_get_layer(s_time_layer));  // добавляем слой с временем в окно

  // инициализация слоя для даты
  // http://habrahabr.ru/company/madrobots/blog/204338/
  s_date_layer = text_layer_create(GRect(0, 90, 144, 75));    // создаем текстовый слой
  text_layer_set_background_color(s_date_layer, GColorClear);    // устанавливаем цвет фона
  text_layer_set_text_color(s_date_layer, forecolor);          // устанавливаем цвет шрифта
  text_layer_set_text(s_date_layer, "XX.XX.XXXX");             // выводим текст на слой
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  //text_layer_set_font(s_date_layer, s_time_font);                     // устанавливаем какой фонт будет использоваться при выводе текста на этот слой
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);  // устанваливаем выравнивание
  layer_add_child(s_bg_layer, text_layer_get_layer(s_date_layer));  // добавляем слой с временем в окно

  // инициализация слоя для сообщения
  s_mess_layer = text_layer_create(GRect(0, 110, 144, 50));    // создаем текстовый слой
  text_layer_set_background_color(s_mess_layer, GColorClear);    // устанавливаем цвет фона
  text_layer_set_text_color(s_mess_layer, forecolor);          // устанавливаем цвет шрифта
  text_layer_set_text(s_mess_layer, "XXXXXXXXXXX");             // выводим текст на слой
  text_layer_set_font(s_mess_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  //text_layer_set_font(s_date_layer, s_time_font);                     // устанавливаем какой фонт будет использоваться при выводе текста на этот слой
  text_layer_set_text_alignment(s_mess_layer, GTextAlignmentCenter);  // устанваливаем выравнивание
  layer_add_child(s_bg_layer, text_layer_get_layer(s_mess_layer));  // добавляем слой с временем в окно
  
}

static void main_window_unload(Window *window) {
//  gbitmap_destroy(s_background_bitmap);     // Destroy GBitmap
//  bitmap_layer_destroy(s_background_layer); // Destroy BitmapLayer
  fonts_unload_custom_font(s_time_font);    // Unload GFont
  text_layer_destroy(s_time_layer);         // уничтожаем слой для времени
  text_layer_destroy(s_date_layer);         // уничтожаем слой для даты
  text_layer_destroy(s_mess_layer);         // уничтожаем слой для сообщения
}

static void init() {
  watchface_theme = 0;
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