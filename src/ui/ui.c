#include "ui.h"
#include "images.h"
#include "main.h"

static int16_t currentScreen = -1;

static lv_obj_t *getLvglObjectFromIndex(int32_t index) {
    if (index == -1) {
        return 0;
    }
    return ((lv_obj_t **)&objects)[index];
}

static const void *getLvglImageByName(const char *name) {
    for (size_t imageIndex = 0; imageIndex < sizeof(images) / sizeof(ext_img_desc_t); imageIndex++) {
        if (strcmp(images[imageIndex].name, name) == 0) {
            return images[imageIndex].img_dsc;
        }
    }
    return 0;
}

void loadScreen(enum ScreensEnum screenId) {
    currentScreen = screenId - 1;
    lv_obj_t *screen = getLvglObjectFromIndex(currentScreen);
    lv_scr_load_anim(screen, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, true);
}

void ui_init() {
    create_screens();
    loadScreen(SCREEN_ID_DISCLAIMER);
}

void ui_tick() {
    tick_screen(currentScreen);
}

void updateUIElemsOff() {
    lv_label_set_text(objects.lbl_state_txt, "Off");
    lv_label_set_text(objects.lbl_btn_on_off, "On");
}

void updateUIElemsOn(int stateMap) {
    switch (stateMap) {
        case 0:
            lv_label_set_text(objects.lbl_state_txt, "On");
            lv_label_set_text_fmt(objects.lbl_flow_cur_rate, "%d", flowRate);
            break;
        case 1:
            lv_label_set_text(objects.lbl_state_txt, "Usr");
            lv_label_set_text_fmt(objects.lbl_flow_cur_rate, "%d", flowRate);
            break;
        case 2:
            lv_label_set_text(objects.lbl_state_txt, "Dm");
            break;
        default:
            lv_label_set_text(objects.lbl_state_txt, "On");
            lv_label_set_text_fmt(objects.lbl_flow_cur_rate, "%d", flowRate);
    }
    lv_label_set_text(objects.lbl_btn_on_off, "Off");
}

void updateFlowRateElems() {
    lv_label_set_text_fmt(objects.lbl_flow_cur_rate, "%d", flowRate);
}

void resetUIElems() {
    lv_label_set_text(objects.lbl_state_txt, "Off");
    lv_label_set_text(objects.lbl_btn_on_off, "On");
}