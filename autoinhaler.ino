#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <ESP32Servo.h>

#define BLYNK_TEMPLATE_ID "TMPL374k35dCm"
#define BLYNK_TEMPLATE_NAME "Automatic Inhaler System using ESP32"
#define BLYNK_AUTH_TOKEN "4liW0mnTzbYqJgbvALGg4aHa_S6WZCZd"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define REPORTING_PERIOD_MS     1000

PulseOximeter pox;
Servo servo1;

float BPM, Spo2;
uint32_t tsLastReport = 0;
int gas_Sensor=34;
int servo_pin=25;
int servoPosition = 0;
char ssid[] = "Balaji-4G";
char pass[] = "skfamily";

void onBeatDetected()
{
    Serial.println("Beat!");
}

BLYNK_WRITE(V5) {
  servoPosition = param.asInt();  // Get the value from the Blynk app slider
  servo1.write(servoPosition);  // Set the servo position
}

void setup()
{
    Serial.begin(115200);
    pinMode(gas_Sensor,INPUT);
    servo1.attach(servo_pin);
    servo1.write(0);

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    Serial.print("Initializing pulse oximeter..");

    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{
    Blynk.run();
    pox.update();
    BPM = pox.getHeartRate();
    Spo2 = pox.getSpO2();
    int val=analogRead(gas_Sensor);
    Blynk.virtualWrite(V3, val);
    if(val>900)
    {
        Blynk.virtualWrite(V4,1);
        delay(500);
        Blynk.virtualWrite(V4,0);
        delay(100);
        servo1.write(90);
        delay(200);
        Blynk.virtualWrite(V5,90);
        delay(1000);
        servo1.write(0);
        delay(200);
        Blynk.virtualWrite(V5,0);
        delay(1000);
    }
    // if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        Serial.print("Heart rate:");
        Serial.print(BPM);
        Serial.println("bpm / SpO2:");
        Serial.print(Spo2);
        Serial.println("%");
    //     Blynk.virtualWrite(V1, BPM);
    //     Blynk.virtualWrite(V2, Spo2);
    //     Blynk.virtualWrite(V3, val);
    //     tsLastReport = millis();
    // }

    if (BPM > 30 && BPM < 180 && Spo2 > 70 && Spo2 <= 100) {
    Blynk.virtualWrite(V1, BPM);
    Blynk.virtualWrite(V2, Spo2);
}

}
