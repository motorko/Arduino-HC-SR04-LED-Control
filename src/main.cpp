#include <Arduino.h>

#include "GammaCorrect.h"

#define VB_DEB 0
#define VB_CLICK 900
#include <VirtualButton.h>

VButton gest;

#define GREEN1 11
#define GREEN2 10
#define YELLOW1 9
#define YELLOW2 6
#define RED1 5
#define RED2 3

#define TRIGGER_PIN 8
#define ECHO_PIN 7
#define MAX_DISTANCE 50

GammaCorrect gammaCorrect;

const int leds[] = {RED2, RED1, YELLOW2, YELLOW1, GREEN2, GREEN1};

int bringht = 0;

enum Mode { RED, YELLOW, GREEN, ALL };
Mode mode = ALL;

void check();
void render();
int getDist(byte trig, byte echo);
int getFilterMedian(int newVal);
int getFilterSkip(int val);
int getFilterExp(int val);

void setup() {
  for (int pin : leds) pinMode(pin, OUTPUT);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  check();
  render();
}

void check() {
  unsigned long currentTime = millis();
  static unsigned long lastTime = 0;
  const unsigned long interval = 50;

  if (currentTime - lastTime < interval) return;
  lastTime = currentTime;

  static int offsetDist;
  static byte offsetBringht;

  int dist = getDist(TRIGGER_PIN, ECHO_PIN);
  dist = getFilterMedian(dist);
  dist = getFilterSkip(dist);
  int distFiltered = getFilterExp(dist);

  gest.poll(dist);

  if (gest.click() && bringht > 0) {
    mode = (Mode)((mode + 1) % 4);
  }

  if (gest.held()) {
    offsetDist = distFiltered;
    offsetBringht = bringht;
  }

  if (gest.hold()) {
    int shift = constrain(offsetBringht + (distFiltered - offsetDist), 0, 255);

    bringht = shift;
  }
}

void render() {
  for (int i = 0; i < 6; i++) {
    if (mode == ALL || mode == i / 2) {
      analogWrite(leds[i], gammaCorrect.getValue(bringht));
    } else {
      analogWrite(leds[i], 0);
    }
  }
}

int getDist(byte trig, byte echo) {
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  unsigned int us = pulseIn(echo, HIGH, (1000L * 2 * 1000 / 343));

  return (us * 343L / 2000);
}

int getFilterMedian(int newVal) {
  static int buf[3];
  static byte count = 0;
  buf[count] = newVal;
  if (++count >= 3) count = 0;
  return (max(buf[0], buf[1]) == max(buf[1], buf[2]))
             ? max(buf[0], buf[2])
             : max(buf[1], min(buf[0], buf[2]));
}

int getFilterSkip(int val) {
  static int prev;
  static byte count;

  if (!prev && val) prev = val;

  if (abs(prev - val) > 80 || !val) {
    count++;
    if (count > 7) {
      prev = val;
      count = 0;
    } else
      val = prev;
  } else
    count = 0;
  prev = val;

  return val;
}

int getFilterExp(int val) {
  static long filt;
  if (val)
    filt += (val * 16L - filt) / 2L;
  else
    filt = 0;
  return filt / 16L;
}
