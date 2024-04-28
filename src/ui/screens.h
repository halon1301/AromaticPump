#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *btn_override;
    lv_obj_t *btn_run_state;
    lv_obj_t *lbl_btn_on_off;
    lv_obj_t *lbl_btn_override;
    lv_obj_t *lbl_override_use;
    lv_obj_t *lbl_override_use_cnt;
    lv_obj_t *lbl_state;
    lv_obj_t *lbl_state_txt;
    lv_obj_t *lbl_time;
    lv_obj_t *lbl_time_cnt;
    lv_obj_t *lbl_title;
    lv_obj_t *pnl_override_use;
    lv_obj_t *pnl_state;
    lv_obj_t *pnl_time;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

void create_screen_main();
void tick_screen_main();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/