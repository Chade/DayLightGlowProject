#include <TimeLib.h>
#include <DS3232RTC.h>
#include <EEPROM.h>

#include "FMRM1.h"

#include "DayLightGlow.h"


// ================================================================================
// Setup
// ================================================================================

void setup()
{
    ////////////////////////////////////////
    // Start communication
    ////////////////////////////////////////

    Serial.begin(BAUD_RATE);
    Wire.begin();

    ////////////////////////////////////////
    // Init pins
    ////////////////////////////////////////

    Serial.print(F("Init pind..."));
    pinMode(4, INPUT);
    pinMode(LED_RED_PIN,   OUTPUT);
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_BLUE_PIN,  OUTPUT);
    pinMode(LED_WHITE_PIN, OUTPUT);
    Serial.println(F("Done"));

    ////////////////////////////////////////
    // Init radio
    ////////////////////////////////////////

    Serial.print(F("Init radio..."));
    EEPROM.get(EEPROM_VOLUME,    volume);
    EEPROM.get(EEPROM_FREQUENCY, frequency);

    Radio.FMRM_init();
    Radio.FMRM_config();
    Radio.FMRM_rds_config();
    Radio.FMRM_frequency_set(frequency);
    Radio.FMRM_volume_set(volume);
    Radio.FMRM_audio_mute(mute);
    Serial.println(F("Done"));

    ////////////////////////////////////////
    // Init RTC
    ////////////////////////////////////////

    Serial.println(F("Init rtc..."));
    setSyncProvider(RTC.get);
    if (timeStatus() != timeSet)
    {
        Serial.println(F("Unable to sync with the RTC"));
    }
    else
    {
        Serial.println(F("RTC has set the system time"));
        initAlarm();
    }
    Serial.println(F("Done"));

    ////////////////////////////////////////
    // Init sensor lamp
    ////////////////////////////////////////

    Serial.print(F("Init lamp..."));
    sensorLamp.init();
    Serial.println(F("Done"));

    ////////////////////////////////////////
    // Init encoders
    ////////////////////////////////////////

    Serial.print(F("Init encoder..."));
    encoderLeft.init();
    encoderRight.init();
    Serial.println(F("Done"));

    ////////////////////////////////////////
    // Init display
    ////////////////////////////////////////

    Serial.print(F("Init display..."));
    oled.init();
    oled.setTime(now());
    oled.setAlarm(alarm0, 0, alarm0enabled);
    oled.setAlarm(alarm1, 1, alarm1enabled);
    oled.setVolume(Radio.FMRM_volume_get(), mute);
    oled.setFrequency(frequency);
    oled.forceUpdate();
    Serial.println(F("Done"));
}


// ================================================================================
// Loop
// ================================================================================

void loop()
{
    ////////////////////////////////////////
    // Update
    ////////////////////////////////////////

    encoderLeft.update(5);
    encoderRight.update(5);

    ////////////////////////////////////////
    // Handle screen modes
    ////////////////////////////////////////

    switch (screenMode) {
    case SCREEN_HOME:
        {
            screenMode = drawHomeScreen();
        }
        break;

    case SCREEN_ALARM0:
        {
            int8_t ret = drawSetupScreen(dateTime, "Set Alarm 1");
            if (ret >= OK)
            {
                screenMode = SCREEN_HOME;

                if(elapsedSecsToday(now()) < elapsedSecsToday(dateTime))
                    alarm0 = previousMidnight(now()) + elapsedSecsToday(dateTime);
                else
                    alarm0 = nextMidnight(now()) + elapsedSecsToday(dateTime);

                if (ret == TOGGLE)
                    alarm0enabled = false;

                EEPROM.put(EEPROM_VOLUME, volume);
                EEPROM.put(EEPROM_FREQUENCY, frequency);
                EEPROM.put(EEPROM_ALARM0, elapsedSecsToday(alarm0));
                EEPROM.put(EEPROM_ALARM0_ENABLED, alarm0enabled);
                oled.setAlarm(alarm0, 0, alarm0enabled);
                oled.forceUpdate();
            }
            else if (ret == CANCEL)
            {
                screenMode = SCREEN_HOME;
                oled.forceUpdate();
            }
        }
        break;

    case SCREEN_ALARM1:
        {
            int8_t ret = drawSetupScreen(dateTime, "Set Alarm 2");
            if (ret >= OK)
            {
                screenMode = SCREEN_HOME;

                if(elapsedSecsToday(now()) < elapsedSecsToday(dateTime))
                    alarm1 = previousMidnight(now()) + elapsedSecsToday(dateTime);
                else
                    alarm1 = nextMidnight(now()) + elapsedSecsToday(dateTime);

                if (ret == TOGGLE)
                    alarm1enabled = false;

                EEPROM.put(EEPROM_VOLUME, volume);
                EEPROM.put(EEPROM_FREQUENCY, frequency);
                EEPROM.put(EEPROM_ALARM1, elapsedSecsToday(alarm1));
                EEPROM.put(EEPROM_ALARM1_ENABLED, alarm1enabled);
                oled.setAlarm(alarm1, 1, alarm1enabled);
                oled.forceUpdate();
            }
            else if (ret == CANCEL)
            {
                screenMode = SCREEN_HOME;
                oled.forceUpdate();
            }
        }
        break;

    case SCREEN_TIME:
        {
            int8_t ret = drawSetupScreen(dateTime, "Set Time");
            if (ret == OK)
            {
                screenMode = SCREEN_HOME;
                RTC.set(dateTime);
                setTime(dateTime);
                oled.setTime(dateTime);
                oled.forceUpdate();
            }
            else if (ret == CANCEL)
            {
                screenMode = SCREEN_HOME;
                oled.forceUpdate();
            }
        }
        break;
    }

    ////////////////////////////////////////
    // Handle alarm modes
    ////////////////////////////////////////

    switch (sleepMode)
    {
    case ACTIVE_SLEEP:
        {
            sleepMode = (active(alarm0, alarm0enabled)  | active(alarm1, alarm1enabled));
            sensorLamp.update(10);
        }
        break;

    case ACTIVE_PREWAKE:
        {
            static uint8_t detected = 0;

            sleepMode = (active(alarm0, alarm0enabled)  | active(alarm1, alarm1enabled));

            if (sensorLamp.handDetected(10) == 1)
            {
                detected = (detected > 0) ? 0 : 1;
            }

            if (!detected || sleepMode == ACTIVE_WAKEUP)
            {
                // Update wake-up light
                updateWakeUpLight();
            }
            else
            {
                // Disable light, but keep alarm on
                resetWakeUpLight();
            }
        }
        break;

    case ACTIVE_WAKEUP:
        {
            uint8_t detected = sensorLamp.handDetected(10);

            if (mute == 1 && detected == 0)
            {
                // Unmute
                mute = 0;
                Radio.FMRM_audio_mute(mute);
                oled.setVolume(volume, mute);
                oled.forceUpdate();
            }
            else if (mute == 0 && detected == 1)
            {
                // Snooze
                if (active(alarm0, alarm0enabled) == ACTIVE_WAKEUP)
                {
                    alarm0 = (now() + SNOOZE);
                    oled.setAlarm(alarm0, 0, alarm0enabled);
                }

                if (active(alarm1, alarm1enabled) == ACTIVE_WAKEUP)
                {
                    alarm1 = (now() + SNOOZE);
                    oled.setAlarm(alarm1, 1, alarm1enabled);
                }

                // Mute
                mute = 1;
                Radio.FMRM_audio_mute(mute);
                oled.setVolume(volume, mute);
                oled.forceUpdate();

                // Change mode
                sleepMode = ACTIVE_SNOOZE;
            }
            else if (detected == 2 || (active(alarm0, alarm0enabled)  | active(alarm1, alarm1enabled)) == ACTIVE_SLEEP)
            {
                // Reset alarm for tomorrow
                initAlarm();
                oled.setAlarm(alarm0, 0, alarm0enabled);
                oled.setAlarm(alarm1, 1, alarm1enabled);

                // Mute
                mute = 1;
                Radio.FMRM_audio_mute(mute);
                oled.setVolume(volume, mute);
                oled.forceUpdate();

                // Disable light
                resetWakeUpLight();

                // Change mode
                sleepMode = ACTIVE_SLEEP;
            }

        }
        break;


    case ACTIVE_SNOOZE:
        {

            uint8_t detected = sensorLamp.handDetected(10);

            if (detected == 2)
            {
                // Reset alarm for tomorrow
                initAlarm();
                oled.setAlarm(alarm0, 0, alarm0enabled);
                oled.setAlarm(alarm1, 1, alarm1enabled);

                // Mute
                mute = 1;
                Radio.FMRM_audio_mute(mute);
                oled.setVolume(volume, mute);
                oled.forceUpdate();

                // Disable light
                resetWakeUpLight();

                // Change mode
                sleepMode = ACTIVE_SLEEP;
            }

            if ((active(alarm0, alarm0enabled)  | active(alarm1, alarm1enabled)) ==  ACTIVE_WAKEUP)
            {
                // Change mode
                sleepMode = ACTIVE_WAKEUP;
            }
        }
        break;
    }
}

