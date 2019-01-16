#include "SensorLamp.h"

SensorLamp::SensorLamp(uint8_t sensorPin, uint8_t ledPin) : 
  sensor_pin(sensorPin), led_pin(ledPin) 
{
  filter = new MedianFilter(FILTERSIZE);
}

SensorLamp::~SensorLamp()
{
  delete filter;
}

void SensorLamp::init()
{
  pinMode(sensor_pin, INPUT);
  pinMode(led_pin,   OUTPUT);
  
  analogWrite(led_pin, 0);
  
  // Fill median filter
  for (uint8_t i = 0; i < filter->size(); i++){
    filter->add(analogRead(sensor_pin));
    delay(10);
  }

  // Get median
  int16_t adc_input = filter->get();

  // Convert analog value to distance in mm
  uint16_t distance = MedianFilter::multiMap(adc_input, in, out, 14);

  // Set max sense value
  if (distance > sense_max) {
    ;
  }
  else if (distance > (sense_min + track_distance + SENSE_MAX_OFFSET)) {
    sense_max = distance - SENSE_MAX_OFFSET;
  }
  else if (distance > (sense_min + SENSE_MAX_OFFSET)) {
    sense_max = distance - SENSE_MAX_OFFSET;
    track_distance = sense_max - sense_min;

    // Operation might be impeded
    analogWrite(led_pin, 255);
    delay(250);
    analogWrite(led_pin, 0);
  }
  else {
    // Operation not possible
    while (true) {
      pwm_output = pwm_output ? 0 : 255;
      analogWrite(led_pin, pwm_output);
      delay(250);
    }
  }
}

void SensorLamp::update(uint32_t cycleTime)
{
  uint32_t current_time = millis();
  
  if (current_time - last_time >= cycleTime) {
    last_time = current_time;
    
    // Debounce if neccessary
    if(debounce_cycles) {
      debounce_cycles -= 1;
      return;
    }
    
    if (blink_cycles) {
      analogWrite(led_pin, 0);
      blink_cycles -= 1;
      return;
    }
    else {
      analogWrite(led_pin, pwm_output);
    }
    
    // Read analog value and add to median filter
    filter->add(analogRead(sensor_pin));
    uint16_t adc_input = filter->get();

    // Convert analog value to distance in mm
    uint16_t distance = MedianFilter::multiMap(adc_input, in, out, 14);

    if (hand_tracking) {
      if(distance < sense_max) {
        if(distance < tracking_lower_limit) {
          hand_tracked_bright = MIN_BRIGHTNESS;

          // Shift lower border
          tracking_lower_limit = distance;
          tracking_upper_limit = constrain(distance + track_distance,sense_min, sense_max);

          // Blink to signal
          /*analogWrite(led_pin, 0);
          delay(25);
          analogWrite(led_pin, pwm_output);*/
          blink_cycles = 2;
        }
        else if (distance > tracking_upper_limit) {
          hand_tracked_bright = MAX_BRIGHTNESS;

          // Shift upper border
          tracking_lower_limit = constrain(distance - track_distance,sense_min, sense_max);
          tracking_upper_limit = distance;

          // Blink to signal
          /*analogWrite(led_pin, 0);
          delay(25);
          analogWrite(led_pin, pwm_output);*/
          blink_cycles = 2;
        }
        else {
          hand_tracked_bright = map(distance, tracking_lower_limit, tracking_upper_limit, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
        }

        if (abs(target_bright - hand_tracked_bright) > HAND_MINIMUM_CHANGE || !lamp_lighted) {
          target_bright = hand_tracked_bright;
          lamp_lighted = true;
        }

        hand_cycles = 0;
      }
      else {
        target_bright = pwm_output;
        stored_bright = pwm_output;
        hand_cycles += 1;
        if (hand_cycles == END_TRACKING_CYCLES) {
          // Stop hand tracking
          hand_tracking = false;
          hand_cycles = 0;

          // Blink to confirm
          /*analogWrite(led_pin, 0);
          delay(200);
          analogWrite(led_pin, pwm_output);*/
          blink_cycles = 20;
        }
      }
    }
    else {
      if (distance < sense_max) {
        hand_cycles += 1;
        if (hand_cycles == START_TRACKING_CYCLES) {
          // Set tracking range
          tracking_lower_limit = constrain(distance - map(stored_bright, 0, 255, 0, track_distance), sense_min, sense_max);
          tracking_upper_limit = constrain(distance + map(stored_bright, 0, 255, track_distance, 0), sense_min, sense_max);

          // Start hand tracking
          hand_tracking = true;
          hand_cycles = 0;

          // Blink to confirm
          /*analogWrite(led_pin, 0);
          delay(200);
          analogWrite(led_pin, pwm_output);*/
          blink_cycles = 20;
        }
      }
      else {
        if (hand_cycles) {
          lamp_lighted = !lamp_lighted;
          target_bright = lamp_lighted ? stored_bright : 0;
          debounce_cycles = DEBOUNCE_CYCLES;
          hand_cycles = 0;
        }
      }
    }

    // Adjust brightness
    if (pwm_output != target_bright) {
      if(pwm_output > target_bright && pwm_output > 0) --pwm_output;
      if(pwm_output < target_bright && pwm_output < 0xFF) ++pwm_output;
      analogWrite(led_pin, pwm_output);
    }
  }
}