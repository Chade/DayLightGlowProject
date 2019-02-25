#ifndef _DAYLIGHTGLOW_H_
#define _DAYLIGHTGLOW_H_

#include "src/SensorLamp/SensorLamp.h"
#include "src/EncoderControl/EncoderControl.h"
#include "src/DisplayControl/DisplayControl.h"

// *****************************************************************************
// Communication
// *****************************************************************************

#define BAUD_RATE                    115200

// *****************************************************************************
// Pins
// *****************************************************************************

// SensorLamp
#define LED_WHITE_PIN                5
#define LED_BLUE_PIN                 6
#define LED_RED_PIN                  9
#define LED_GREEN_PIN                10
#define SENSOR_PIN                   A0

// Encoder
#define ENCODER_LEFT_A_PIN           3
#define ENCODER_LEFT_B_PIN           7
#define ENCODER_LEFT_BUTTON_PIN      11
#define ENCODER_RIGHT_A_PIN          2
#define ENCODER_RIGHT_B_PIN          8
#define ENCODER_RIGHT_BUTTON_PIN     12

// *****************************************************************************
// Delays
// *****************************************************************************

#define CYCLE_TIME                   10

// *****************************************************************************
// Display settings
// *****************************************************************************

#define SCREEN_HOME                  0
#define SCREEN_ALARM0                1
#define SCREEN_ALARM1                2
#define SCREEN_TIME                  3

// *****************************************************************************
// EEPROM
// *****************************************************************************

#define EEPROM_VOLUME                0
#define EEPROM_FREQUENCY             1
#define EEPROM_ALARM0                3
#define EEPROM_ALARM1                7
#define EEPROM_ALARM0_ENABLED        11
#define EEPROM_ALARM1_ENABLED        12

// *****************************************************************************
// General
// *****************************************************************************

#define UNDEF                        0
#define OK                           1
#define TOGGLE                       2
#define CANCEL                       -1

#define ACTIVE_SLEEP                 0
#define ACTIVE_SNOOZE                1
#define ACTIVE_PREWAKE               3
#define ACTIVE_WAKEUP                7

#define FREQUENCY_MIN                8750
#define FREQUENCY_MAX                10800
#define FREQUENCY_STEP_LONG          100
#define FREQUENCY_STEP_SHORT         10
#define VOLUME_MAX                   63

#define START_LIGHT                  1200
#define STOP_ALARM                   900
#define SNOOZE                       540

// *****************************************************************************
// Global variables
// *****************************************************************************

uint8_t  screenMode = SCREEN_HOME;
uint8_t  sleepMode = ACTIVE_SLEEP;
uint8_t  mute = 1;
uint8_t  volume;
uint16_t frequency;
time_t   dateTime;
time_t   alarm0;
time_t   alarm1;
uint8_t  alarm0enabled;
uint8_t  alarm1enabled;
time_t   snooze;

// *****************************************************************************
// Classes
// *****************************************************************************

SensorLamp sensorLamp(SENSOR_PIN, LED_WHITE_PIN);
EncoderControl encoderLeft(ENCODER_LEFT_A_PIN, ENCODER_LEFT_B_PIN, ENCODER_LEFT_BUTTON_PIN);
EncoderControl encoderRight(ENCODER_RIGHT_A_PIN, ENCODER_RIGHT_B_PIN, ENCODER_RIGHT_BUTTON_PIN);
DisplayControl oled;

// ================================================================================
// Functions
// ================================================================================

uint8_t drawHomeScreen()
{
    ////////////////////////////////////////
    // Handle dual click
    ////////////////////////////////////////

    if (encoderLeft.longPress() && encoderRight.longPress())
    {
        dateTime = now();
        encoderLeft.reset();
        encoderRight.reset();
        return SCREEN_TIME;
    }

    ////////////////////////////////////////
    // Handle left encoder
    ////////////////////////////////////////
    if (encoderLeft.changed())
    {
        int8_t step = 0;

        if (encoderLeft.left())
        {
            step = -FREQUENCY_STEP_SHORT;
        }
        else if (encoderLeft.right())
        {
            step = FREQUENCY_STEP_SHORT;
        }
        else if (encoderLeft.up())
        {
            step = FREQUENCY_STEP_LONG;
        }
        else if (encoderLeft.down())
        {
            step = -FREQUENCY_STEP_LONG;
        }
        else if (encoderLeft.shortPress())
        {
            uint16_t newfrequency = frequency;
            Radio.FMRM_audio_mute(1);

            do
            {
                if ((newfrequency + FREQUENCY_STEP_SHORT) <= FREQUENCY_MAX)
                    newfrequency += FREQUENCY_STEP_SHORT;
                else
                    newfrequency = FREQUENCY_MIN;

                Radio.FMRM_frequency_set(newfrequency);
                // Instead of delay update display
                oled.setFrequency(newfrequency);
                oled.update();
                Radio.FMRM_tune_status_get();
            } while ((Radio.FMRM_is_valid() == 0) && (newfrequency != frequency));

            delay(500);
            frequency = Radio.FMRM_frequency_get();
            Radio.FMRM_audio_mute(mute);
        }
        else if (encoderLeft.longPress())
        {
            EEPROM.get(EEPROM_ALARM0, dateTime);
            encoderLeft.reset();
            return SCREEN_ALARM0;
        }

        if (step != 0)
        {
            if ((frequency + step) > FREQUENCY_MAX)
                frequency = FREQUENCY_MIN;
            else if ((frequency + step) < FREQUENCY_MIN)
                frequency = FREQUENCY_MAX;
            else
                frequency += step;

            Radio.FMRM_frequency_set(frequency);
        }

        oled.setFrequency(frequency);
        encoderLeft.reset();
    }

    ////////////////////////////////////////
    // Handle right encoder
    ////////////////////////////////////////
    if (encoderRight.changed())
    {
        if (encoderRight.left())
        {
            if (volume > 0)
            {
                volume--;
                Radio.FMRM_volume_set(volume);
            }
        }
        else if (encoderRight.right())
        {
            if (volume < VOLUME_MAX)
            {
                volume++;
                Radio.FMRM_volume_set(volume);
            }
        }
        else if (encoderRight.shortPress())
        {
            mute = !mute;
            Radio.FMRM_audio_mute(mute);
            oled.setVolume(volume, mute);
            oled.forceUpdate();
        }
        else if (encoderRight.longPress())
        {
            EEPROM.get(EEPROM_ALARM1, dateTime);
            encoderRight.reset();
            return SCREEN_ALARM1;
        }

        oled.setVolume(volume, mute);
        encoderRight.reset();
    }

    oled.setTime(now());
    oled.update();

    return SCREEN_HOME;
}

int8_t drawSetupScreen(time_t &time, const char *caption)
{
    int8_t ret = UNDEF;
    TimeElements tm;
    breakTime(time, tm);

    ////////////////////////////////////////
    // Handle left encoder
    ////////////////////////////////////////
    if (encoderLeft.changed())
    {
        if (encoderLeft.left())
        {
            tm.Hour = (tm.Hour >= 1 ) ? (tm.Hour - 1 ) : (tm.Hour + 23);
        }
        else if (encoderLeft.right())
        {
            tm.Hour = (tm.Hour < 23) ? (tm.Hour + 1 ) : (tm.Hour - 23);
        }
        else if (encoderLeft.shortPress())
        {
            ret = OK;
        }
        else if (encoderLeft.longPress())
        {
            ret = TOGGLE;
        }
        time = makeTime(tm);
        oled.drawTimeSetup(time, caption);
        encoderLeft.reset();
    }

    ////////////////////////////////////////
    // Handle right encoder
    ////////////////////////////////////////
    if (encoderRight.changed())
    {
        if (encoderRight.left())
        {
            tm.Minute = (tm.Minute >= 1) ? (tm.Minute - 1) : (tm.Minute + 59);
        }
        else if (encoderRight.right())
        {
            tm.Minute = (tm.Minute <  59) ? (tm.Minute + 1) : (tm.Minute - 59);
        }
        else if (encoderRight.shortPress())
        {
            ret = CANCEL;
        }
        else if (encoderRight.longPress())
        {
            ret = CANCEL;
        }
        time = makeTime(tm);
        oled.drawTimeSetup(time, caption);
        encoderRight.reset();
    }
    return ret;
}


uint8_t active(const time_t &alarm, const uint8_t &enabled) {
    if (!enabled)
        return ACTIVE_SLEEP;

    if (now() >= (alarm - START_LIGHT) && now() < alarm)
    {
        return ACTIVE_PREWAKE;
    }
    else if (now() >= alarm && now() < (alarm + STOP_ALARM))
    {
        return ACTIVE_WAKEUP;
    }

    return ACTIVE_SLEEP;
}

void updateWakeUpLight()
{
    static uint16_t lastStep = 0;

    uint16_t diff = START_LIGHT - (min(alarm0, alarm1) - now());
    uint16_t step = pow(2, (8.229 * diff / START_LIGHT));

    if (step != lastStep && step <= 300)
    {
        uint8_t white = 0;
        uint8_t red   = 0;
        uint8_t blue  = 0;

        if (step < 255)
            white = step;
        else
            white = 255;

        if (step <= 100)
            red = step;
        else if (step > 100 && step <= 200)
            red = 200 - step;
        else if (step > 200 && step <= 300)
            blue = step - 200;
        else
            blue = 100;

        lastStep = step;

        analogWrite(LED_WHITE_PIN, white);
        analogWrite(LED_RED_PIN,   red);
        analogWrite(LED_BLUE_PIN,  blue);
    }
}

void resetWakeUpLight()
{
    analogWrite(LED_WHITE_PIN, 0);
    analogWrite(LED_RED_PIN,   0);
    analogWrite(LED_GREEN_PIN, 0);
    analogWrite(LED_BLUE_PIN,  0);
}

void initAlarm()
{
    EEPROM.get(EEPROM_ALARM0, alarm0);
    EEPROM.get(EEPROM_ALARM1, alarm1);
    EEPROM.get(EEPROM_ALARM0_ENABLED, alarm0enabled);
    EEPROM.get(EEPROM_ALARM1_ENABLED, alarm1enabled);

    // Set next alarm time
    if (elapsedSecsToday(now()) < elapsedSecsToday(alarm0))
        alarm0 = previousMidnight(now()) + elapsedSecsToday(alarm0);
    else
        alarm0 = nextMidnight(now()) + elapsedSecsToday(alarm0);

    if (elapsedSecsToday(now()) < elapsedSecsToday(alarm1))
        alarm1 = previousMidnight(now()) + elapsedSecsToday(alarm1);
    else
        alarm1 = nextMidnight(now()) + elapsedSecsToday(alarm1);
}

#endif // _DAYLIGHTGLOW_H_
