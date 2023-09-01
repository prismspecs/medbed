const int ledPin = 13; // Pin number where the LED is connected

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); // Set the baud rate to match the serial communication
}

void loop() {
  if (Serial.available() > 0) {
    String receivedString = Serial.readStringUntil('\n'); // Read the incoming serial data until a newline character
    
    int commaIndex = receivedString.indexOf(','); // Find the position of the comma character
    
    if (commaIndex != -1) {
      String command = receivedString.substring(0, commaIndex); // Extract the command string
      int value = receivedString.substring(commaIndex + 1).toInt(); // Extract the integer value after the comma
      
      if (command == "Lights") {
        digitalWrite(ledPin, HIGH);
        delay(value);
        digitalWrite(ledPin, LOW);
        delay(value);
      }
      
      // Implement other actions for different commands here
    }
  }
}
