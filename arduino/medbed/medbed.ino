/*

pin layout
Stepper Motor:
  2 - F/R
  3 - PWM / AVI
  4 - button stop 1
  12 - button stop 2
  ALM ?(not sure if necessary but setting aside)

DC Bed Tilt:
  5 - FORWARD PWM
  6 - REVERSE PWM

DC Door Open:
  9 - FORWARD PWM
  10 - REVERSE PWM

Roomlight Relays:
  7 - Light 1
  8 - Light 2

LEDs:
  11 - Neopixel data

*/

// SCANNER
#define SCANNER_PIN_FR 2
#define SCANNER_PIN_AVI 3
long SCANNER_start_time = 0;
int SCANNER_scan_duration = 10000;  // one full sweep is really 20 seconds

bool SCANNER_forward = false;
bool SCANNER_reverse = false;
int SCANNER_speed = 50;
// scanner lights
#include <Adafruit_NeoPixel.h>
#define LEDS_PIN 11
#define NUM_LEDS 120
Adafruit_NeoPixel pixels(NUM_LEDS, LEDS_PIN, NEO_GRB + NEO_KHZ800);


// BED TILT
#define TILT_FWD_PIN 5
#define TILT_REV_PIN 6

// DOORS
#define DOORS_FWD_PIN 9
#define DOORS_REV_PIN 10

// ROOM LIGHTS
#define ROOMLIGHT_1_PIN 7
#define ROOMLIGHT_2_PIN 8


void setup() {

  // set up pins
  pinMode(SCANNER_PIN_FR, OUTPUT);
  pinMode(ROOMLIGHT_1_PIN, OUTPUT);
  pinMode(ROOMLIGHT_2_PIN, OUTPUT);

  // set home positions
  // move stepper until stop...

  Serial.begin(38400);

  // set up LEDs
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() {
  if (Serial.available() > 0) {
    // read the incoming serial data until a newline character
    String receivedString = Serial.readStringUntil('\n');
    Serial.print("received: ");
    Serial.println(receivedString);

    // find the position of the comma character
    int commaIndex = receivedString.indexOf(',');

    // extract the command string
    if (commaIndex != -1) {
      String command = receivedString.substring(0, commaIndex);
      // extract the integer value after the comma
      int value = receivedString.substring(commaIndex + 1).toInt();

      if (command == "DEBUG") {
        if (value == 9) {

          analogWrite(SCANNER_PIN_AVI, SCANNER_speed);
          digitalWrite(SCANNER_PIN_FR, HIGH);

        } else if (value == 11) {

          analogWrite(SCANNER_PIN_AVI, SCANNER_speed);
          digitalWrite(SCANNER_PIN_FR, LOW);
        }
      }

      if (command == "Scanner") {
        if (value == 0) {
          stopScan();
        } else if (value == 1) {
          startScan();
        }
      } else if (command == "Roomlights1") {
        if (value == 0) {
          digitalWrite(ROOMLIGHT_1_PIN, LOW);
        } else if (value == 1) {
          digitalWrite(ROOMLIGHT_1_PIN, HIGH);
        }
      } else if (command == "Roomlights2") {
        if (value == 0) {
          digitalWrite(ROOMLIGHT_2_PIN, LOW);
        } else if (value == 1) {
          digitalWrite(ROOMLIGHT_2_PIN, HIGH);
        }
      } else if (command == "LEDs") {
        handleLEDs(value);
      }

      // implement other actions for different commands here
    }
  }


  // check on scanner
  scanner();
}

void handleLEDs(int val) {
  switch (val) {
    case 0:
      setAllLeds(0, 0, 0, 0);
      break;
  }
}

void setAllLeds(int r, int g, int b, int brightness) {
  for (int i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(r, g, b);
  }
  pixels.setBrightness(brightness);
  pixels.show();
}

void startScan() {
  Serial.println("starting scan");
  SCANNER_forward = true;
  SCANNER_reverse = false;

  analogWrite(SCANNER_PIN_AVI, SCANNER_speed);
  digitalWrite(SCANNER_PIN_FR, HIGH);

  SCANNER_start_time = millis();
}

void stopScan() {
  Serial.println("stopping scan");
  analogWrite(SCANNER_PIN_AVI, 0);
}

bool scannerHitStart() {
  // check on scanner hitting the ends
  return false;
}

bool scannerHitEnd() {
  // check on scanner hitting the ends
  return false;
}

void scanner() {

  if (scannerHitStart()) {
    stopScan();
  }
  if (scannerHitEnd()) {
    stopScan();
  }

  long currentTime = millis();

  if (SCANNER_forward || SCANNER_reverse) {

    if (currentTime > SCANNER_start_time + SCANNER_scan_duration) {

      Serial.println("Scanner timer went off");

      if (SCANNER_forward) {
        SCANNER_forward = false;
        SCANNER_reverse = true;
        SCANNER_start_time = millis();

        // switch to reverse
        digitalWrite(SCANNER_PIN_FR, LOW);
        analogWrite(SCANNER_PIN_AVI, SCANNER_speed);

        Serial.println("scan has gone all the way forward, returning");
      } else if (SCANNER_reverse) {
        SCANNER_forward = false;
        SCANNER_reverse = false;
        // switch to forward (for later)
        digitalWrite(SCANNER_PIN_FR, HIGH);
        // stop it
        stopScan();
      }
    }
  }
}