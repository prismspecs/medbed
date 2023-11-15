/*
IBT-2 Motor Control Board driven by Arduino.

Speed and direction controlled by a potentiometer attached to analog input 0.
One side pin of the potentiometer (either one) to ground; the other side pin to +5V

Connection to the IBT-2 board:
IBT-2 pin 1 (RPWM) to Arduino pin 5(PWM)
IBT-2 pin 2 (LPWM) to Arduino pin 6(PWM)
IBT-2 pins 3 (R_EN), 4 (L_EN), 7 (VCC) to Arduino 5V pin
IBT-2 pin 8 (GND) to Arduino GND
IBT-2 pins 5 (R_IS) and 6 (L_IS) not connected
*/

int RPWM_Output = 5; // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
int LPWM_Output = 6; // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)
int speed = 255;

void setup()
{
  pinMode(RPWM_Output, OUTPUT);
  pinMode(LPWM_Output, OUTPUT);

  Serial.begin(38400);
}

void loop()
{
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Read the incoming serial data
    command.trim(); // Remove leading and trailing white spaces

    if (command.equals("forward")) {
      // Forward rotation
      int forwardPWM = speed;
      analogWrite(RPWM_Output, forwardPWM);
      analogWrite(LPWM_Output, 0);
    } else if (command.equals("reverse")) {
      // Reverse rotation
      int reversePWM = speed;
      analogWrite(LPWM_Output, reversePWM);
      analogWrite(RPWM_Output, 0);
    }
    else if (command.equals("stop")) {
      // stop rotation
      analogWrite(LPWM_Output, 0);
      analogWrite(RPWM_Output, 0);
    }
  }
}
