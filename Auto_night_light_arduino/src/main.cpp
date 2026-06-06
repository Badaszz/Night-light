#include <Arduino.h>

const int LDR_PIN = A0;
const int LED_PIN = 9;

int ldrValue = 0;
int brightness = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
}

void loop()
{
    ldrValue = analogRead(LDR_PIN);

    brightness = map(ldrValue, 0, 1023, 255, 0);
    analogWrite(LED_PIN, brightness);

    Serial.print("LDR Value: ");
    Serial.println(ldrValue);

    Serial.print("Brightness: ");
    Serial.println(brightness);

    delay(50);
}