#include <Adafruit_NeoPixel.h>
#include<Wire.h>
#include <LiquidCrystal_I2C.h>
#include <NecDecoder.h>
#include "arduinoFFT.h"
////////////////////////////
#define SAMPLES 128             //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 2048 //Ts = Based on Nyquist, must be 2 times the highest expected frequency.
#define VERSION "1.0.4"
arduinoFFT FFT = arduinoFFT();
unsigned int k = 102;
unsigned int samplingPeriod;
unsigned long microSeconds;

double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values
///////////////////////////////////////////////////
LiquidCrystal_I2C lcd(0x3F, 16, 4);
#define PIN 6   // input pin Neopixel is attached to
#define G1 196
#define A1 220
#define B1 247
#define C2 261
#define D2 293
#define E2 329
#define F2 349
#define G2 392
#define A2 440
#define B2 493
#define C3 523
#define D3 587
#define E3 659
#define F3 740
#define G3 784
#define A3 880
#define B3 987
#define NUMPIXELS     16 // number of neopixels in strip
NecDecoder ir;
Adafruit_NeoPixel led = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int delayval = 100; // timing delay in milliseconds
const  int a[] = {G1, A1, B1, C2, D2, E2, F2, G2, A2, B2, C3, D3, E3, F3, G3, A3, B3};
const String ar[] = {"G1", "A1", "B1", "C2", "D2", "E2", "F2", "G2", "A2", "B2", "C3", "D3", "E3", "F3", "G3", "A3", "B3"};
const int indexL[] = { -1, 1, 3, 4, -2, 1, 2, 4, -3, 1, 2, 4, -4, 0, 2, 4, 6};
int redColor = 0;
int greenColor = 0; int blueColor = 0;
void irIsr() {
  ir.tick();
}
void setup() {
  attachInterrupt(0, irIsr, FALLING);
  led.begin();
  lcd.init();
  lcd.backlight();
  samplingPeriod = round(1000000 * (1.0 / SAMPLING_FREQUENCY)); //Period in microseconds
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(10, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(A1, INPUT);
  Serial.begin(9600);
  Serial.println(2333);
  start();


}
void clearS() {
  for (int i = 0; i < NUMPIXELS; i++) {

    led.setPixelColor(i, led.Color(0, 0, 0));

  }
  led.show();
}
void demo() {
  for (int i = 0; i < NUMPIXELS; i++) {

    led.setPixelColor(i, led.Color(map(random(0, 255), 0, 255, 0, k), map(random(0, 255), 0, 255, 0, k), map(random(0, 255), 0, 255, 0, k)));
    led.show();
    delay(50);

  }
}
void printLed(int index) {
  clearS();
  if (index == -1) {
    for (int i = 0; i < NUMPIXELS; i++) {

      led.setPixelColor(i, led.Color(k, 0, 0));

    }
  }
  else if (index == -2) {
    for (int i = 0; i < NUMPIXELS; i++) {

      led.setPixelColor(i, led.Color(0, k, 0));

    }
  }
  else if (index == -3) {
    for (int i = 0; i < NUMPIXELS; i++) {

      led.setPixelColor(i, led.Color(0, 0, k));

    }
  }
  else if (index == -4) {
    for (int i = 0; i < NUMPIXELS; i++) {

      led.setPixelColor(i, led.Color(k, k, 0));

    }
  }
  else {
    led.setPixelColor(index, led.Color(map(random(0, 255), 0, 255, 0, k), map(random(0, 255), 0, 255, 0, k), map(random(0, 255), 0, 255, 0, k)));
  }
  led.show();
}
int i = 0;
void show() {

  clearS();
  printLed(indexL[0]);
  lcd.clear();
  lcd.print("G1");
  i = 0;
  Serial.println(222);
  while (true) {
    if ( ir.available()) {
      Serial.println(99);
      uint32_t r =  ir.readCommand();
      if (r == 0x22) {
        noTone(8);
        lcd.clear();
        i -= 1;
        i = (i + 17) % 17;
        lcd.print(ar[i]);
        printLed(indexL[i]);
      }
      else if (r == 0xC2) {
        noTone(8);
        lcd.clear();
        i += 1;
        i %= 17;
        lcd.print(ar[i]);
        printLed(indexL[i]);
      }
      else if (r == 0x18) {
        lcd.clear();
        clearS();
        menu();
      }
    }
  }
}
bool checkN(double peak, int ind) {
  double right = (double)a[i];
  return abs(right - peak) <= 5;
}
void check() {
  i = 0;
  clearS();
  printLed(indexL[i]);
  lcd.clear();
  lcd.print("G1");
  i = 0;
  while (true) {
    if ( ir.available()) {
      uint32_t r =  ir.readCommand();
      if (r == 0x68) {
        lcd.clear();
        clearS();
        long t = millis();
        for (int i = 0; i < 4; i++) {
          lcd.print(4 - i);
          delay(1000);
          lcd.setCursor(0, 0);
        }
        for (int j = 0; j < SAMPLES; j++)
        {
          microSeconds = micros();

          vReal[j] = analogRead(1);
          vImag[j] = 0;
          while (micros() < (microSeconds + samplingPeriod))
          {
          }
        }
        FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
        FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
        FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
        double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
        lcd.clear();
        lcd.print(peak);
        if (checkN(peak, i)) {
          lcd.setCursor(0, 1);
          lcd.print("good!");
          printLed(-2);
        }
        else {
          lcd.setCursor(0, 1);
          lcd.print("bad");
          printLed(-1);
        }
        delay(5000);
        menu();
        break;
      }
      if (r == 0xC2) {
        lcd.clear();
        i += 1;
        i %= 17;
        lcd.print(ar[i]);
        printLed(indexL[i]);
      }
      if (r == 0x22) {
        noTone(8);
        lcd.clear();
        i -= 1;
        i = (i + 17) % 17;
        lcd.print(ar[i]);
        printLed(indexL[i]);
      }
      if (r == 0x18) {

        lcd.clear();
        clearS();
        menu();
        break;
      }
    }
  }
}
void play() {
  i = 0;
  clearS();
  lcd.clear();
  lcd.print("G1");
  printLed(indexL[i]);
  i = 0;
  Serial.println(222);
  while (true) {
    if ( ir.available()) {
      Serial.println(99);
      uint32_t r =  ir.readCommand();
      if (r == 0x22) {
        noTone(10);
        lcd.clear();
        i -= 1;
        i = (i + 17) % 17;
        lcd.print(ar[i]);
        printLed(indexL[i]);
        tone(10, a[i], 2000);
      }
      else if (r == 0xC2) {
        noTone(10);
        lcd.clear();
        i += 1;
        i %= 17;
        lcd.print(ar[i]);
        printLed(indexL[i]);
        tone(10, a[i], 2000);
      }
      else if (r == 0x18) {
        lcd.clear();
        clearS();
        menu();
      }
    }
  }
}
int choosed = 0;
bool work = true;
bool islight = true;
void start() {
  lcd.clear();

  lcd.print("Ivets Project");
  delay(3000);
  menu();
}
void sett() {
  byte c = 0;
  int v = 255;
  lcd.setCursor(0, 0);
  String s = (c == 0 ? ">" : " ");
  lcd.print(s);
  lcd.print("bright:");
  lcd.print(k);
  lcd.setCursor(0, 1);
  s = (c == 1 ? ">" : " ");
  lcd.print(s);
  lcd.print("volume:");
  lcd.print(v);
  while (true) {
    if (ir.available()) {
      uint32_t r =  ir.readCommand();
      if (r == 0xA8) {
        c += 1;
        c %= 2;
      }
      else if (r == 0x62) {
        c -= 1;
        c = (c + 2) % 2;
      }
      if (r == 0x18) {
        lcd.clear();
        menu();
        break;
      }
      lcd.setCursor(0, 0);
      s = (c == 0 ? ">" : " ");
      lcd.print(s);
      lcd.print("bright:");
      lcd.print(k);
      lcd.setCursor(0, 1);
      
      s = (c == 1 ? ">" : " ");
      lcd.print(s);
      lcd.print("volume:");
      lcd.print(v);
      if (r == 0xC2 && c == 0) {
        k += 1;

        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 0);
        s = (c == 0 ? ">" : " ");
        lcd.print(s);
        lcd.print("bright:");
        k %= 255;
        lcd.print(k);

        printLed(-4);
      }
      if (r == 0x22 && c == 0) {
        k -= 1;

        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 0);
        s = (c == 0 ? ">" : " ");
        lcd.print(s);
        lcd.print("bright:");
        k = (k + 255) % 255;
        lcd.print(k);


        printLed(-4);
      }
      if (r == 0xC2 && c == 1) {
        v += 1;
        noTone(10);
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        s = (c == 1 ? ">" : " ");
        lcd.print(s);
        lcd.print("volume:");
        v %= 255;
        lcd.print(v);
        analogWrite(8, 255 - v);
        tone(10, 220, 1000);

      }
      if (r == 0x22 && c == 1) {
        v -= 1;
        noTone(10);
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        s = (c == 1 ? ">" : " ");
        lcd.print(s);
        lcd.print("volume:");
        v = (v + 255) % 255;
        lcd.print(v);
        analogWrite(8, 255 - v);
        tone(10, 220, 1000);
      }
    }
  }
}
void menu() {
  Serial.println(2000);
  lcd.clear();
  String s = (choosed == 0 ? ">" : " ");
  lcd.print(s);
  lcd.print("Show note");
  lcd.setCursor(0, 1);
  s = (choosed == 1 ? ">" : " ");
  lcd.print(s);
  lcd.print("Play note");
  s = (choosed == 2 ? ">" : " ");
  lcd.setCursor(0, 2);
  lcd.print(s);
  lcd.print("Check");
  s = (choosed == 3 ? ">" : " ");
  lcd.setCursor(0, 3);
  lcd.print(s);
  lcd.print("Settings");
  lcd.setCursor(13, 3);
  lcd.print("v");
  lcd.print(VERSION);
  clearS();
  demo();
  noTone(8);
  while (true) {
    if ( ir.available()) {
      Serial.print("0x");
      Serial.println(ir.readCommand(), HEX);
      uint32_t r =  ir.readCommand();
      if (r == 0x5A) {
        work = !work;
        if (work) {
          if (islight) lcd.backlight();
          else lcd.noBacklight();
          lcd.setCursor(13, 3);
          lcd.print("v");
          lcd.print(VERSION);
          demo();
        }
        else {
          lcd.noBacklight();
          clearS();
          lcd.clear();
        }
      }
      if (work) {
        if (r == 0xC2) {
          choosed += 1;
          choosed %= 4;
        }
        if (r == 0x22) {
          choosed -= 1;
          choosed = (choosed + 4) % 4;
        }
        if (r == 0x30) {
          islight = !islight;
          if (islight) lcd.backlight();
          else lcd.noBacklight();
        }
        lcd.setCursor(0, 0);
        String s = (choosed == 0 ? ">" : " ");
        lcd.print(s);
        lcd.print("Show note");
        lcd.setCursor(0, 1);
        s = (choosed == 1 ? ">" : " ");
        lcd.print(s);
        lcd.print("Play note");
        s = (choosed == 2 ? ">" : " ");
        lcd.setCursor(0, 2);
        lcd.print(s);
        lcd.print("Check");
        s = (choosed == 3 ? ">" : " ");
        lcd.setCursor(0, 3);
        lcd.print(s);
        lcd.print("Settings");
        if (r == 0x7A) {
          k += 51;
          k %= 255;
          printLed(-4);
        }
        if (r == 0xB0 && choosed == 0) {

          lcd.clear();
          show();
          break;
        }
        if (r == 0xB0 && choosed == 1) {

          lcd.clear();
          play();
          break;
        }
        if (r == 0xB0 && choosed == 2) {

          lcd.clear();
          check();
          break;
        }
        if (r == 0xB0 && choosed == 3) {

          lcd.clear();
          sett();
          break;
        }
      }
    }
  }
}
void loop() {

}
void setColor() {
  redColor = 255;
  greenColor = 0;
  blueColor = 0;
}


