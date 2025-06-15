/******************************************************************************
  This code is made with the use of a example code from the waveshare ESP32 S3 documentation using the built in accelerometer to create a graph. Can be found here : 
  https://www.waveshare.com/wiki/ESP32-S3-LCD-1.69#04_QMI8658

  We modified it to fit our project and used Chat GPT during the process as aid. 
******************************************************************************/

#include <lvgl.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include "lv_conf.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "HWCDC.h"
#include "temperature_sensor.h"

HWCDC USBSerial;

#define EXAMPLE_LVGL_TICK_PERIOD_MS 2
#define BUZZER_PIN 42

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[LCD_WIDTH * LCD_HEIGHT / 10];

lv_obj_t *tvoc_chart;
lv_obj_t *co2_chart;
lv_chart_series_t *tvoc_series;
lv_chart_series_t *Co2_series;
lv_obj_t *co2_label, *tvoc_label, *temp_label;

int tvoc_min = 9999, tvoc_max = 0;
int co2_min = 9999, co2_max = 0;

bool buzzer_active = false;
unsigned long buzzer_start_time = 0;

temperature_sensor_handle_t temp_sensor = NULL;

Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI);
Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST, 0, true, LCD_WIDTH, LCD_HEIGHT, 0, 20, 0, 0);

// Incoming data structure from sender 
typedef struct struct_message {
  float temperature;  // Not used
  uint16_t co2;
  uint16_t tvoc;
} struct_message;

struct_message incomingData;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif
  lv_disp_flush_ready(disp);
}

void lvgl_tick(void *arg) {
  lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int data_len) {
  if (data_len != sizeof(incomingData)) return;
  memcpy(&incomingData, data, sizeof(incomingData));

  // Update min/max for dynamic scaling
  tvoc_min = min(tvoc_min, (int)incomingData.tvoc);
  tvoc_max = max(tvoc_max, (int)incomingData.tvoc);
  co2_min = min(co2_min, (int)incomingData.co2);
  co2_max = max(co2_max, (int)incomingData.co2);

  int tvoc_range_min = max(0, tvoc_min - 10);
  int tvoc_range_max = tvoc_max + 10;
  int co2_range_min = max(0, co2_min - 50);
  int co2_range_max = co2_max + 50;

  lv_chart_set_range(tvoc_chart, LV_CHART_AXIS_PRIMARY_Y, tvoc_range_min, tvoc_range_max);
  lv_chart_set_range(co2_chart, LV_CHART_AXIS_PRIMARY_Y, co2_range_min, co2_range_max);

  // Update charts
  lv_chart_set_next_value(tvoc_chart, tvoc_series, incomingData.tvoc);
  lv_chart_set_next_value(co2_chart, Co2_series, incomingData.co2);

  // Internal temperature Only for monitoring the internal temp becuase the display got very hot. 
  float internal_temp = 0;
  esp_err_t temp_err = temperature_sensor_get_celsius(temp_sensor, &internal_temp);
  char buf[32];

  if (temp_err == ESP_OK) {
    snprintf(buf, sizeof(buf), "Temp: %.1f °C", internal_temp);
  } else {
    snprintf(buf, sizeof(buf), "Temp: N/A");
    USBSerial.printf("Temp read failed (err %d)\n", temp_err);
  }
  lv_label_set_text(temp_label, buf);

  snprintf(buf, sizeof(buf), "CO2: %d ppm", incomingData.co2);
  lv_label_set_text(co2_label, buf);

  snprintf(buf, sizeof(buf), "TVOC: %d ppb", incomingData.tvoc);
  lv_label_set_text(tvoc_label, buf);

  // CO2 alert
  if (incomingData.co2 > 1500 && !buzzer_active) {
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xFF0000), LV_PART_MAIN);
    digitalWrite(BUZZER_PIN, HIGH);
    buzzer_start_time = millis();
    buzzer_active = true;
  }

}
  


void setup() {
  USBSerial.begin(115200);
  gfx->begin();

  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, LCD_WIDTH * LCD_HEIGHT / 10);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_WIDTH;
  disp_drv.ver_res = LCD_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // TVOC chart
  tvoc_chart = lv_chart_create(lv_scr_act());
  lv_obj_set_size(tvoc_chart, 240, 90);
  lv_obj_align(tvoc_chart, LV_ALIGN_TOP_MID, 0, 10);
  lv_chart_set_type(tvoc_chart, LV_CHART_TYPE_LINE);
  lv_chart_set_range(tvoc_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 200);
  lv_chart_set_point_count(tvoc_chart, 50);
  tvoc_series = lv_chart_add_series(tvoc_chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);

  // CO2 chart
  co2_chart = lv_chart_create(lv_scr_act());
  lv_obj_set_size(co2_chart, 240, 90);
  lv_obj_align(co2_chart, LV_ALIGN_TOP_MID, 0, 110);
  lv_chart_set_type(co2_chart, LV_CHART_TYPE_LINE);
  lv_chart_set_range(co2_chart, LV_CHART_AXIS_PRIMARY_Y, 400, 1000);
  lv_chart_set_point_count(co2_chart, 50);
  Co2_series = lv_chart_add_series(co2_chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);

  // Labels
  temp_label = lv_label_create(lv_scr_act());
  lv_obj_align(temp_label, LV_ALIGN_BOTTOM_LEFT, 80, -25);
  lv_label_set_text(temp_label, "ESP Temp: -- °C");
  

  co2_label = lv_label_create(lv_scr_act());
  lv_obj_align(co2_label, LV_ALIGN_BOTTOM_LEFT, 80, -45);
  lv_label_set_text(co2_label, "CO2: -- ppm");
  lv_obj_set_style_text_color(co2_label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);

  tvoc_label = lv_label_create(lv_scr_act());
  lv_obj_align(tvoc_label, LV_ALIGN_BOTTOM_LEFT, 80, -60);
  lv_label_set_text(tvoc_label, "TVOC: -- ppb");
  lv_obj_set_style_text_color(tvoc_label, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);


  // MAC address
  WiFi.mode(WIFI_STA);
  String macStr = "MAC: " + WiFi.macAddress();
  lv_obj_t *mac_label = lv_label_create(lv_scr_act());
  //lv_label_set_text(mac_label, macStr.c_str());
  //lv_obj_align(mac_label, LV_ALIGN_BOTTOM_MID, 0, -5);

  //printMacAddress();

  // LVGL tick
  const esp_timer_create_args_t lvgl_tick_timer_args = {
    .callback = &lvgl_tick,
    .name = "lvgl_tick"
  };
  esp_timer_handle_t lvgl_tick_timer = NULL;
  esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
  esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000);

  // Install and enable temperature sensor (with error checks)
  temperature_sensor_config_t config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
  esp_err_t err = temperature_sensor_install(&config, &temp_sensor);
  if (err != ESP_OK) {
    USBSerial.printf("Temp sensor install failed (err %d)\n", err);
  } else {
    err = temperature_sensor_enable(temp_sensor);
    if (err != ESP_OK) {
      USBSerial.printf("Temp sensor enable failed (err %d)\n", err);
    } else {
      USBSerial.println("Internal temperature sensor ready");
    }
  }

  // ESP-NOW
  if (esp_now_init() != ESP_OK) {
    USBSerial.println("ESP-NOW init failed");
    while (1);
  }
  esp_now_register_recv_cb(OnDataRecv);
  USBSerial.println("ESP-NOW receiver ready");
}

void loop() {
  lv_timer_handler();

  if (buzzer_active && millis() - buzzer_start_time > 5000) {
    digitalWrite(BUZZER_PIN, LOW);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);
    buzzer_active = false;
  }

  delay(5);
}

