#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>

const char *ssid = "SSID";
const char *password = "Password";

AsyncWebServer server(80);

int prev = 1;
int count = 0;
int counttot = 0;
int countold = 0;
int minute = 0;
int i = 0;
int i2 = -1;
unsigned long LastTime[3];
int RPM[240];
int calories;
int estrpm;
int estkmh;

void setup()
{
    Serial.begin(115200);
    pinMode(4, INPUT_PULLUP);

    if (!SPIFFS.begin())
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    Serial.println(WiFi.localIP());

    String WEBRPM()
    {
        return String(estrpm);
    }
    String WEBKM()
    {
        return String(counttot * 0.003);
    }
    String WEBKMH()
    {
        return String(estKMH);
    }
    String WEBTime()
    {
        return String(int((millis() * 0.001) / 60)) + ":" + String((millis() * 0.001) - (int((millis() * 0.001) / 60) * 60));
    }
    String WEBCalories()
    {
        return String(calories);
    }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html"); });

    server.on("/RPM", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/plain", WEBRPM().c_str()); });

    server.on("/KM", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/plain", WEBKM().c_str()); });

    server.on("/KMH", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/plain", WEBKMH().c_str()); });

    server.on("/Time", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/plain", WEBTime().c_str()); });

    server.on("/Calories", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/plain", WEBCalories().c_str()); });
    server.begin();
}

void loop()
{
    // This part runs every minute to save the amount of RPM done into an array in order to retrive data post workout
    if ((millis() - LastTime[0]) >= 60000)
    {
        RPM[minute] = int(count / 2);
        LastTime[0] = millis();
        minute += 1;
        calories += int((RPM[minute] / 11));
        count = 0;
    }

    // This part check whether you're pedaling or no + some signal debouncing
    if (digitalRead(4) == 0 && prev == 1 && (millis() - LastTime[1]) > 150)
    {
        count++;
        counttot++;
        prev = digitalRead(4);
        LastTime[1] = millis();
    }
    else
    {
        prev = digitalRead(4);
    }

    if ((millis() - LastTime[2]) >= 2000)
    {
        estrpm = (counttot - countold) * 30;
        estkmh = (estrpm * 7 * 0.06);
        countold = counttot;
    }
}
