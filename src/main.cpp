#include "M5Unified.h"
#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>
#include <SPI.h>
#include "ui/ui.h"
#include "main.h"
#include <pumpAction.h>
#include "remote.h"
#include <esp_timer.h>

const int displayHorizonal = 320;
const int displayVertical = 240;


const int remoteUserPin = 1; // GPIO pin the remote is sending a signal to
const int remoteAdminPin = 14;
int remoteUserStatus = 0;
int remoteAdminStatus = 0;


const unsigned long allowedRunTime = 30; // This is used to reset the runtimer and set the runtimer
unsigned long runtimer = allowedRunTime; // 30 seconds to run (Adjusted by the admin runtimer)
unsigned long minSleepTimer = 30; // low end of the sleep timer between runs (if admin override hasn't been used)
unsigned long maxSleepTimer = 120; // high sleep timer between runs (if admin override has been used fully)
int adminMax = 3; // number of presses the admin button has before it's ignored
unsigned long adminRuntimer = 10; // number of seconds the admin button allows runtime for over the run timer
int adminUse = 0; // used as a counter for usage times
int runCount = 0; // determine the number of runs in a period of elapsed time
int runCountTimer = 300; // The period of time passed when the runCount gets reset
unsigned long runCountStartTime = 0; // The millis() time the counter increments above 0, so run time can be tracked
unsigned long runCountElapsedTime = 0; // The elapsed millis() of the run time in the runCountTimer
int runCountMaxTime = 200; // The maximum amount of time in the runCountTimer allowed to run. When hit, will lockout until runCountTimer is passed.

unsigned int countdownTimer = 30;
bool runAllowed = true;
bool runstate = false;

unsigned long startTime;
unsigned long lastTime = 0;
unsigned long sleepStartTime = 0;
unsigned long currentTime;

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
    M5.begin();
    pinMode(remoteUserPin, INPUT);
    pinMode(remoteAdminPin, INPUT);
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
    Serial.println("Setup Complete.");
}

void loop() {
    M5.update();
    lv_task_handler();
    currentTime = millis();
    remoteUserStatus = digitalRead(remoteUserPin);
    remoteAdminStatus = digitalRead(remoteAdminPin);

    if (runAllowed) {
        if (runstate) {
            if (remoteUserStatus == 1) {
                sleepStartTime = stopPump();
                delay(1000);
            } else if (remoteAdminStatus == 1 and adminUse < adminMax) {
                // We add 10s to a max of an additional 30s
                adminBtnUse();
                delay(1000);
            }
        } else {
            if (remoteUserStatus == 1) {
                startTime = runPump(1);
                delay(1000);
            } else if (remoteAdminStatus == 1) {
                startTime = runPump(2);
                delay(1000);
            }

        } /*else if (remoteAdminStatus == 1) {
            startTime = runPump(2);
            adminBtnUse();
        }*/
    }

    /*
     * Logic:
     * if admin has been fully used, and the runtime has been exceeded: sleep for maxSleepTimer
     * if admin hasn't been fully used, but runtime exceeded, BUT max sleep hasn't been reached: allow run to max admin
     * if admin hasn't been fully used, but runtime exceeded, but max sleep reached: reset everything
     *
     * On top of the above, we also need to check the runCountTimer to make sure
    */
    if (adminUse == adminMax and currentTime - startTime >= runtimer * 1000 and runstate and runAllowed) {
        Serial.print("runtime exceeded, adminUse max, sleeping for max sleep time- ");
        sleepStartTime = stopPump();
        updateUIElemsOff();
        startTime = 0;
        adminUse = 0;
        runtimer = allowedRunTime;
        runAllowed = false;
    } else if (adminUse < adminMax and currentTime - startTime >= runtimer * 1000 and runstate and runAllowed) {
        Serial.print("runtime exceeded, adminUse under max, no sleep required  - Current Time");
        sleepStartTime = stopPump();
        updateUIElemsOff();
        runtimer = allowedRunTime; // timer can run again
        runAllowed = true;
    } else if (adminUse == adminMax and currentTime - sleepStartTime >= maxSleepTimer * 1000 and !runstate) {
        Serial.println("sleep time exceeded, admin use max, reset");
        startTime = 0;
        sleepStartTime = 0;
        adminUse = 0;
        runtimer = allowedRunTime;
        runAllowed = true;
    } else if (currentTime - sleepStartTime >= maxSleepTimer * 1000 and !runstate and !runAllowed) {
        Serial.println("sleep time exceeded, admin use max, reset");
        startTime = 0;
        sleepStartTime = 0;
        adminUse = 0;
        runtimer = allowedRunTime;
        runAllowed = true;
        resetUIElems();
    } else if (currentTime - sleepStartTime >= maxSleepTimer * 1000 and !runstate and runAllowed) {
        startTime = 0;
        sleepStartTime = 0;
        adminUse = 0;
        runtimer = allowedRunTime;
        runAllowed = true;
    } else {
        //if (currentTime - startTime >= 1000) {
        //    Serial.println(currentTime - startTime);
        //}
    }


    if (!runstate) {
        countdownTimer = 30;
        startTimeCounter = 0;
        lv_label_set_text_fmt(objects.lbl_time_cnt, "%d", countdownTimer);
    }

    if (startTimeCounter != 0) {
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

    delay(5);
}

