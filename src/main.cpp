#include <Arduino.h>
#include <FastLED.h>

#define ECHO_PIN 1
#define TRIGGER_PIN 2

#define LED_STRIP 3
#define LED_COUNT 42

long duration;
float speedOfSound = 0.034;

// * Tracking a moving average
const int totalReadings = 50;
int sensorReadings[totalReadings];
int currentIndex = 0;
int currentSum = 0;

uint8_t lightLevel;
CRGB leds[LED_COUNT];
uint8_t hue = 253;
uint8_t saturation = 255;

void setupUltrasonicSensor()
{
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void setupLedStrip()
{
  FastLED.addLeds<NEOPIXEL, LED_STRIP>(leds, LED_COUNT);
}

void setup()
{
  setupUltrasonicSensor();
  setupLedStrip();

  Serial.begin(9600);
}

void updateLEDs(int brightnessLevel)
{
  lightLevel = map(brightnessLevel, 0, 100, 255, 0);

  // for (int i = 0; i < LED_COUNT; i++)
  // {
  //   hue += 1;
  //   if (hue % 255 == 0)
  //   {
  //     hue = 0;
  //   }
  //   leds[i] = CHSV(hue, saturation, lightLevel);
  // }
  fill_solid(leds, LED_COUNT, CHSV(hue, saturation, lightLevel));
  FastLED.show();
}

int readSensor()
{
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  // * Speed of sound wave divided by 2 (out and back)
  return duration * speedOfSound / 2;
}

void logDistance(int distance)
{
  Serial.print(distance);
  Serial.print(" cm");
  for (int i = 0; i < distance; i++)
  {
    Serial.print("-");
  }
  Serial.println("|");
}

int updateMovingAverage(int rawDistance)
{
  currentSum -= sensorReadings[currentIndex];
  sensorReadings[currentIndex] = rawDistance;
  currentSum += rawDistance;
  currentIndex = (currentIndex + 1) % totalReadings;

  return currentSum / totalReadings;
}

unsigned long colorChangeInterval = 10;
unsigned long colorChangeLastChecked = 0;

void loop()
{
  unsigned long now = millis();
  if (now - colorChangeLastChecked > colorChangeInterval)
  {
    // int distance = readSensor();
    int distance = updateMovingAverage(readSensor());
    logDistance(distance);
    colorChangeLastChecked = now;
    updateLEDs(distance);
  }
}