#include <NewPing.h>

#define SONAR_COUNT 1
#define LED_COUNT 3
#define SONAR_MAX_DISTANCE 200

NewPing sonars[SONAR_COUNT] = {
    NewPing(3, 2, SONAR_MAX_DISTANCE)};

// leds[id] = pin;
const int leds[LED_COUNT] = {13, 12, 11};

bool checkIfHigh(int ledPin)
{
  return digitalRead(ledPin);
}

// Stolen from https://stackoverflow.com/a/14824108
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void publishString(String data)
{
  Serial.write((char *)data.c_str());
}

void sendSonarData(int sonarId)
{
  String data = String("SONAR;") + sonarId + ";" + String(sonars[sonarId].ping_cm()) + "\n";

  publishString(data);
}

void sendLedStatus(int ledId)
{
  String data = String("LED;") + ledId + ";" + checkIfHigh(leds[ledId]) + "\n";

  publishString(data);
}

void turnOnLed(int ledPin)
{
  digitalWrite(ledPin, HIGH);
}

void turnOffLed(int ledPin)
{
  digitalWrite(ledPin, LOW);
}

void setup()
{
  Serial.begin(9600);

  // Set leds as output.
  for (int i = 0; i < LED_COUNT; i++)
  {
    pinMode(leds[i], OUTPUT);
  }
}

void loop()
{
  if (Serial.available() > 0)
  {
    String data = Serial.readStringUntil('\n');

    if (getValue(data, ';', 0) == "SONAR")
    {
      int sonarId = getValue(data, ';', 1).toInt();
      String action = getValue(data, ';', 2);

      if (action == "QUERY")
        sendSonarData(sonarId);
    }

    if (getValue(data, ';', 0) == "LED")
    {
      int ledId = getValue(data, ';', 1).toInt();
      String action = getValue(data, ';', 2);

      if (action == "QUERY")
        sendLedStatus(ledId);
      if (action == "ON")
        turnOnLed(leds[ledId]);
      if (action == "OFF")
        turnOffLed(leds[ledId]);
    }
  }
}