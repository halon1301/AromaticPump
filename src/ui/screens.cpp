#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"
#include <pumpAction.h>
#include <Arduino.h>
#include "main.h"

objects_t objects;
lv_obj_t *tick_value_change_obj;

unsigned long startTimeCounter = 0;

static void on_btnRunStatePress(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_PRESSED) {
        if (runstate) {
            Serial.println("off");
            updateUIElemsOff();
            sleepStartTime = stopPump();
            startTimeCounter = 0;
        } else {
            updateUIElemsOn();
            routine = 0;
            Serial.println("runstate true");
            startTime = runPump();
            startTimeCounter = startTime;
        }

    }
}

static void on_btnTenOnOff(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_PRESSED) {
        if (routine == 0) {
            // Check if currently running, if so, shut down and change the params
            Serial.println("off");
            updateUIElems10sOff();
            sleepStartTime = stopPump();
            startTimeCounter = 0;
            routine = 1;
        } else if (routine == 1) {
            if (runstate) {
                updateUIElems10sOff();
                sleepStartTime = stopPump();
                startTimeCounter = 0;
            } else {
                updateUIElems10sOn();
                routine = 0;
                startTime = runPump();
                startTimeCounter = startTime;
            }
        }
    }
}

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // btn_runState
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_run_state = obj;
            lv_obj_set_pos(obj, 114, 164);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, on_btnRunStatePress, LV_EVENT_ALL, 0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff32124), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lbl_btnOnOff
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_btn_on_off = obj;
            lv_obj_set_pos(obj, 151, 181);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "off");
        }
        {
            // lblTitle
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_title = obj;
            lv_obj_set_pos(obj, 103, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Lets Get High!");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // pnlState
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.pnl_state = obj;
            lv_obj_set_pos(obj, 7, 39);
            lv_obj_set_size(obj, 130, 85);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblState
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_state = obj;
            lv_obj_set_pos(obj, -128, -93);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "State:");
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // pnlTime
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.pnl_time = obj;
            lv_obj_set_pos(obj, 188, 35);
            lv_obj_set_size(obj, 127, 89);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_time = obj;
            lv_obj_set_pos(obj, 193, 19);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Time:");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // pnlOverrideUse
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.pnl_override_use = obj;
            lv_obj_set_pos(obj, 148, 130);
            lv_obj_set_size(obj, 64, 27);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblOverrideUse
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_override_use = obj;
            lv_obj_set_pos(obj, 74, 136);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "+10s Use:");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btnTenOnOff
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_ten_on_off = obj;
            lv_obj_set_pos(obj, 12, 164);
            lv_obj_set_size(obj, 83, 50);
            lv_obj_add_event_cb(obj, on_btnTenOnOff, LV_EVENT_ALL, 0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            // btnOverride
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_override = obj;
            lv_obj_set_pos(obj, 232, 164);
            lv_obj_set_size(obj, 79, 50);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            // lbl_btnOverride
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_btn_override = obj;
            lv_obj_set_pos(obj, 257, 181);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "+10s");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lbl_btnTenOnOff
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_btn_ten_on_off = obj;
            lv_obj_set_pos(obj, 18, 181);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "10on/10off");
        }
        {
            // lblOverrideUseCnt
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_override_use_cnt = obj;
            lv_obj_set_pos(obj, 176, 136);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "0");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ffea), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblTimeCnt
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_time_cnt = obj;
            lv_obj_set_pos(obj, 222, 54);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "30");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblStateTxt
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_state_txt = obj;
            lv_obj_set_pos(obj, 21, 58);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Off");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff5bff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

void tick_screen_main() {
}


void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}

typedef void (*tick_screen_func_t)();

tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};

void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
