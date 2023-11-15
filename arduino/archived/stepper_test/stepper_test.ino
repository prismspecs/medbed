/*

W = blue
V = green
U = yellow
(same for hv,hw,hu)

REF+ is red from hall sensor
hu = yellow
hv = green
hw = blue
REF- is black from hall sensor

ENBL to ground
BRK to 5v
AVI to PWM (pin 6) on Arduino
GND shared ground w Arduino  

the BRK (brake) is a momentary brake. at 5v (HIGH) it is off. at GND (LOW) it is on, and stops it immediately.

I should also wire the alarm pin


motor URL: https://www.act-motor.com/brushless-dc-motor-57blf-product/
controller URL: https://www.act-motor.com/brushless-dc-motor-driver-bldc-8015a-product/
*/

int FWPin = 2;
int motorPin = 3;
int ALMPin = 7;
int speed = 50;

void setup() {
  // put your setup code here, to run once:
  pinMode(motorPin, OUTPUT);  // sets the pin as output
  pinMode(FWPin, OUTPUT);
  analogWrite(motorPin, 0);

  // pinMode(ALMPin, INPUT);

  Serial.begin(9600);
}

void loop() {

  int ALMsignal = digitalRead(ALMPin);
  Serial.println(ALMsignal);

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');  // Read the incoming serial data
    command.trim();                                 // Remove leading and trailing white spaces

    if (command.equals("forward")) {
      // Forward rotation
      analogWrite(motorPin, speed);
      digitalWrite(FWPin, HIGH);
    } else if (command.equals("reverse")) {
      // Reverse rotation
      analogWrite(motorPin, speed);
      digitalWrite(FWPin, LOW);

    } else if (command.equals("stop")) {
      // stop rotation
      analogWrite(motorPin, 0);
    }
  }
}
