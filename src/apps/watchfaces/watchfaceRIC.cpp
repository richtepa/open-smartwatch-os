
#include "./apps/watchfaces/watchfaceRIC.h"
// #define GIF_BG

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

#ifdef GIF_BG
#include "./apps/_experiments/gif_player.h"
#endif

void OswAppWatchfaceRIC::drawWatch(OswHal* hal) {
  hal->gfx()->drawMinuteTicks(120, 120, 116, 112, ui->getForegroundDimmedColor());
  hal->gfx()->drawHourTicks(120, 120, 117, 107, ui->getForegroundColor());

  uint32_t steps = hal->getStepCount();

  uint32_t stepGoal = 10800;
  if(steps <= stepGoal){
      hal->gfx()->drawArc(120, 120, 0, 360 * steps / stepGoal, 90, 93, 6,ui->getInfoColor(), true);
  } else {
      hal->gfx()->drawArc(120, 120, 360 * (steps - stepGoal) / stepGoal, 360, 90, 93, 6,ui->getInfoColor(), true);
      hal->gfx()->drawArc(120, 120, 0, 360 * (steps - stepGoal) / stepGoal, 90, 93, 6, ui->getSuccessColor(), true);
  }

  // below two arcs take too long to draw

  // hal->gfx()->drawArc(120, 120, 0, 360, 180, 75, 7, changeColor(COLOR_GREEN, 0.25));
  // hal->gfx()->drawArc(120, 120, 0, (steps / 360) % 360, 180, 75, 7, dimColor(COLOR_GREEN, 25));
  // hal->gfx()->drawArc(120, 120, 0, (steps / 360) % 360, 180, 75, 6, COLOR_GREEN);

  // float bat = hal->getBatteryPercent() * 3.6;

  // hal->gfx()->drawArc(120, 120, 0, 360, 180, 57, 7, changeColor(COLOR_BLUE, 0.25));
  // hal->gfx()->drawArc(120, 120, 0, bat, 180, 57, 7, dimColor(COLOR_BLUE, 25));
  // hal->gfx()->drawArc(120, 120, 0, bat, 180, 57, 6, COLOR_BLUE);

  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  hal->getLocalTime(&hour, &minute, &second);

  uint32_t day = 0;
  uint32_t month = 0;
  uint32_t year = 0;
  hal->getDate(&day, &month, &year);

  // date (day)
  hal->gfx()->setTextSize(2.5);
  hal->gfx()->setTextCursor(DISP_W * 6 / 8, DISP_H / 2 + hal->gfx()->getTextOfsetRows(.5));
  hal->gfx()->print(day);

  // hours
  hal->gfx()->drawThickTick(120, 120, 0, 16, 360.0 / 12.0 * (1.0 * hour + minute / 60.0), 1, ui->getForegroundColor());
  hal->gfx()->drawThickTick(120, 120, 16, 60, 360.0 / 12.0 * (1.0 * hour + minute / 60.0), 4, ui->getForegroundColor());

  // minutes
  hal->gfx()->drawThickTick(120, 120, 0, 16, 360.0 / 60.0 * (1.0 * minute + second / 60.0), 1,
                            ui->getForegroundColor());
  hal->gfx()->drawThickTick(120, 120, 16, 105, 360.0 / 60.0 * (1.0 * minute + second / 60.0), 4,
                            ui->getForegroundColor());

#ifndef GIF_BG
  // seconds
  hal->gfx()->fillCircle(120, 120, 3, ui->getDangerColor());
  hal->gfx()->drawThickTick(120, 120, 0, 16, 360.0 / 60.0 * second, 1, ui->getDangerColor());
  hal->gfx()->drawThickTick(120, 120, 0, 110, 360.0 / 60.0 * second, 1, ui->getDangerColor());
#endif
}

#ifdef GIF_BG
OswAppGifPlayer* bgGif = new OswAppGifPlayer();
#endif

void OswAppWatchfaceRIC::setup(OswHal* hal) {
#ifdef GIF_BG
  bgGif->setup(hal);
#endif
}

void OswAppWatchfaceRIC::loop(OswHal* hal) {
  if (hal->btnHasGoneDown(BUTTON_3)) {
    hal->increaseBrightness(25);
  }
  if (hal->btnHasGoneDown(BUTTON_2)) {
    hal->decreaseBrightness(25);
  }

  if(hal->btnIsDownSince(BUTTON_3) > 500 && hal->btnIsDownSince(BUTTON_2) > 500 && resetStepsPossible){
      resetStepsPossible = false;
      hal->resetStepCount();
  } else {
      resetStepsPossible = true;
  }

#ifdef GIF_BG
  // if (millis() - 1000 > lastDraw) {
  bgGif->loop(hal);
  // lastDraw = millis();
  // }
#else
  hal->gfx()->fill(ui->getBackgroundColor());
#endif

  drawWatch(hal);
  hal->requestFlush();
}

void OswAppWatchfaceRIC::stop(OswHal* hal) {
#ifdef GIF_BG
  bgGif->stop(hal);
#endif
}
