#ifndef _ENCODERCONTROL_H_
#define _ENCODERCONTROL_H_

#include <Arduino.h>
#include <Encoder.h>

#define PRESSED LOW
#define RELEASED HIGH

#define BUTTON_LONG_PRESS    800   // ms
#define BUTTON_SHORT_PRESS   120   // ms

class EncoderControl
{
  public:
    EncoderControl(uint8_t CLK, uint8_t DT, uint8_t SW);
    ~EncoderControl();

    void init();
    void update(const uint32_t &cycleTime = 0);

    int32_t getPosition();
    void setPosition(const int32_t &position);

    bool changed();

    bool up();
    bool down();
    bool left();
    bool right();
    bool shortPress();
    bool longPress();

    void reset();

  private:
    Encoder encoder;
    int32_t encoder_last = 0;

    const uint8_t button_pin;
    uint32_t button_time = 0;
    bool button_last = RELEASED;

    bool state_left  = false;
    bool state_right = false;
    bool state_up    = false;
    bool state_down  = false;
    bool state_short = false;
    bool state_long  = false;
    bool state_undef = false;

    uint32_t last_time = 0;
};

#endif // _ENCODERCONTROL_H_
