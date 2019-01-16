#ifndef _RADIOCONTROL_H_
#define _RADIOCONTROL_H_

#include <Arduino.h>

class EncoderControl
{
  public:
    RadioControl();
    ~RadioControl();

    void init();
    void update(uint32_t cycleTime = 0);


  private:

};

#endif // _RADIOCONTROL_H_