#include <M5Unified.h>
#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>
#include "ui/ui.h"
#include "main.h"
#include "lib/pumpAction.h"
#include "lib/remote.h"
#include <esp_timer.h>
#include "lib/flow.h"

const int displayHorizonal = 320;
const int displayVertical = 240;

const int remoteUserPin = 26; // Button B
const int flowUpPin = 35; // Button C
const int flowDownPin = 25; // Button D
int remoteUserStatus = 0;
int flowUpStatus = 0;
int flowDownStatus = 0;

// The percentage of flow rate.  This is directly related to the PWM value passed on PIN2 of the motor driver, it starts and 75% and goes up or down from there
// 0 is 0%, 255 is 100%
unsigned int flowRate = 75; // 0.75 * 255 = 192
unsigned int flowRateAdj = 5; // The percentage the flow rate will change by


// Safety Timing
unsigned long runTimer = 300; // The period of time passed when the runLimit gets reset
unsigned long runMaxTime = 240; // The maximum amount of time in runTimer the pump can run, after this, it locks out until runTimer passes
unsigned long runTotalTime; // Total run time counter

unsigned int countdownTimer = 30;
bool runAllowed = true;
bool runstate = false;
bool boot = true; // If the m5stack has just booted, this is to capture the disclaimer timeout

unsigned long bootTime = 0;
unsigned long initialStartTime = 0;
unsigned long startTime;
unsigned long lastTime = 0;
unsigned long sleepStartTime = 0;
unsigned long currentTime;
unsigned long runElapsedTime = 0;

lv_display_t *display;
lv_indev_t *indev;
static lv_draw_buf_t *draw_buf1;

// Display flushing
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    lv_draw_sw_rgb565_swap(px_map, w*h);
    M5.Display.pushImageDMA<uint16_t>(area->x1, area->y1, w, h, (uint16_t *)px_map);
    lv_disp_flush_ready(disp);
}

uint32_t my_tick_function() {
    return(esp_timer_get_time() / 1000LL);
}

void my_touchpad_read(lv_indev_t * drv, lv_indev_data_t * data) {
   M5.update();
   auto count = M5.Touch.getCount();
   if (count == 0) {
       data->state = LV_INDEV_STATE_RELEASED;
   } else {
       auto touch = M5.Touch.getDetail(0);
       data->state = LV_INDEV_STATE_PRESSED;
       data->point.x = touch.x;
       data->point.y = touch.y;
   }
}

static void event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = reinterpret_cast<lv_obj_t *>(lv_event_get_user_data(e));

    switch (code)
    {
        case LV_EVENT_PRESSED:
            lv_label_set_text(label, "The last button event:\nLV_EVENT_PRESSED");
            break;
        case LV_EVENT_CLICKED:
            lv_label_set_text(label, "The last button event:\nLV_EVENT_CLICKED");
            break;
        case LV_EVENT_LONG_PRESSED:
            lv_label_set_text(label, "The last button event:\nLV_EVENT_LONG_PRESSED");
            break;
        case LV_EVENT_LONG_PRESSED_REPEAT:
            lv_label_set_text(label, "The last button event:\nLV_EVENT_LONG_PRESSED_REPEAT");
            break;
        default:
            break;
    }
}

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    pinMode(remoteUserPin, INPUT);
    pinMode(flowUpPin, INPUT);
    pinMode(flowDownPin, INPUT);
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    M5.Display.setEpdMode(epd_mode_t::epd_fast);
    if (M5.Display.width() < M5.Display.height()) {
        M5.Display.setRotation(M5.Display.getRotation() ^ 1);
    }
    lv_init();
    lv_tick_set_cb(my_tick_function);
    display = lv_display_create(displayHorizonal, displayVertical);
    lv_display_set_flush_cb(display, my_disp_flush);
    static lv_color_t buf1[displayHorizonal * 15];
    lv_display_set_buffers(display, buf1, nullptr, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);
    ui_init();
    Serial.println("Initialization complete");
    bootTime = millis();

}

void loop() {
    M5.update();
    lv_task_handler();
    currentTime = millis();
    if (boot) {
      if (currentTime - bootTime >= 1000) {
          if (countdownTimer != 0) {
              countdownTimer--;
              lv_label_set_text_fmt(objects.lbl_accept_btn, "%d", countdownTimer);
              bootTime = currentTime;
          } else {
               // Timer has runout
               boot = false;
               lv_label_set_text(objects.lbl_accept_btn, "I Will Be Smart");
          }
      }
    } else {
        remoteUserStatus = digitalRead(remoteUserPin);
        flowUpStatus = digitalRead(flowUpPin);
        flowDownStatus = digitalRead(flowDownPin);



        /*
         * if pump has run for runMaxTime in the runTimer, then it needs to shut off and be locked out until the runTimer elapses
         */

        if (runstate) {
            if (runAllowed) {
                if (remoteUserStatus == 1) {
                    sleepStartTime = stopPump();
                    delay(500);
                }
                if (flowUpStatus == 1) {
                    Serial.println("FlowUpRemote");
                    flowUpdateUp();
                    delay(500);
                }
                if (flowDownStatus == 1) {
                    Serial.println("FlowDownRemote");
                    flowUpdateDown();
                    delay(500);
                }
                if (currentTime - startTimeCounter >= 1000) { // 1 sec has passed
                    if (countdownTimer != 0) {
                        countdownTimer--;
                        lv_label_set_text_fmt(objects.lbl_time_cnt, "%d", countdownTimer);
                        startTimeCounter = currentTime;
                    } else {
                        sleepStartTime = stopPump();
                        lv_label_set_text(objects.lbl_state_txt, "Off");
                        lv_label_set_text(objects.lbl_btn_on_off, "On");
                        startTimeCounter = 0;
                    }
                }
            }
        } else {
            if (remoteUserStatus == 1) {
                startTime = runPump(1);
                delay(500);
            } else {
                countdownTimer = 30;
                startTimeCounter = 0;
                lv_label_set_text_fmt(objects.lbl_time_cnt, "%d", countdownTimer);
            }
        }
    }
    delay(5);

}

