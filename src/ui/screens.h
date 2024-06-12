#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *disclaimer;
    lv_obj_t *main;
    lv_obj_t *btn_accept;
    lv_obj_t *btn_flow_down;
    lv_obj_t *btn_flow_up;
    lv_obj_t *btn_run_state;
    lv_obj_t *lbl_accept_btn;
    lv_obj_t *lbl_btn_on_off;
    lv_obj_t *lbl_flow_cur_rate;
    lv_obj_t *lbl_flow_down;
    lv_obj_t *lbl_flow_rate;
    lv_obj_t *lbl_flow_up;
    lv_obj_t *lbl_state;
    lv_obj_t *lbl_state_txt;
    lv_obj_t *lbl_time;
    lv_obj_t *lbl_time_cnt;
    lv_obj_t *lbl_title;
    lv_obj_t *lbl_warning;
    lv_obj_t *pnl_flow_rate_status;
    lv_obj_t *pnl_state;
    lv_obj_t *pnl_time;
    lv_obj_t *txt_disclaimer_text;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_DISCLAIMER = 1,
    SCREEN_ID_MAIN = 2,
};

void create_screen_disclaimer();
void tick_screen_disclaimer();

void create_screen_main();
void tick_screen_main();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/