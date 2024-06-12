#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"
#include "lib/pumpAction.h"
#include <Arduino.h>
#include "main.h"
#include "lib/remote.h"
#include "lib/flow.h"
#include "M5Unified.h"

objects_t objects;
lv_obj_t *tick_value_change_obj;

unsigned long startTimeCounter = 0;

static void on_btn_acceptStatePress(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    // Don't actually accept a button press for 30 sec

    if (event == LV_EVENT_PRESSED) {
        if (!boot) {
            loadScreen(SCREEN_ID_MAIN);
        }
    }
}

static void on_btnRunStatePress(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_PRESSED) {
        if (runAllowed) {
            if (runstate) {
                Serial.println("off");
                updateUIElemsOff();
                sleepStartTime = stopPump();
            } else {
                updateUIElemsOn(0);
                Serial.println("runstate true");
                startTime = runPump(0);
            }
        }
    }
}

static void on_btnFlowUp(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_PRESSED) {
        // Check current flow rate, if it's 100% it can't go up any higher
        if (runstate) {
            flowUpdateUp();
        }
    }
}

static void on_btnFlowDown(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_PRESSED) {
        if (runstate) {
            flowUpdateDown();
        }
    }
}

void create_screen_disclaimer() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.disclaimer = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // txt_DisclaimerText
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.txt_disclaimer_text = obj;
            lv_obj_set_pos(obj, 10, 38);
            lv_obj_set_size(obj, 300, 163);
            lv_textarea_set_max_length(obj, 1024);
            lv_textarea_set_text(obj, "This device can be dangerous, it is not to be used in unsafe ways.\n- DO NOT USE WHILE INTOXICATED\n- DO NOT USE WITH DANGEROUS SUBSTANCES\n- DO NOT USE ALONE\n- BE SMART\n- YOU ARE RESPONSIBLE FOR YOUR OWN ACTIONS");
            lv_textarea_set_one_line(obj, false);
            lv_textarea_set_password_mode(obj, false);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btn_accept
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_accept = obj;
            lv_obj_set_pos(obj, 84, 201);
            lv_obj_set_size(obj, 152, 39);
            lv_obj_add_event_cb(obj, on_btn_acceptStatePress, LV_EVENT_ALL, 0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffbe0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lbl_AcceptBtn
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_accept_btn = obj;
            lv_obj_set_pos(obj, 106, 213);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "30");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xfffefefe), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_decor(obj, LV_TEXT_DECOR_UNDERLINE, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lbl_Warning
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_warning = obj;
            lv_obj_set_pos(obj, 61, 11);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "!!!!! WARNING !!!!!");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}


void tick_screen_disclaimer() {
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
            lv_obj_set_pos(obj, 6, 142);
            lv_obj_set_size(obj, 199, 92);
            lv_obj_add_event_cb(obj, on_btnRunStatePress, LV_EVENT_ALL, 0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff32124), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lbl_btnOnOff
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_btn_on_off = obj;
            lv_obj_set_pos(obj, 96, 184);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "off");
        }
        {
            // lblTitle
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_title = obj;
            lv_obj_set_pos(obj, 102, 6);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Lets Get High!");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // pnlState
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.pnl_state = obj;
            lv_obj_set_pos(obj, 6, 45);
            lv_obj_set_size(obj, 109, 85);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblState
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_state = obj;
            lv_obj_set_pos(obj, -129, -87);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "State:");
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // pnlTime
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.pnl_time = obj;
            lv_obj_set_pos(obj, 119, 45);
            lv_obj_set_size(obj, 86, 85);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_time = obj;
            lv_obj_set_pos(obj, 118, 25);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Time:");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btnFlowDown
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_flow_down = obj;
            lv_obj_set_pos(obj, 238, 184);
            lv_obj_set_size(obj, 65, 50);
            lv_obj_add_event_cb(obj, on_btnFlowDown, LV_EVENT_ALL, 0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            // lblTimeCnt
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_time_cnt = obj;
            lv_obj_set_pos(obj, 133, 62);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "30");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblStateTxt
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_state_txt = obj;
            lv_obj_set_pos(obj, 20, 64);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Off");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff5bff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btnFlowUp
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_flow_up = obj;
            lv_obj_set_pos(obj, 238, 45);
            lv_obj_set_size(obj, 65, 50);
            lv_obj_add_event_cb(obj, on_btnFlowUp, LV_EVENT_ALL, 0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            // lblFlowRate
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_flow_rate = obj;
            lv_obj_set_pos(obj, 223, 22);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Flow Rate %:");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // pnlFlowRateStatus
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.pnl_flow_rate_status = obj;
            lv_obj_set_pos(obj, 226, 104);
            lv_obj_set_size(obj, 88, 70);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, false);
            lv_textarea_set_password_mode(obj, false);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lblFlowUp
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_flow_up = obj;
            lv_obj_set_pos(obj, 260, 62);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Up");
        }
        {
            // lblFlowDown
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_flow_down = obj;
            lv_obj_set_pos(obj, 249, 201);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Down");
        }
        {
            // lblFlowCurRate
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_flow_cur_rate = obj;
            lv_obj_set_pos(obj, 233, 114);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text_fmt(obj, "%d", flowRate);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_44, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

void tick_screen_main() {
}


void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);

    create_screen_disclaimer();
    create_screen_main();
}

typedef void (*tick_screen_func_t)();

tick_screen_func_t tick_screen_funcs[] = {
        tick_screen_disclaimer,
        tick_screen_main,
};

void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
