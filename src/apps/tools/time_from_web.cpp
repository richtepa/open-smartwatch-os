
#include "./apps/tools/time_from_web.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <time.h>

#include "osw_ui_util.h"

bool manualSettingScreen = false;
int8_t manualSettingStep = 0;
int16_t manualSettingTimestamp[11];

void OswAppTimeFromWeb::setup(OswHal* hal) { hal->getWiFi()->setDebugStream(&Serial); }

void OswAppTimeFromWeb::loop(OswHal* hal) {
  if(!manualSettingScreen){

    hal->gfx()->fill(rgb565(0, 0, 0));
    hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
    hal->gfx()->setTextSize(2);

    hal->gfx()->setTextCursorBtn3();
    if (hal->getWiFi()->isConnected()) {
      hal->gfx()->print(LANG_DISCONNECT);
    } else {
      hal->gfx()->print(LANG_CONNECT);
      hal->gfx()->setTextCursorBtn2();
      hal->gfx()->print(LANG_MANUALLY);
    }

    if (hal->btn3Down()) {
      if (hal->getWiFi()->isConnected()) {
        hal->getWiFi()->disableWiFi();
      } else {
        hal->getWiFi()->joinWifi();
      }
      hal->clearBtn3();
    }

    if (hal->getWiFi()->isConnected()) {
      hal->gfx()->setTextCursorBtn2();
      hal->gfx()->print(LANG_TFW_UPDATE);
      if (hal->btn2Down()) {
        if (hal->getWiFi()->isConnected()) {
          Serial.println("updating...");

          hal->updateTimeViaNTP(TIMEZONE * 3600, DAYLIGHTOFFSET * 3600, 5 /*seconds*/);
          Serial.println("done...");
        }
        hal->clearBtn2();
      }
    } else {
      if (hal->btn2Down()) {
        uint32_t second = 0;
        uint32_t minute = 0;
        uint32_t hour = 0;
        uint32_t day = 0;
        uint32_t month = 0;
        uint32_t year = 0;
        hal->getLocalTime(&hour, &minute, &second);
        hal->getDate(&day, &month, &year);
        manualSettingTimestamp[0] = year%10;
        manualSettingTimestamp[1] = month/10;
        manualSettingTimestamp[2] = month%10;
        manualSettingTimestamp[3] = day/10;
        manualSettingTimestamp[4] = day%10;
        manualSettingTimestamp[5] = hour/10;
        manualSettingTimestamp[6] = hour%10;
        manualSettingTimestamp[7] = minute/10;
        manualSettingTimestamp[8] = minute%10;
        manualSettingTimestamp[9] = second/10;
        manualSettingTimestamp[10] = second%10;
        manualSettingScreen = true;
        hal->clearBtn2();
      }
    }

    hal->gfx()->setTextSize(4);
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(4), 120);

    uint32_t second = 0;
    uint32_t minute = 0;
    uint32_t hour = 0;
    hal->getLocalTime(&hour, &minute, &second);
    hal->gfx()->printDecimal(hour, 2);
    hal->gfx()->print(":");
    hal->gfx()->printDecimal(minute, 2);
    hal->gfx()->print(":");
    hal->gfx()->printDecimal(second, 2);

  } else {

    hal->gfx()->fill(rgb565(0, 0, 0));
    hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
    hal->gfx()->setTextSize(2);


    // Add-Button
    hal->gfx()->setTextCursorBtn3();
    hal->gfx()->print("+");
    if(manualSettingStep == 12){ // SAVE
      if (hal->btn3Down()) {
        // ToDo: Date
        int16_t yy = 2020 + manualSettingTimestamp[0];
        int8_t mm = manualSettingTimestamp[1] * 10 + manualSettingTimestamp[2] - 1;  // January = 0
        int8_t dd = manualSettingTimestamp[3] * 10 + manualSettingTimestamp[4];
        int8_t h = manualSettingTimestamp[5] * 10 + manualSettingTimestamp[6];
        int8_t m = manualSettingTimestamp[7] * 10 + manualSettingTimestamp[8];
        int8_t s = manualSettingTimestamp[9] * 10 + manualSettingTimestamp[10];
        struct tm date = {s,m,h,dd,mm,yy-1900};
        time_t epoch = mktime(&date);

        hal->setUTCTime(epoch - (TIMEZONE * 3600) - (DAYLIGHTOFFSET * 3600));
        manualSettingScreen = false;
        
        hal->clearBtn3();
      }
    } else if(manualSettingStep == 11){ // CANCEL
      if (hal->btn3Down()) {
        manualSettingScreen = false;
        
        hal->clearBtn3();
      }

    } else { // +1
      if (hal->btn3Down()) {
        manualSettingTimestamp[manualSettingStep] ++;
        
        if(manualSettingStep == 1){ // MONTHTEN
          if(manualSettingTimestamp[manualSettingStep] > 1){
            manualSettingTimestamp[manualSettingStep] = 0;
          }
        } else if(manualSettingStep == 3){ // DAYTEN
          if(manualSettingTimestamp[manualSettingStep] > 3){
            manualSettingTimestamp[manualSettingStep] = 0;
          }
        } else if(manualSettingStep == 5){ // HOURTEN
          if(manualSettingTimestamp[manualSettingStep] > 2){
            manualSettingTimestamp[manualSettingStep] = 0;
          }
        } else if(manualSettingStep == 7 || manualSettingStep == 9){ // MINTEN or SECTEN
          if(manualSettingTimestamp[manualSettingStep] > 5){
            manualSettingTimestamp[manualSettingStep] = 0;
          }
        } else {
          if(manualSettingTimestamp[manualSettingStep] > 9){ // other
            manualSettingTimestamp[manualSettingStep] = 0;
          }
        }

        hal->clearBtn3();
      }  
    }

    // Next-Button
    hal->gfx()->setTextCursorBtn2();
    hal->gfx()->print(">");
    if (hal->btn2Down()) {
      manualSettingStep ++;
      if(manualSettingStep > 12){
        manualSettingStep = 0;
      }
      hal->clearBtn2();
    }

    hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
    hal->gfx()->setTextSize(3);
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextLeftAligned();

    // Date
    hal->gfx()->setTextCursor((DISP_W/2) - hal->gfx()->getTextOfsetColumns(5), DISP_H * 3 / 8);
    hal->gfx()->print("202");
    for(int8_t i = 0; i < 5; i++){
      if(i == manualSettingStep){
        hal->gfx()->setTextColor(rgb565(255, 0, 0), rgb565(0, 0, 0));
      } else {
        hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
      }
      hal->gfx()->print(manualSettingTimestamp[i]);
      if(i == 0 || i == 2){
        hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
        hal->gfx()->print("-");
      }
    }

    // Timee
    hal->gfx()->setTextCursor((DISP_W/2) - hal->gfx()->getTextOfsetColumns(4), DISP_H / 2);
    for(int8_t i = 5; i < 11; i++){
      if(i == manualSettingStep){
        hal->gfx()->setTextColor(rgb565(255, 0, 0), rgb565(0, 0, 0));
      } else {
        hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
      }
      hal->gfx()->print(manualSettingTimestamp[i]);
      if(i == 6 || i == 8){
        hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
        hal->gfx()->print(":");
      }
    }

    hal->gfx()->setTextSize(2);

    // Cancel-Field
    hal->gfx()->setTextRightAligned();
    hal->gfx()->setTextCursor((DISP_W/2) - hal->gfx()->getTextOfsetColumns(0.5), DISP_H * 5 / 8);
    if(manualSettingStep == 11){
      hal->gfx()->setTextColor(rgb565(255, 0, 0), rgb565(0, 0, 0));
    } else {
      hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
    }   
    hal->gfx()->print(LANG_CANCEL);

    // Done-Field
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor((DISP_W/2) + hal->gfx()->getTextOfsetColumns(0.5), DISP_H * 5 / 8);
    if(manualSettingStep == 12){
      hal->gfx()->setTextColor(rgb565(255, 0, 0), rgb565(0, 0, 0));
    } else {
      hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
    } 
    hal->gfx()->print(LANG_SAVE);
  }

  hal->requestFlush();
}
void OswAppTimeFromWeb::stop(OswHal* hal) {}
