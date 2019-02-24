#ifndef _RADIOCONTROL_H_
#define _RADIOCONTROL_H_

#include <Arduino.h>

class RadioControl
{
  public:
    RadioControl();
    ~RadioControl();

    void init();
    void update(const uint32_t &cycleTime = 0);


  private:

};

#endif // _RADIOCONTROL_H_
