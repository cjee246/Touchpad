// -------------------- HEADER --------------------

// Snowflake touchpad as gamepad
//
// NOTE: This sketch file is for use with Arduino Leonardo and
//       Arduino Micro only.
//
// Calvin Jee, 2019-04-14

// -------------------- INIT --------------------

// Include libraries
#include <Joystick.h>
#include <Touchpad_Snowflake.h>
#include <CapacitiveSensor.h>

// Construct joystick as gamepad
// -- 4 buttons, X and Y axes
const int BTN_COUNT = 8;
const uint8_t JS_MIN = 0;
const uint8_t JS_MAX = 255;
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                   BTN_COUNT, 0,           // Button Count, Hat Switch Count
                   true, true, false,      // X, Y, Z Axes
                   false, false, false,    // Rx, Ry, Rz
                   false, false,           // Rudder, throttle
                   false, false, false);   // Accelerator, brake, or steering

// Construct touchpad
Touchpad_Snowflake dpad = Touchpad_Snowflake();
const int MAP_DEFAULT = 0;
const int MAP_INNER = 1;

// Assign threshold and capcacitive sensor pins
// -- try with 100K resistor between reference and sensor pins
// -- CapacitiveSensor{receive pin, sensor pin}
const long CS_THRESH = 10;
const int CS_REC = 7;
const int CS_SAMPLE = 100;
const long CS_TIMEOUT = 10;

// For left stick USB on XBOX adaptive
CapacitiveSensor csArray[BTN_COUNT] = {
  CapacitiveSensor(CS_REC, 5),        // L.stick
  CapacitiveSensor(CS_REC, 6),        // R.stick
  CapacitiveSensor(CS_REC, 8),        // L.thumb
  CapacitiveSensor(CS_REC, 9),        // L.bumper
  CapacitiveSensor(CS_REC, 10),       // A
  CapacitiveSensor(CS_REC, 11),       // B
  CapacitiveSensor(CS_REC, 12),       // View
  CapacitiveSensor(CS_REC, 13),       // Menu
};

// -------------------- SETUP --------------------

void setup() {
  
  // Initialize Joystick Library
  Joystick.begin();
  Joystick.setXAxisRange(JS_MIN, JS_MAX);
  Joystick.setYAxisRange(JS_MIN, JS_MAX);

  // Initialize MPR121 sensors for touchpad
  // -- see MPR121 documentation for addresses
  dpad.initialize(MAP_INNER);

  // Set timeout of capacitive sensing
  for (int i = 0; i < BTN_COUNT; i++) {
    csArray[i].set_CS_Timeout_Millis(CS_TIMEOUT);
  }
}

// -------------------- PRE-LOOP --------------------

// starting position of dpad
uint8_t xPos = JS_MAX - (JS_MAX - JS_MIN) / 2;
uint8_t yPos = JS_MAX - (JS_MAX - JS_MIN) / 2;

// button array
long btn_read[BTN_COUNT];
int btn_curr[BTN_COUNT];
int btn_last[BTN_COUNT];

// -------------------- LOOP --------------------

void loop() {

  // Set touchpad position
  xPos = dpad.getPosition('x');
  yPos = dpad.getPosition('y');
  Joystick.setXAxis(xPos);
  Joystick.setYAxis(yPos);

  // Set button states
  // -- from buttons 2 to 6 (as defined above)
  for (int i = 2; i < BTN_COUNT - 2; i++) {
    btn_read[i] = csArray[i].capacitiveSensor(CS_SAMPLE);
    if (btn_read[i] > CS_THRESH) {
      btn_curr[i] = 1;
    }
    else {
      btn_curr[i] = 0;
    }
    if (btn_curr[i] != btn_last[i]) {
      Joystick.setButton(i, btn_curr[i]);
      btn_last[i] = btn_curr[i];
    }
  }

  // Delay buffer
  delay(20);

}
