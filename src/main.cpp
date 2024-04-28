#include <M5Core2.h>
#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>
#include <SPI.h>
#include "ui/ui.h"
#include "main.h"
#include <pumpAction.h>
#include <AXP192.h>



const int displayHorizonal = 320;
const int displayVertical = 240;

const int remoteUserPin = 23; // GPIO pin the remote is sending a signal to
const int remoteAdminPin = 38;
int remoteUserStatus = 0;
int remoteAdminStatus = 0;
const unsigned long allowedRunTime = 30; // This is used to reset the runtimer and set the runtimer
unsigned long runtimer = allowedRunTime; // 30 seconds to run (Adjusted by the admin runtimer)
unsigned long minSleepTimer = 30; // low end of the sleep timer between runs (if admin override hasn't been used)
unsigned long maxSleepTimer = 120; // high sleep timer between runs (if admin override has been used fully)
int adminMax = 3; // number of presses the admin button has before it's ignored
unsigned long adminRuntimer = 10; // number of seconds the admin button allows runtime for over the run timer
int adminUse = 0; // used as a counter for usage times

bool touch_disabled = false;
unsigned int countdownTimer = 30;
bool runAllowed = true;
bool runstate = false;

unsigned long startTime;
unsigned long lastTime = 0;
unsigned long sleepStartTime = 0;
unsigned long currentTime;

// Debugging vars
bool doPrintMe = false;
bool doPrintMe2 = false;

/* Routines:
 * 0: standard 30s run time then off
 * 1: 10s on/10s off run until reset
*/
int routine = 0;


static lv_disp_draw_buf_t draw_buf; // Draw buffer
static lv_disp_drv_t disp_drv; // Display Driver
static lv_indev_drv_t indev_drv; // Touch driver

M5Display *tft;
static lv_obj_t * kb;

void tft_lv_initialization() {
    M5.begin();
    lv_init();
    static lv_color_t buf1[(displayHorizonal * displayVertical) / 10];  // Declare a buffer for 1/10 screen siz

    // Initialize `disp_buf` display buffer with the buffer(s).
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, (LV_HOR_RES_MAX * LV_VER_RES_MAX) / 10);

    tft = &M5.Lcd;
}

// Display flushing
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
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

    disp_drv.flush_cb = my_disp_flush;  // Set your driver function
    disp_drv.draw_buf = &draw_buf;      // Assign the buffer to the display
    disp_drv.hor_res = LV_HOR_RES_MAX;  // Set the horizontal resolution of the display
    disp_drv.ver_res = LV_VER_RES_MAX;  // Set the vertical resolution of the display

    lv_disp_drv_register(&disp_drv);                   // Finally register the driver
    lv_disp_set_bg_color(NULL, lv_color_hex3(0x000));  // Set default background color to black
}

void my_touchpad_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    if(!touch_disabled){
        TouchPoint_t pos = M5.Touch.getPressPoint();
        bool touched = pos.x != -1;

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

void exampleTabview(void) {
    lv_obj_t *tabview = lv_btn_create(lv_scr_act());
    tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Tab 1");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Tab 2");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Tab 3");
    lv_obj_t * label = lv_label_create(tab1);
    lv_label_set_text(label, "This the first tab\n\n"
                             "If the content\n"
                             "of a tab\n"
                             "becomes too\n"
                             "longer\n"
                             "than the\n"
                             "container\n"
                             "then it\n"
                             "automatically\n"
                             "becomes\n"
                             "scrollable.\n"
                             "\n"
                             "\n"
                             "\n"
                             "Can you see it?");
    label = lv_label_create(tab2);
    lv_label_set_text(label, "Second tab");

    label = lv_label_create(tab3);
    lv_label_set_text(label, "Third tab");

    lv_obj_scroll_to_view_recursive(label, LV_ANIM_ON);
}

void setup() {
    M5.begin();
    M5.Axp.SetBusPowerMode(1);
    tft_lv_initialization();
    init_disp_driver();
    init_touch_driver();
    if (!doPrintMe) {
        Serial.println(millis());
        doPrintMe = true;
    }
    ui_init();

}

void loop() {
    if (!doPrintMe2) {
        Serial.println(millis());
        doPrintMe2 = true;
    }


    lv_task_handler();
    currentTime = millis();
    remoteUserStatus = digitalRead(remoteUserPin);
    remoteAdminStatus = digitalRead(remoteAdminPin);
    Serial.print("CurrentTime: ");
    Serial.print(currentTime);
    Serial.print(" - StartTime: ");
    Serial.println(startTime);

    /*if (currentTime - startTime >= runtimer * 1000 and runstate) {
        Serial.print("runtime exceeded, adminUse under max, no sleep required  - Current Time");
        Serial.print(currentTime);
        Serial.print(" StartTime: ");
        Serial.println(startTime);
        Serial.print("Math: ");
        Serial.print(currentTime - startTime);
        Serial.print(" >= ");
        Serial.println(runtimer * 1000);
        Serial.print("runstate-pre: ");
        Serial.print(runstate);
        Serial.print(" - Admin use: ");
        Serial.println(adminUse);
        sleepStartTime = stopPump();
        Serial.print("runstate-post: ");
        Serial.println(runstate);
        updateUIElemsOff();
        runtimer = allowedRunTime; // timer can run again
        runAllowed = true;
    }*/

    /*
     * Logic:
     * if admin has been fully used, and the runtime has been exceeded: sleep for maxSleepTimer
     * if admin hasn't been fully used, but runtime exceeded, BUT max sleep hasn't been reached: allow run to max admin
     * if admin hasn't been fully used, but runtime exceeded, but max sleep reached: reset everything
    */
    if (adminUse == adminMax and currentTime - startTime >= runtimer * 1000 and runstate and runAllowed) {
        Serial.print("runtime exceeded, adminUse max, sleeping for max sleep time- ");
        Serial.print(currentTime);
        Serial.print(" - Admin use: ");
        Serial.println(adminUse);
        sleepStartTime = stopPump();
        updateUIElemsOff();
        startTime = 0;
        adminUse = 0;
        runtimer = allowedRunTime;
        runAllowed = false;
    } else if (adminUse < adminMax and currentTime - startTime >= runtimer * 1000 and runstate and runAllowed) {
        Serial.print("runtime exceeded, adminUse under max, no sleep required  - Current Time");
        Serial.print(currentTime);
        Serial.print(" StartTime: ");
        Serial.println(startTime);
        Serial.print("Math: ");
        Serial.print(currentTime - startTime);
        Serial.print(" >= ");
        Serial.println(runtimer * 1000);
        Serial.print("runstate-pre: ");
        Serial.print(runstate);
        Serial.print(" - Admin use: ");
        Serial.println(adminUse);
        sleepStartTime = stopPump();
        Serial.print("runstate-post: ");
        Serial.println(runstate);
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

    // Don't do any of the control state stuff yet.
    /*if(runAllowed) {
        if (remoteUserStatus == 1 and !runstate) {
            startTime = runPump();
            Serial.println("user on");
            lv_label_set_text(objects.labelArea1, "Run");
            delay(1000);
        } else if (remoteUserStatus == 1 and runstate) {
            sleepStartTime = stopPump();
            sleepStartTime = 0;
            delay(1000); // slow down detection on press
        } else if (remoteAdminStatus == 1 and runstate and adminUse < adminMax) {
            runtimer = runtimer + adminRuntimer;
            adminUse++;
            Serial.print("Admin use: ");
            Serial.println(adminUse);
            delay(1000); // slow down the detection after this, because otherwise the button gets detected
        } else if (remoteAdminStatus == 1 and adminUse < adminMax and !runstate) {
            adminUse++;
            startTime = runPump();
            Serial.println("AdminUse but no time adjustment");
            delay(1000); // slow down detection
        } else if (remoteAdminStatus == 1 and !runstate) {
            startTime = runPump();
            Serial.println("Admin on");
            lv_label_set_text(objects.labelArea1, "Run");
            delay(1000); // slow down detection when it's an admin use
        }
    }

     */
    if (!runstate) {
        if (routine == 0) {
            countdownTimer = 30;
            startTimeCounter = 0;
            lv_label_set_text_fmt(objects.lbl_time_cnt, "%d", countdownTimer);
        } else if (routine == 1) {
            countdownTimer = 10;
            startTimeCounter = 0;
            lv_label_set_text_fmt(objects.lbl_time_cnt, "%d", countdownTimer);
        }
    }

    if (startTimeCounter != 0) {
        if (currentTime - startTimeCounter >= 1000) { // 1 sec has passed
            if (countdownTimer != 0) {
                countdownTimer--;
                lv_label_set_text_fmt(objects.lbl_time_cnt, "%d", countdownTimer);
                startTimeCounter = currentTime;
                Serial.print("startTimeCounter: ");
                Serial.println(startTimeCounter);
                Serial.print("countdownTimer: ");
                Serial.println(countdownTimer);
            } else {
                if (routine == 0) {
                    lv_label_set_text(objects.lbl_time_cnt, "30");
                    countdownTimer = 30;
                } else if (routine == 1) {
                    lv_label_set_text(objects.lbl_time_cnt, "10");
                    countdownTimer = 10;
                }
                sleepStartTime = stopPump();
                lv_label_set_text(objects.lbl_state_txt, "Off");
                lv_label_set_text(objects.lbl_btn_on_off, "On");
                startTimeCounter = 0;
            }
        }
    }

    delay(5);
}

