#include <Adafruit_NeoPixel.h>
#define LEDS_PIN 6
#define NUM_LEDS 90
Adafruit_NeoPixel pixels(NUM_LEDS, LEDS_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
    pixels.begin();  // INITIALIZE NeoPixel pixels object (REQUIRED)

}

void loop() {
  // put your main code here, to run repeatedly:
  LEDheartBeat(random(5)+1,random(5)+10);
}

// spd is how much the light brightness increases per cycle
// d is the delay within the loop
void LEDheartBeat(int spd, int d) {
  for (int b = 20; b < 255-spd; b += spd) {
    for (int i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    }
    pixels.setBrightness(b);
    pixels.show();
    delay(d);
  }
  for (int b = 255; b > 20+spd; b -= spd) {
    for (int i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    }
    pixels.setBrightness(b);
    pixels.show();
    delay(d);
  }
}