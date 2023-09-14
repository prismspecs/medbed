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
int SCANNER_PIN_FR = 2;
int SCANNER_PIN_AVI = 3;
long SCANNER_start_time = 0;
int SCANNER_scan_duration = 5000;

bool SCANNER_forward = false;
bool SCANNER_reverse = false;
int SCANNER_speed = 50;

void setup() {
  // set home positions
  // move stepper until stop...

  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    // read the incoming serial data until a newline character
    String receivedString = Serial.readStringUntil('\n');

    // find the position of the comma character
    int commaIndex = receivedString.indexOf(',');

    // extract the command string
    if (commaIndex != -1) {
      String command = receivedString.substring(0, commaIndex);
      // extract the integer value after the comma
      int value = receivedString.substring(commaIndex + 1).toInt();

      if (command == "Scanner") {
        if (value == 0) {
          stopScan();
        } else if (value == 1) {
          startScan();
        }
      } else if (command == "Lights") {
        // digitalWrite(ledPin, HIGH);
        // delay(value);
        // digitalWrite(ledPin, LOW);
        // delay(value);
      }

      // implement other actions for different commands here
    }
  }


  // check on scanner
  scanner();
}

void startScan() {
  SCANNER_forward = true;
  SCANNER_reverse = false;

  SCANNER_start_time = millis();
}

void stopScan() {
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

  if (currentTime > SCANNER_start_time + SCANNER_scan_duration) {

    if (SCANNER_forward) {
      SCANNER_forward = false;
      SCANNER_reverse = true;
    } else if (SCANNER_reverse) {
      SCANNER_forward = false;
      SCANNER_reverse = false;
      // stop it
      stopScan();
    }
  }
}