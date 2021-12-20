/**
 * http://little-scale.blogspot.com/2011/08/usb-midi-game-boy-sync-for-16.html
 * http://milkcrate.com.au/_other/downloads/teensy/Teensy_GB_Sync/Teensy_GB_Sync_20110903.pde
 */

#include <Arduino.h>
#include <usbmidi.h>

#if defined(__AVR_ATtiny85__)
  #define LED_PIN 0 // default LED pin Model A
  #define LSDJ_PIN 1 // (GB) SC / CLK (Red)
  #define NANOLOOP_PIN 2 // (GB) SI (X)
  #define DIN_PULSE_PIN 3
  #define DIN_START_PIN 4

// #elif defined(CORE_TEENSY)
#elif defined (__MK20DX256__) || defined (__MK20DX128__) || defined (__MKL26Z64__)
  #define LED_PIN 11 // default Teensy 2 LED pin
  #define LSDJ_PIN 0 // (GB) SC / CLK (Red)
  #define NANOLOOP_PIN 1 // (GB) SI (X)
  #define DIN_PULSE_PIN 2
  #define DIN_START_PIN 3

#elif defined(__AVR_ATmega32U4__)
  #if defined(ARDUINO_MICRO)
    #define LED_PIN 17 // defauly Pro Micro LED pin
    #define LSDJ_PIN 7 // (GB) SC / CLK (Red)
    #define NANOLOOP_PIN 10 // (GB) SI (X)
    #define DIN_PULSE_PIN 2
    #define DIN_START_PIN 3
  #endif

#else
  #error Only ATtiny85, Teensy, and ATmega32U4 boards are supported
#endif

#define CLOCK 0xF8  // 248
#define START 0xFA  // 250
#define CONTINUE 0xFB  // 251
#define STOP 0xFC  // 252

static uint8_t counter;

void LEDSync() {
  if (counter == 23) {
    counter = 0;
    digitalWrite(LED_PIN, HIGH);
  }

  if (counter == 11) {
    digitalWrite(LED_PIN, LOW);
  }
}

void LSDJSync() {
  for (int i = 0; i < 8; i ++) {
    digitalWrite(LSDJ_PIN, LOW);
    digitalWrite(LSDJ_PIN, HIGH);
  }
}

void nanoloopSync() {
  digitalWrite(NANOLOOP_PIN, counter % 12);
}

void dinSync() {
#ifdef DIN_PULSE_PIN
  digitalWrite(DIN_PULSE_PIN, HIGH);
  delay(3);
  digitalWrite(DIN_PULSE_PIN, LOW);
#endif
}

void RealTimeSystem(byte realtimebyte) {
  if (realtimebyte == CLOCK) {
    LEDSync();
    LSDJSync();
    nanoloopSync();
    dinSync();

    counter++;
  }

  if (realtimebyte == START || realtimebyte == CONTINUE) {
    counter = 0;
    digitalWrite(LED_PIN, HIGH);
#ifdef DIN_START_PIN
    digitalWrite(DIN_START_PIN, HIGH);
#endif
  }

  if (realtimebyte == STOP) {
    digitalWrite(LED_PIN, LOW);
#ifdef DIN_START_PIN
    digitalWrite(DIN_START_PIN, LOW);
#endif
  }
}

void setup() {
  Serial.begin(9600);

#if defined(__AVR_ATmega32U4__)
  Serial1.begin(31250); // MIDI baudrate.
  USBMIDI.setHandleRealTimeSystem(RealTimeSystem);
#endif

  pinMode(LED_PIN, OUTPUT);
  pinMode(LSDJ_PIN, OUTPUT); // LSDJ sync output
  pinMode(NANOLOOP_PIN, OUTPUT); // Nanoloop sync output

#ifdef DIN_PULSE_PIN
  pinMode(DIN_PULSE_PIN, OUTPUT); // 24 ppm sync pulse output
#endif

#ifdef DIN_START_PIN
  pinMode(DIN_START_PIN, OUTPUT); // 24 ppm start / stop output
#endif
  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  USBMIDI.poll();
  USBMIDI.read();
}