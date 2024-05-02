//
// Created by falconx on 29/04/24.
//
#include <Arduino.h>
#include "pumpAction.h"
#include "main.h"
#include "remote.h"
#include "ui/ui.h"


void remote_user_control() {
    startTime = runPump(1);
    Serial.println("user on");
}

void remote_admin_control() {
    startTime = runPump(2);
    Serial.println("admin on");
}

void adminBtnUse() {
    adminUse++;
    countdownTimer = countdownTimer + 10;
    runtimer = runtimer + adminRuntimer;
    lv_label_set_text_fmt(objects.lbl_override_use_cnt, "%d", adminUse);
}



/*void unused_function() {
    if (runAllowed) {
        if (remoteUserStatus == 1 and !runstate) {
            startTime = runPump();
            Serial.println("user on");
            updateUIElemsOn();
            delay(1000);
        } else if (remoteUserStatus == 1 and runstate) {
            sleepStartTime = stopPump();
            updateUIElemsOff();
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
            updateUIElemsOn();
            delay(1000); // slow down detection when it's an admin use
        }
    }
}*/