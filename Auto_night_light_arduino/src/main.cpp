#include <Arduino.h>

const int LDR_PIN = A0;

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    int ldrValue = analogRead(LDR_PIN);

    Serial.print("LDR Value: ");
    Serial.println(ldrValue);

    delay(200);
}