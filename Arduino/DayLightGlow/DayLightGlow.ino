#include <TimeLib.h>
#include <DS3232RTC.h>

#include "DayLightGlow.h"
#include "src/SensorLamp/SensorLamp.h"
#include "src/EncoderControl/EncoderControl.h"

SensorLamp sensorLamp(SENSOR_PIN, LED_PIN);
EncoderControl encoderLeft(ENCODER_LEFT_A_PIN, ENCODER_LEFT_B_PIN, ENCODER_LEFT_BUTTON_PIN);
EncoderControl encoderRight(ENCODER_RIGHT_A_PIN, ENCODER_RIGHT_B_PIN, ENCODER_RIGHT_BUTTON_PIN);


void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void setup ()  {
  Serial.begin(BAUD_RATE);

  // Init RTC
  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet) {
   Serial.println("Unable to sync with the RTC");
  }
  else {
   Serial.println("RTC has set the system time");
   digitalClockDisplay();
  }
  
  sensorLamp.init();
  encoderLeft.init();
  encoderRight.init();
}

void loop () {
  sensorLamp.update(CYCLE_TIME);
  encoderLeft.update(CYCLE_TIME);
  encoderRight.update(CYCLE_TIME);

  if (encoderLeft.changed()) {
    Serial.print("Encoder  Left : ");
    Serial.print(encoderLeft.up());
    Serial.print(encoderLeft.down());
    Serial.print(encoderLeft.left());
    Serial.print(encoderLeft.right());
    Serial.print(encoderLeft.shortPress());
    Serial.print(encoderLeft.longPress());
    Serial.println();
    encoderLeft.reset();
  }

  if (encoderRight.changed()) {
    Serial.print("Encoder Right : ");
    Serial.print(encoderRight.up());
    Serial.print(encoderRight.down());
    Serial.print(encoderRight.left());
    Serial.print(encoderRight.right());
    Serial.print(encoderRight.shortPress());
    Serial.print(encoderRight.longPress());
    Serial.println();
    encoderRight.reset();
  }

  delay(10);
}


