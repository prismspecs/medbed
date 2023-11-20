#define SCANNER_PIN_FR 2
#define SCANNER_PIN_AVI 3
#define SCANNER_BRAKE_HEAD 4
#define TILT_REV_PIN 5
#define TILT_FWD_PIN 6
#define ROOMLIGHT_1_PIN 7
#define ROOMLIGHT_2_PIN 8
#define DOORS_REV_PIN 9
#define DOORS_FWD_PIN 10
#define LEDS_PIN 11
#define SCANNER_BRAKE_FEET 12

#define BAUD_RATE 115200
// scanner takes 11.5 seconds feet to head
// doors take 62 seconds open to close
// doors take 45 seconds close to open
// tilt takes 53 seconds from all the way forward to all the way back
// tilt takes 58 seconds from all the way back to all the way forward

// SCANNER
const int SCANNER_speed = 90; // 90 is good
const int SCANNER_bounceback_duration = 300;
unsigned long SCANNER_start_time = 0;
#define SCANNER_SCAN_DURATION 11500; // one full sweep is really 20 seconds
bool SCANNER_move_to_head = false;
bool SCANNER_move_to_feet = false;
bool SCANNER_freakout = false;
unsigned long SCANNER_freakout_start_time = 0;
int SCANNER_freakout_step_duration = 2000;
const int SCANNER_freakout_led_max_duration = 4000;
int SCANNER_freakout_step = 0;
int SCANNER_freakout_num_steps = 8;
int SCANNER_freakout_brightness = 255;
int SCANNER_freakout_brightness_direction = 1;
bool SCANNER_override = false;   // when scanner hits a pre-stop, send it back to the middle before doing anything else with it
int SO_direction = 0;            // scanner override direction (1 to head, 2 to feet)
unsigned long SO_start_time = 0; // when it started to override
const int SO_duration = 2000;    // override for X seconds

// LEDs SCANNER
#include <Adafruit_NeoPixel.h>
#define NUM_LEDS 120
Adafruit_NeoPixel pixels(NUM_LEDS, LEDS_PIN, NEO_GRB + NEO_KHZ800);
bool LED_cycle = false;                 // smooth color cycle mode
unsigned long LED_cycle_start_time = 0; // ...
const int LED_cycle_duration = 4000;
const int LED_num_color_phases = 4;
const int LED_phase_duration = LED_cycle_duration / LED_num_color_phases;
int LED_current_color_phase = 0;
float LED_prevColorPhaseProgress = 0;
uint8_t LED_red = 0;
uint8_t LED_green = 0;
uint8_t LED_blue = 0;

// BED TILT
int tiltSpeed = 255;
#define TILT_BACK_DURATION 53000 // how long it takes to go all the way back from all the way forward

// DOORS
int doorSpeed = 255;
#define DOORS_OPEN_DURATION 45000  // how long it takes closed to open
#define DOORS_CLOSE_DURATION 62000 // how long it takes closed to open

// ROOM LIGHTS

void setup()
{

  Serial.begin(BAUD_RATE);
  Serial.setTimeout(10);

  // set up pins
  pinMode(SCANNER_PIN_FR, OUTPUT);
  pinMode(SCANNER_BRAKE_HEAD, INPUT_PULLUP);
  pinMode(SCANNER_BRAKE_FEET, INPUT_PULLUP);
  pinMode(ROOMLIGHT_1_PIN, OUTPUT);
  pinMode(ROOMLIGHT_2_PIN, OUTPUT);
  pinMode(TILT_FWD_PIN, OUTPUT);
  pinMode(TILT_REV_PIN, OUTPUT);

  // set up LEDs
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  // set home positions
  // homeSequence();
}

void loop()
{
  // check on scanner
  scanner();

  LEDs();

  if (Serial.available() > 0)
  {
    // read the incoming serial data until a newline character

    String receivedString = Serial.readStringUntil('\n');
    Serial.print("received: ");
    Serial.println(receivedString);

    // find the position of the comma character
    int commaIndex = receivedString.indexOf(',');

    // extract the command string
    if (commaIndex != -1)
    {
      String command = receivedString.substring(0, commaIndex);
      // extract the integer value after the comma
      int value = receivedString.substring(commaIndex + 1).toInt();

      if (command == "DEBUG")
      {
        if (value == 9)
        {

          analogWrite(SCANNER_PIN_AVI, SCANNER_speed);
          digitalWrite(SCANNER_PIN_FR, HIGH);
        }
        else if (value == 11)
        {

          analogWrite(SCANNER_PIN_AVI, SCANNER_speed);
          digitalWrite(SCANNER_PIN_FR, LOW);
        }
        else if (value == 0)
        {

          stopScan();
        }
      }

      if (command == "Scanner")
      {
        handleScanner(value);
      }
      else if (command == "Roomlights1")
      {
        handleRoomLights(1, value);
      }
      else if (command == "Roomlights2")
      {
        handleRoomLights(2, value);
      }
      else if (command == "LEDs")
      {
        handleLEDs(value);
      }
      else if (command == "Tilt")
      {
        handleTilt(value);
      }
      else if (command == "Doors")
      {
        handleDoors(value);
      }
      else if (command == "Home")
      {
        homeSequence();
      }

      // implement other actions for different commands here
    }
  }

  delay(2);
}

void homeSequence()
{
  scannerToFeetStart();
  tiltHome();
  doorsHome();
  Serial.println("turning LEDs off during home sequence");
  setAllLeds(0, 0, 0, 0);
  Serial.println("home sequence complete");
}

void scannerToFeetStart()
{

  Serial.println("starting to move scanner all the way to feet start position");
  scannerTowardsFeet();

  bool home = false;
  while (!home)
  {
    if (scannerHitFeet())
    {
      stopScan();
      home = true;
      SCANNER_move_to_feet = false;
    }
    delay(2);
  }

  // now move slightly towards head
  Serial.println("scanner hit feet, moving slightly towards head");
  scannerTowardsHead();
  delay(SCANNER_bounceback_duration);
  stopScan();
  SCANNER_move_to_head = false;
  Serial.println("scanner homing sequence complete");
}

void scannerTowardsFeet()
{
  Serial.println("moving scanner towards feet");
  SCANNER_move_to_head = false;
  SCANNER_move_to_feet = true;
  analogWrite(SCANNER_PIN_AVI, SCANNER_speed);
  digitalWrite(SCANNER_PIN_FR, LOW);
}

void scannerTowardsHead()
{
  Serial.println("moving scanner towards head");
  SCANNER_move_to_head = true;
  SCANNER_move_to_feet = false;
  analogWrite(SCANNER_PIN_AVI, SCANNER_speed);
  digitalWrite(SCANNER_PIN_FR, HIGH);
}

void startScannerFreakout()
{
  SCANNER_freakout_start_time = millis();
  SCANNER_freakout = true;
  SCANNER_freakout_step = 1;
}

void tiltHome()
{

  // I think the theory here is to tilt it to one direction for one minute (to be sure it's all the way),
  // then to tilt it back half way (we have to base this on the time it takes since there is no sensor)

  Serial.println("moving tilt to home position");

  handleTilt(2);
  delay(TILT_BACK_DURATION);
  handleTilt(1);
  delay(1000);
  handleTilt(0);

  Serial.println("tilt motor got to home");
}

void doorsHome()
{
  Serial.println("moving doors to home position (closed)");

  handleDoors(1); // close

  delay(DOORS_CLOSE_DURATION);

  Serial.println("doors motor got to home");
}

void handleLEDs(int value)
{
  // 0: off
  // 1: blue
  // 2: purple
  // 3: white
  // 4: cycle
  switch (value)
  {
  case 0:
    setAllLeds(0, 0, 0, 0);
    break;
  case 1:
    setAllLeds(0, 0, 255, 255);
    break;
  case 2:
    setAllLeds(128, 0, 128, 255);
    break;
  case 3:
    setAllLeds(255, 255, 255, 255);
    break;
  case 4:
    LEDstartCycle();
    break;
  }
}

void handleTilt(int value)
{
  // 0: stop
  // 1: tilt forward
  // 2: tilt backward
  // 3: tilt center
  switch (value)
  {
  case 0:
    analogWrite(TILT_FWD_PIN, 0);
    analogWrite(TILT_REV_PIN, 0);
    break;

  case 1: // Forward rotation
    analogWrite(TILT_FWD_PIN, tiltSpeed);
    analogWrite(TILT_REV_PIN, 0);
    break;

  case 2:
    analogWrite(TILT_FWD_PIN, 0);
    analogWrite(TILT_REV_PIN, tiltSpeed);
    break;

  case 3:
    // tilt back to center...
    break;
  }
}

void handleDoors(int value)
{
  // 0: stop
  // 1: close
  // 2: open

  switch (value)
  {
  case 0:
    analogWrite(DOORS_FWD_PIN, 0);
    analogWrite(DOORS_REV_PIN, 0);
    break;

  case 1: // doors close
    analogWrite(DOORS_FWD_PIN, doorSpeed);
    analogWrite(DOORS_REV_PIN, 0);
    break;

  case 2: // doors open
    analogWrite(DOORS_FWD_PIN, 0);
    analogWrite(DOORS_REV_PIN, doorSpeed);
    break;
  }
}

void handleScanner(int value)
{
  // 0: stop
  // 1: towards feet
  // 2: towards head
  // 3: fullscan feet to head
  // 4: freakout
  if (value == 0)
  {
    stopScan();
  }
  else if (value == 1)
  {
    startFullScan();
  }
  else if (value == 2)
  {
    startScannerFreakout();
  }
}

// room lights turn on and off via relay
void handleRoomLights(int lights, int value)
{

  int pin = (lights == 1) ? ROOMLIGHT_1_PIN : ROOMLIGHT_2_PIN;

  if (value == 0)
  {
    digitalWrite(pin, LOW);
  }
  else if (value == 1)
  {
    digitalWrite(pin, HIGH);
  }
}

// helper to turn all LEDs a certain color
void setAllLeds(int r, int g, int b, int brightness)
{
  for (int i = 0; i < pixels.numPixels(); i++)
  {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.setBrightness(brightness);
  pixels.show();
}

// smooth color cycle for LEDs
void LEDstartCycle()
{
  LED_cycle = true;
  LED_cycle_start_time = millis();
}

void LEDs()
{
  if (LED_cycle)
  {

    // COLOR PHASING
    unsigned long currentMillis = millis();
    unsigned long elapsedMillis = currentMillis - LED_cycle_start_time;

    // calculate the progress within the current phase (0 to 1)
    float colorPhaseProgress = static_cast<float>(elapsedMillis % LED_phase_duration) / LED_phase_duration;

    if (LED_prevColorPhaseProgress > colorPhaseProgress)
    {
      LED_current_color_phase++;

      if (LED_current_color_phase > LED_num_color_phases)
      {
        LED_current_color_phase = 0;
      }
    }

    LED_prevColorPhaseProgress = colorPhaseProgress;

    // interpolate between colors based on the current phase
    LED_green = 0;
    switch (LED_current_color_phase)
    {
    case 0:
      LED_red = interpolateColor(50, 128, colorPhaseProgress);
      LED_blue = interpolateColor(128, 128, colorPhaseProgress);
      break;
    case 1:
      LED_red = interpolateColor(128, 128, colorPhaseProgress);
      LED_blue = interpolateColor(128, 50, colorPhaseProgress);
      break;
    case 2:
      LED_red = interpolateColor(128, 128, colorPhaseProgress);
      LED_blue = interpolateColor(50, 128, colorPhaseProgress);
      break;
    case 3:
      LED_red = interpolateColor(128, 50, colorPhaseProgress);
      LED_blue = interpolateColor(128, 128, colorPhaseProgress);
      break;
    }

    if (SCANNER_freakout_brightness_direction == 1)
    {
      SCANNER_freakout_brightness -= 1;
      if (SCANNER_freakout_brightness <= 50)
      {
        SCANNER_freakout_brightness = 50;
        SCANNER_freakout_brightness_direction = -1;
      }
    }
    else
    {
      SCANNER_freakout_brightness += 1;
      if (SCANNER_freakout_brightness >= 255)
      {
        SCANNER_freakout_brightness = 255;
        SCANNER_freakout_brightness_direction = 1;
      }
    }

    setAllLeds(LED_red, LED_green, LED_blue, SCANNER_freakout_brightness); // full brightness
  }
}

// helper for color transitions
uint8_t interpolateColor(uint8_t start, uint8_t end, float progress)
{
  // interpolate between start and end based on progress
  return static_cast<uint8_t>(start + (end - start) * progress);
}

// start going from feet to head and back
void startFullScan()
{
  Serial.println("starting full scan");
  SCANNER_move_to_head = true;
  SCANNER_move_to_feet = false;

  analogWrite(SCANNER_PIN_AVI, SCANNER_speed);
  digitalWrite(SCANNER_PIN_FR, HIGH);

  SCANNER_start_time = millis();
}

// stop scanner motor
void stopScan()
{
  Serial.println("stopping scan");
  analogWrite(SCANNER_PIN_AVI, 0);
  digitalWrite(SCANNER_PIN_FR, LOW);
}

// check if scanner hit head pre-stop
bool scannerHitHead()
{
  // check on scanner hitting the ends
  if (digitalRead(SCANNER_BRAKE_HEAD) == LOW)
  {
    Serial.println("scannerHitHead returned true");
    return true;
  }
  else
  {
    return false;
  }
}

// check if scanner hit feet pre-stop
bool scannerHitFeet()
{
  // check on scanner hitting the ends
  if (digitalRead(SCANNER_BRAKE_FEET) == LOW)
  {
    Serial.println("scannerHitFeet returned true");
    return true;
  }
  else
  {
    return false;
  }
}

void scanner()
{

  // if (scannerHitFeet())
  // {
  //   SCANNER_override = true;
  //   SO_direction = 1; // move towards head position
  //   Serial.println("scanner hit feet, moving towards head slightly");
  // }

  // if (scannerHitHead())
  // {
  //   SCANNER_override = true;
  //   SO_direction = 2; // move towards head position
  //   Serial.println("scanner hit head, moving towards feet slightly");
  // }

  // scanner brakes -- should avoid this by tuning the editing sequence... this is mostly a fallback
  if (scannerHitFeet())
  {
    stopScan();
    scannerTowardsHead();
    delay(SCANNER_bounceback_duration);
  }
  if (scannerHitHead())
  {
    stopScan();
    scannerTowardsFeet();
    delay(SCANNER_bounceback_duration);
  }

  const unsigned long currentTime = millis();

  if (SCANNER_freakout)
  {
    if (currentTime > SCANNER_freakout_start_time + SCANNER_freakout_step_duration)
    {

      SCANNER_freakout_start_time = millis();
      SCANNER_freakout_step++;

      // even steps have half duration
      if (SCANNER_freakout_step % 2 == 0)
      {
        SCANNER_freakout_step_duration = SCANNER_freakout_led_max_duration / 2;
        scannerTowardsHead();
      }
      else
      {
        SCANNER_freakout_step_duration = SCANNER_freakout_led_max_duration;
        scannerTowardsFeet();
      }

      if (SCANNER_freakout_step >= SCANNER_freakout_num_steps)
      {
        SCANNER_freakout = false;
      }
    }
  }

  // if (SCANNER_move_to_head || SCANNER_move_to_feet)
  // {

  //   if (currentTime > SCANNER_start_time + SCANNER_SCAN_DURATION)
  //   {

  //     Serial.println("Scanner timer went off");

  //     if (SCANNER_move_to_head)
  //     {
  //       SCANNER_move_to_head = false;
  //       SCANNER_move_to_feet = true;
  //       SCANNER_start_time = millis();

  //       // switch to reverse
  //       digitalWrite(SCANNER_PIN_FR, LOW);
  //       analogWrite(SCANNER_PIN_AVI, SCANNER_speed);

  //       Serial.println("scan has gone all the way forward, returning");
  //     }
  //     else if (SCANNER_move_to_feet)
  //     {
  //       SCANNER_move_to_head = false;
  //       SCANNER_move_to_feet = false;
  //       // switch to forward (for later)
  //       digitalWrite(SCANNER_PIN_FR, HIGH);
  //       // stop it
  //       stopScan();
  //     }
  //   }
  // }
}