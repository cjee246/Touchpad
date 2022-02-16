// ---------- HEADER ----------

// Snowflake touchpad as joystick
//
// NOTE: This sketch file is for use with Arduino Leonardo and
//       Arduino Micro only.
//
// Calvin Jee, 2019-04-14

// ---------- INIT ----------

// Include libraries
#include <Joystick.h>
#include <Touchpad_Snowflake.h>

// Construct touchpad
Touchpad_Snowflake dpad = Touchpad_Snowflake();

// ---------- SETUP ----------

void setup() {

  // Initialize Joystick Library
  Joystick.begin(false);    // must manually send state
  Joystick.setXAxisRange(0, 255);
  Joystick.setYAxisRange(0, 255);

  // Initialize MPR121 sensors for touchpad
  dpad.initialize(0x5A, 0x5B);

}

// ---------- PRE-LOOP ----------

// starting position of dpad
uint8_t xPos = 127;
uint8_t yPos = 127;

// ---------- LOOP ----------

void loop() {

  // Get touchpad position
  xPos = dpad.getPosition('x');
  yPos = dpad.getPosition('y');

  // Set Joystick axes
  Joystick.setXAxis(xPos);
  Joystick.setYAxis(yPos);

  // Send Joystick state
  Joystick.sendState();
  delay(20);
}
