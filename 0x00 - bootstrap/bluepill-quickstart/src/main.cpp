
/****** Include Section ******/
#include <Arduino.h>
#include "pins_config.h"
#include "LovyanGFX_Driver.h"
#include <lvgl.h>
#include <Wire.h>
#include <SPI.h>
#include <TCA9534.h>
// #include "ui.h"
/************************************/

#define SECTION_BANNER_LEN 28


/****** Global Objects Section ******/
LGFX   gfx;
TCA9534 ioex;
/************************************/

/******* LVGL Buffers Section *******/
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;
static lv_color_t *buf1 = nullptr;
/************************************/

/****** Screen Resolution Section ******/
static const uint16_t SCREEN_HOR_RES = 800;
static const uint16_t SCREEN_VER_RES = 480;
/***************************************/


/****** I2C Utilities Section ******/
bool i2cScan(uint8_t addr){
  Wire.beginTransmission(addr);
  return (Wire.endTransmission() == 0);
}
/***********************************/


/****** Backlight Control Section ******/
void enable_backlight(){
  bool has_mcu  = i2cScan(0x30);
  bool has_ioex = i2cScan(0x18);
  
  if (has_mcu) {
    Wire.beginTransmission(0x30);
    Wire.write(0x10);
    Wire.endTransmission();
    Wire.beginTransmission(0x30);
    Wire.write(0x18);
    Wire.endTransmission();
  } else if (has_ioex) {
    ioex.attach(Wire);
    ioex.setDeviceAddress(0x18);
    ioex.config(1, TCA9534::Config::OUT);
    ioex.config(2, TCA9534::Config::OUT);
    ioex.output(1, TCA9534::Level::H);
    delay(20);
    ioex.output(2, TCA9534::Level::H);
  }
}
/***************************************/


/****** LVGL Display Flush Section *****/

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p){
  const int32_t x = area->x1;
  const int32_t y = area->y1;
  const int32_t w = area->x2 - area->x1 + 1;
  const int32_t h = area->y2 - area->y1 + 1;
  const int32_t stripe_h = 16;
  lgfx::rgb565_t *src = (lgfx::rgb565_t *)&color_p->full;
  gfx.startWrite();
  for (int32_t row = 0; row < h; row += stripe_h) {
    int32_t ch = (h - row) > stripe_h ? stripe_h : (h - row);
    lgfx::rgb565_t *ptr = src + (size_t)row * (size_t)w;
    gfx.pushImageDMA(x, y + row, w, ch, ptr);
    gfx.waitDMA();
  }
  gfx.endWrite();
  lv_disp_flush_ready(disp);
}
/***************************************/


/******** Touchpad Read Section ********/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data){
  static bool     last_touched = false;
  static uint16_t last_x = 0, last_y = 0;
  uint16_t x, y;
  bool touched = gfx.getTouch(&x, &y);
  if (touched) {
    if (x >= SCREEN_HOR_RES) x = SCREEN_HOR_RES - 1;
    if (y >= SCREEN_VER_RES) y = SCREEN_VER_RES - 1;
    last_touched = true;
    last_x = x;
    last_y = y;
    data->state   = LV_INDEV_STATE_PR;
    data->point.x = last_x;
    data->point.y = last_y;
  } else {
    last_touched  = false;
    data->state   = LV_INDEV_STATE_REL;
  }
}
/***************************************/


void setup(){
  Serial.begin(115200);
  delay(200);
  Wire.begin(15, 16);
  delay(20);
  enable_backlight();
  #if CONFIG_SPIRAM_SUPPORT
    if (!psramInit()) {
      Serial.println("PSRAM init failed");
    } else {
      Serial.printf("PSRAM size: %d MB\n", ESP.getPsramSize() / 1024 / 1024);
    }
  #endif
  gfx.init();
  gfx.initDMA();
  gfx.fillScreen(TFT_BLACK);
  lv_init();
  const uint32_t lv_lines     = 40;
  const size_t   buffer_px    = (size_t)SCREEN_HOR_RES * lv_lines;
  const size_t   buffer_bytes = sizeof(lv_color_t) * buffer_px;
  buf = (lv_color_t *)heap_caps_malloc(buffer_bytes, MALLOC_CAP_DMA);
  buf1 = nullptr;
  if (!buf) {
    Serial.println("LVGL buffer allocation failed");
    while (1) delay(1000);
  }
  lv_disp_draw_buf_init(&draw_buf, buf, buf1, buffer_px);
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res  = SCREEN_HOR_RES;
  disp_drv.ver_res  = SCREEN_VER_RES;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type    = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
  // ui_init();
  Serial.println("Setup done");
}



void loop(){
  lv_timer_handler();
  delay(5);
}
