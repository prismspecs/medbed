// Define the number of pins
const int numPins = 12;

// Define an array to store pin numbers
int pins[numPins] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

void setup() {
  // Set up all pins from 2 to 13 as INPUT_PULLUP
  for (int i = 0; i < numPins; i++) {
    pinMode(pins[i], INPUT_PULLUP);
  }

  Serial.begin(9600);
}

void loop() {
  // Check the state of each pin
  for (int i = 0; i < numPins; i++) {
    int readPin = digitalRead(pins[i]);
    if (readPin == LOW) {
      Serial.print("Pin ");
      Serial.print(pins[i]);
      Serial.println(" is LOW.");
    }
  }

  delay(10);
}
