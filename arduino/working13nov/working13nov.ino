#define SCANNER_PIN_FR 2
#define SCANNER_PIN_AVI 3
#define SCANNER_BRAKE_HOME 4
#define TILT_FWD_PIN 5
#define TILT_REV_PIN 6
#define ROOMLIGHT_1_PIN 7
#define ROOMLIGHT_2_PIN 8
#define DOORS_FWD_PIN 9
#define DOORS_REV_PIN 10
#define LEDS_PIN 11
#define SCANNER_BRAKE_AWAY 12

#define BAUD_RATE 115200

// SCANNER
long SCANNER_start_time = 0;
// duration for speed 80 = 7000
// duration for speed 150 = 5000
// duration for speed 200 = 4000
int SCANNER_scan_duration = 4000;  // one full sweep is really 20 seconds
bool SCANNER_forward = false;
bool SCANNER_reverse = false;
int SCANNER_speed = 90;

// LEDs SCANNER
#include <Adafruit_NeoPixel.h>
#define NUM_LEDS 120
Adafruit_NeoPixel pixels(NUM_LEDS, LEDS_PIN, NEO_GRB + NEO_KHZ800);


// BED TILT
int tiltSpeed = 255;
#define TILT_DURATION 10000  // how long it takes to go all the way

// DOORS
int doorSpeed = 255;

// ROOM LIGHTS



void setup() {

  Serial.begin(BAUD_RATE);
  //Serial.setTimeout(2);

  // set up pins
  pinMode(SCANNER_PIN_FR, OUTPUT);
  pinMode(SCANNER_BRAKE_HOME, INPUT_PULLUP);
  pinMode(ROOMLIGHT_1_PIN, OUTPUT);
  pinMode(ROOMLIGHT_2_PIN, OUTPUT);
  pinMode(TILT_FWD_PIN, OUTPUT);
  pinMode(TILT_REV_PIN, OUTPUT);

  // set up LEDs
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)

  // set home positions
  // scannerHome();
  // tiltHome();
}

void scannerHome() {

  Serial.println("moving scanner to home position");
  SCANNER_forward = true;
  SCANNER_reverse = false;

  analogWrite(SCANNER_PIN_AVI, SCANNER_speed);
  digitalWrite(SCANNER_PIN_FR, HIGH);

  bool home = false;
  while (!home) {
    if (scannerHitStart()) {
      stopScan();
      home = true;
    }
    delay(2);
  }
  Serial.println("scanner got to home, stopping home sequence");
}

void tiltHome() {

  // I think the theory here is to tilt it to one direction for one minute (to be sure it's all the way),
  // then to tilt it back half way (we have to base this on the time it takes since there is no sensor)

  Serial.println("moving tilt to home position");

  // tilt forward
  handleTilt(1);

  delay(TILT_DURATION);

  // tild backward
  handleTilt(2);
  delay(TILT_DURATION / 2);

  Serial.println("tilt motor got to home, stopping home sequence");
}


void loop() {
  // brake test
  int brk = digitalRead(SCANNER_BRAKE_HOME);
  if (brk == LOW) {
    Serial.println("braking");
  }

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

        } else if (value == 0) {

          stopScan();
        }
      }

      if (command == "Scanner") {
        handleScanner(value);
      } else if (command == "Roomlights1") {
        handleRoomLights(1, value);
      } else if (command == "Roomlights2") {
        handleRoomLights(2, value);
      } else if (command == "LEDs") {
        handleLEDs(value);
      } else if (command == "Tilt") {
        handleTilt(value);
      } else if (command == "Doors") {
        handleDoors(value);
      }

      // implement other actions for different commands here
    }
  }


  // check on scanner
  scanner();

  delay(1);
}

void handleLEDs(int value) {
  // 0 = off
  // 1 = blue
  // 2 = red
  switch (value) {
    case 0:
      setAllLeds(0, 0, 0, 0);
      break;
    case 1:
      setAllLeds(0, 0, 255, 255);
      break;
    case 2:
      setAllLeds(255, 0, 0, 255);
      break;
    case 3:
      setAllLeds(255, 255, 255, 255);
      break;
  }
}

void handleTilt(int value) {
  switch (value) {
    case 0:
      analogWrite(TILT_FWD_PIN, 0);
      analogWrite(TILT_REV_PIN, 0);
      break;

    case 1:  // Forward rotation
      analogWrite(TILT_FWD_PIN, tiltSpeed);
      analogWrite(TILT_REV_PIN, 0);

      break;

    case 2:
      analogWrite(TILT_FWD_PIN, 0);
      analogWrite(TILT_REV_PIN, tiltSpeed);

      break;
  }
}

void handleDoors(int value) {
  switch (value) {
    case 0:
      analogWrite(DOORS_FWD_PIN, 0);
      analogWrite(DOORS_REV_PIN, 0);
      break;

    case 1:  // Forward rotation
      analogWrite(DOORS_FWD_PIN, doorSpeed);
      analogWrite(DOORS_REV_PIN, 0);

      break;

    case 2:
      analogWrite(DOORS_FWD_PIN, 0);
      analogWrite(DOORS_REV_PIN, doorSpeed);

      break;
  }
}

void handleScanner(int value) {
  if (value == 0) {
    stopScan();
  } else if (value == 1) {
    startScan();
  }
}

void handleRoomLights(int lights, int value) {

  int pin = (lights == 1) ? ROOMLIGHT_1_PIN : ROOMLIGHT_2_PIN;

  if (value == 0) {
    digitalWrite(pin, LOW);
  } else if (value == 1) {
    digitalWrite(pin, HIGH);
  }
}

void setAllLeds(int r, int g, int b, int brightness) {
  for (int i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
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
  digitalWrite(SCANNER_PIN_FR, LOW);
}

bool scannerHitStart() {
  // check on scanner hitting the ends
  if (digitalRead(SCANNER_BRAKE_HOME) == LOW) {
    return true;
  } else {
    return false;
  }
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