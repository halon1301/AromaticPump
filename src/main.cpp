#include <Arduino.h>
#include <lvgl.h>
#include <M5Core2.h>
#include <SPI.h>
#include "config.h"
#include "ui/ui.h"


#define DEBUG

#ifdef DEBUG
#define LogDebug(...) Serial.println(__VA_ARGS__)
#define LogDebugFormatted(...) Serial.printf(__VA_ARGS__)
#else
#define LogDebug(...) ((void)0)
#define LogDebugFormatted(...) ((void)0)
#endif


// M5Stack config
bool touchDisabled = false;
bool vibrateMde = true;

// UI Config
bool darkMode = true;
int start_screen =0

// init TFT ESPI
static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;

M5Display *tft;
static lv_obj_t * kb;

void tft_lv_init() {
    M5.begin();
    lv_init();
    static lv_color_t buf1[(LV_HOR_RES_MAX * LV_VER_RES_MAX) /10];
    static lv_color_t buf2[(LV_HOR_RES_MAX * LV_VER_RES_MAX) /10];
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, (LV_HOR_RES_MAX * LV_VER_RES_MAX) /10);

    tft = &M5.Lcd;
}

// Display flushing
void flushDisplay(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft->startWrite();
    tft->setAddrWindow(area->x1, area->y1, w, h);
    tft->pushColors((uint16_t *)&color_p->full, w * h, true);
    tft->endWrite();

    lv_disp_flush_ready(disp);
}

void init_disp_driver() {
    lv_disp_drv_init(&disp_drv);  // Basic initialization

    disp_drv.flush_cb = flushDisplay;  // Set your driver function
    disp_drv.draw_buf = &draw_buf;      // Assign the buffer to the display
    disp_drv.hor_res = LV_HOR_RES_MAX;  // Set the horizontal resolution of the display
    disp_drv.ver_res = LV_VER_RES_MAX;  // Set the vertical resolution of the display

    lv_disp_drv_register(&disp_drv);                   // Finally register the driver
    lv_disp_set_bg_color(NULL, lv_color_hex3(0x000));  // Set default background color to black
}

void my_touchpad_read(lv_indev_drv_t * drv, lv_indev_data_t * data) {
    if(!touchDisabled){
        TouchPoint_t pos = M5.Touch.getPressPoint();
        bool touched = !(pos.x == -1);

        if(!touched) {
            data->state = LV_INDEV_STATE_RELEASED;
        } else {
            if (M5.BtnA.wasPressed()){  // tab 1 : A Button
                Serial.println("ButtonA");
                data->point.x = 80; data->point.y = 220; // mouse position x,y
                data->state =LV_INDEV_STATE_PR; M5.update();
            } else if (M5.BtnB.wasPressed()){  // tab 2 : B Button
                Serial.println("ButtonB");
                data->point.x = 160; data->point.y = 220;
                data->state =LV_INDEV_STATE_PR; M5.update();
            } else if (M5.BtnC.wasPressed()){  // tab 3 : C Button
                Serial.println("ButtonC");
                data->point.x = 270; data->point.y = 220;
                data->state =LV_INDEV_STATE_PR; M5.update();
            } else {
                data->state = LV_INDEV_STATE_PRESSED;
                data->point.x = pos.x;
                data->point.y = pos.y;
            }
        }
    }
}

void init_touch_driver() {
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);  // register
}

static uint32_t my_tick(void) {
    return millis();
}

void setup() {
    // write your initialization code here
    Serial.begin(115200);
    M5.begin();
    tft_lv_init();
    init_disp_driver();
    init_touch_driver();
    ui_init();

    LogDebug( "Setup done" );
}

void loop() {
    // write your code here

}