#include <Arduino.h>
#include "pumpAction.h"
#include "main.h"
#include "ui/ui.h"


unsigned long runPump() {
    unsigned long loc_startTime = millis();
    runstate = true;
    Serial.print("startTime: ");
    Serial.println(loc_startTime);
    digitalWrite(motorPin1, HIGH);
    analogWrite(motorPin2, 128);
    Serial.println("RUN");
    return loc_startTime;
}

unsigned long stopPump() {
    unsigned long loc_sleepStartTime = millis();
    runstate = false;
    digitalWrite(motorPin1, LOW);
    analogWrite(motorPin2, 0);
    Serial.println("STOP");
    return loc_sleepStartTime;
}



void updateUIElemsOff() {
    lv_label_set_text(objects.lbl_state_txt, "Off");
    lv_label_set_text(objects.lbl_btn_on_off, "On");
}

void updateUIElemsOn() {
    lv_label_set_text(objects.lbl_state_txt, "Run");
    lv_label_set_text(objects.lbl_btn_on_off, "Off");
}

void updateUIElems10sOff() {
    lv_label_set_text(objects.lbl_state_txt, "Off");
    lv_label_set_text(objects.lbl_btn_on_off, "On");
    lv_label_set_text(objects.lbl_time_cnt, "10");
    lv_label_set_text(objects.lbl_btn_ten_on_off, "10on/10off");
}

void updateUIElems10sOn() {
    lv_label_set_text(objects.lbl_state_txt, "Off");
    lv_label_set_text(objects.lbl_btn_on_off, "On");
    lv_label_set_text(objects.lbl_btn_ten_on_off, "30s");
}

