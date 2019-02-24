#ifndef _DISPLAYCONTROL_H_
#define _DISPLAYCONTROL_H_

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <TimeLib.h>


class DisplayControl
{
  public:
    DisplayControl();
    ~DisplayControl();

    void init();
    void update(const uint32_t &cycleTime = 0);

    void setVolume(const uint8_t &volume, const uint8_t &mute);
    void setFrequency(const uint16_t &frequency);
    void setTime(const time_t &time);
    void setAlarm(const time_t &time, const uint8_t &num, const uint8_t &enabled);
    void forceUpdate();

    void drawTimeSetup(const time_t &time, const char *caption);

  private:
#ifdef FULL_BUFFER
    U8G2_SH1106_128X64_NONAME_F_HW_I2C display;
#else
    U8G2_SH1106_128X64_NONAME_1_HW_I2C display;
#endif

    uint32_t last_cycle = 0;
    uint8_t current_volume = 0;
    uint8_t current_mute = 0;
    time_t current_time = 0;
    time_t current_alarm0 = 0;
    time_t current_alarm1 = 0;
    uint8_t current_alarm0enabled = 0;
    uint8_t current_alarm1enabled = 0;
    uint16_t current_frequency = 8750;

    bool do_update = false;

    void drawVolume();
    void drawFrequency();
    void drawTime();
    void drawAlarms();
    void drawControls();

};

#endif // _DISPLAYCONTROL_H_
