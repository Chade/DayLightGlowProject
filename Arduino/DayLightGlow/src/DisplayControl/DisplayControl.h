#ifndef _DISPLAYCONTROL_H_
#define _DISPLAYCONTROL_H_

#include <Arduino.h>

class DisplayControl
{
  public:
    DisplayControl();
    ~DisplayControl();

    void init();
    void update(uint32_t cycleTime = 0);


  private:

};

#endif // _DISPLAYCONTROL_H_