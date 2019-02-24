#include "EncoderControl.h"

/*
 * Rotate clockwise                = Right
 * Rotate counter-clockwise        = Left
 * Push short press                = ShortPress
 * Push long press                 = LongPress
 * Push + rotate clockwise         = Up
 * Push + rotate counter-clockwise = Down
 */

EncoderControl::EncoderControl(uint8_t CLK, uint8_t DT, uint8_t SW) :
  encoder(CLK, DT), button_pin(SW)
{

}

EncoderControl::~EncoderControl()
{

}

void EncoderControl::init()
{
  // Setup encoder button pin
  pinMode(button_pin, INPUT);
}

void EncoderControl::update(const uint32_t &cycleTime)
{
  uint32_t current_time = millis();

  if (current_time - last_time >= cycleTime) {
    last_time = current_time;

    // Get current encoder and button state
    int32_t encoder_current = encoder.read();
    bool button_current = digitalRead(button_pin);

    // Encoder rotated counter clockwise
    if (encoder_current - encoder_last >= 3) {
      if (button_current == PRESSED) {
        state_down = true;
        button_last = PRESSED;
        button_time = 0xFFFFFFFF;
      }
      else {
        state_left = true;
      }
      encoder_last = encoder_current;
    }
    // Encoder rotated clockwise
    else if (encoder_current - encoder_last <= -3) {
      if (button_current == PRESSED) {
        state_up = true;
        button_last = PRESSED;
        button_time = 0xFFFFFFFF;
      }
      else {
        state_right = true;
      }
      encoder_last = encoder_current;
    }
    // Encoder not rotated
    else {
      // Button pressed
      if (button_current == PRESSED && button_last == RELEASED) {
        button_time = current_time;
        state_undef = true;
      }
      // Keeping button pressed
      else if (button_current == PRESSED && button_last == PRESSED) {
          if (current_time > button_time && (current_time - button_time) >= BUTTON_LONG_PRESS) {
              state_long = true;
              button_time = 0xFFFFFFFF;
          }
      }
      // Button released
      else if (button_current == RELEASED && button_last == PRESSED) {
        // Was button pressed during rotation?
        if (button_time > current_time) {
          button_time = current_time;
        }
        else if ((current_time - button_time) >= BUTTON_LONG_PRESS) {
          state_long = true;
        }
        else if ((current_time - button_time) >= BUTTON_SHORT_PRESS) {
          state_short = true;
        }
        state_undef = false;
      }
      button_last = button_current;
    }

  }
}

int32_t EncoderControl::getPosition()
{
  return encoder.read();
}

void EncoderControl::setPosition(const int32_t &position)
{
  encoder.write(position);
}

bool EncoderControl::changed()
{
  return state_up || state_down || state_left || state_right || state_short || state_long || state_undef;
}

bool EncoderControl::up()
{
  return state_up;
}

bool EncoderControl::down()
{
  return state_down;
}

bool EncoderControl::left()
{
  return state_left;
}

bool EncoderControl::right()
{
  return state_right;
}

bool EncoderControl::shortPress()
{
  return state_short;
}

bool EncoderControl::longPress()
{
  return state_long;
}

void EncoderControl::reset()
{
  state_up    = false;
  state_down  = false;
  state_left  = false;
  state_right = false;
  state_short = false;
  state_long  = false;
}
