#ifndef OSW_APP_WATCHFACE_RIC_H
#define OSW_APP_WATCHFACE_RIC_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppWatchfaceRIC : public OswApp {
 public:
  OswAppWatchfaceRIC(void) { ui = OswUI::getInstance(); };
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppWatchfaceRIC(){};

 private:
  OswUI* ui;
  void drawWatch(OswHal* hal);
};

#endif
