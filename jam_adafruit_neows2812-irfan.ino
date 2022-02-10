#include <Adafruit_NeoPixel.h>
#include "RTClib.h"
#include <Wire.h>

#define PinLed D6
#define buzzer D5
#define LEDS_PER_SEG 3
#define LEDS_PER_DOT 2
#define LEDS_PER_DIGIT  LEDS_PER_SEG *7
#define LED   88


unsigned long tmrsave = 0;
unsigned long tmrsaveHue = 0;
int delayWarning(200);
int delayHue(2);
int Delay(500);
int brightnes = 200;
bool dotsOn = false;
int hl;
int hr;
int ml;
int mr;

static int hue;
int pixelColor;
static uint16_t saveTime = 0;
 bool stateBlink=false;

Adafruit_NeoPixel strip(LED, PinLed, NEO_GRB + NEO_KHZ800);

RTC_DS3231 rtc;


long numbers[] = { 
//  7654321
  0b0111111,  // [0] 0
  0b0100001,  // [1] 1
  0b1110110,  // [2] 2
  0b1110011,  // [3] 3
  0b1101001,  // [4] 4
  0b1011011,  // [5] 5
  0b1011111,  // [6] 6
  0b0110001,  // [7] 7
  0b1111111,  // [8] 8
  0b1111011,  // [9] 9
  0b0000000,  // [10] off
  0b1111000,  // [11] degrees symbol
  0b0011110,  // [12] C(elsius)
  0b1011110,  // [13] E
  0b0111101,  // [14] n(N)
  0b1001110,  // [15] t
  0b1111110,  // [16] e
  0b1000101,  // [17] n
  0b1000100,  // [18] r
  0b1000111,  // [19] o
  0b1100111,  // [20] d
  0b0000001,  // [21] i
  0b1000110,  // [22] c
};




void setup() {
  digitalWrite(buzzer,LOW);
  pinMode(buzzer,OUTPUT);
  Serial.begin(9600);
   if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  strip.begin();
  strip.setBrightness(brightnes);
  updateClock();
   delay(1000);
  resultTest();
  Serial.println("DONE");
  strip.show();
  delay(5000);
}

void loop() {
  updateClock();
  timerHue();
  Restart();
  Alarm();
  
  ShowClock(Wheel((hue+pixelColor) & 255));
  displayDots(strip.Color(255, 0, 0));

  strip.show();

}

void DisplayShow(byte number, byte segment, uint32_t color) {
  // segment from left to right: 3, 2, 1, 0
  byte startindex = 0;
  switch (segment) {
    case 0:
      startindex = 0;
      break;
    case 1:
      startindex = LEDS_PER_DIGIT;
      break;
    case 2:
      startindex = LEDS_PER_DIGIT * 2 + LEDS_PER_DOT * 2;
      break;
    case 3:
      startindex = LEDS_PER_DIGIT * 3 + LEDS_PER_DOT * 2;
      break;
  }

  for (byte i = 0; i < 7; i++) {           // 7 segments
    for (byte j = 0; j < LEDS_PER_SEG; j++) {         // LEDs per segment
      strip.setPixelColor(i * LEDS_PER_SEG + j + startindex , (numbers[number] & 1 << i) == 1 << i ? color : strip.Color(0, 0, 0));
      //Led[i * LEDS_PER_SEG + j + startindex] = ((numbers[number] & 1 << i) == 1 << i) ? color : color(0,0,0);
      strip.show();
    }
  }

  //yield();
}


void updateClock() {
  DateTime now = rtc.now();

  hl = now.hour() / 10;
  hr = now.hour() % 10;
  ml = now.minute() / 10;
  mr = now.minute() % 10;
}

void ShowClock(uint32_t color) {

  DisplayShow(hl, 3, color);
  DisplayShow(hr, 2, color);
  DisplayShow(ml, 1, color);
  DisplayShow(mr, 0, color);
}

void displayDots(uint32_t color) {
  unsigned long tmr = millis();
  if (tmr - tmrsave > Delay) {
    tmrsave = tmr;
    if (dotsOn) {
      for (int i = 42; i <= 45; i++) {
        strip.setPixelColor(i , color);
      }

    } else {
      for (int i = 42; i <= 45; i++) {
        strip.setPixelColor(i , strip.Color(0, 0, 0));
      }
    }
    dotsOn = !dotsOn;
  }
  strip.show();
}

void timerHue() {
  unsigned long tmr = millis();
  if (tmr - tmrsaveHue > delayHue) {
    tmrsaveHue = tmr;
    if (pixelColor < 256) {
      pixelColor++;
      if (pixelColor == 255) {
        pixelColor = 0;
      }
    }
  }

  for (int hue = 0; hue < strip.numPixels(); hue++) {
    hue++;

  }
}

void Restart(){
  DateTime now = rtc.now();
  int jam = now.hour();
  int menit = now.minute();
  int detik = now.second();

  if(jam == 0 && menit == 0 && detik == 0){
    Buzzer(1);
    delay(1000);
    ESP.restart();
  }

  if(jam == 18 && menit == 30 && detik == 0){
    Buzzer(1);
    delay(1000);
    ESP.restart();
  }
  
}

void Alarm(){
   DateTime now = rtc.now();
  int jam = now.hour();
  int menit = now.minute();
  int detik = now.second();

  if(jam == 4 && menit == 45 && detik == 0){
    BuzzerBlink(1);
  }
  if(jam == 4 && menit == 46 && detik == 0){
    BuzzerBlink(0);
  }
  
   if(jam == 5 && menit == 0 && detik == 0){
    BuzzerBlink(1);
  }
  if(jam == 5 && menit == 1 && detik == 0){
    BuzzerBlink(0);
  }

   if(jam == 12 && menit == 0 && detik == 0){
    BuzzerBlink(1);
  }
   if(jam == 12 && menit == 1 && detik == 0){
    BuzzerBlink(0);
  }

}

void Buzzer(bool Value){
  if(Value == 1){ digitalWrite(buzzer,HIGH); }
  else{ digitalWrite(buzzer,LOW); }
}
  

void BuzzerBlink(bool state){
  
  uint16_t currentMillis = millis();

  if (currentMillis - saveTime >= 100 && state == true) {
    // save the last time you blinked the LED
    saveTime = currentMillis;
    
    if (stateBlink) {
      digitalWrite(buzzer,LOW);
    } else {
      digitalWrite(buzzer,HIGH);
    } 
    stateBlink = !stateBlink;
     
  }
  if(state == false){
    digitalWrite(buzzer,LOW);
    saveTime =0;
  }
}

void resultTest(){
  DisplayShow( 20, 3,strip.Color(0,255,0));
  DisplayShow( 19, 2,strip.Color(0,255,0));
  DisplayShow( 17, 1,strip.Color(0,255,0));
  DisplayShow( 16, 0,strip.Color(0,255,0));
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
