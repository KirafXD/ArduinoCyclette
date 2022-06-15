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
int counttot5s = 0;
int minute = 0;
int i = 0;
int i2 = -1;
unsigned long CurrentTime = millis();
unsigned long LastTime = 0;
unsigned long LastTime2 = 0;
int RPM[240];
int calorie;

String R30s()
{
    float t = RPM[(i2)];
    return String(t);
}

String RPM5s()
{
    float t2 = (counttot - counttot5s) * 6;
    return String(t2);
}

String KM()
{
    float t3 = (counttot / 2) * 0.006;
    return String(t3);
}

String KMH5s()
{
    float t4 = (((((counttot - counttot5s) / 2) * 6) / 5) * 3.6);
    counttot5s = counttot;
    return String(t4);
}

String Tempo()
{
    int t5min = int((CurrentTime * 0.001) / 60);
    int t5sec = (CurrentTime * 0.001) - (t5min * 60);
    String t5 = String(t5min) + ":" + String(t5sec);
    return String(t5);
}

String Calorie()
{
    return String(calorie);
}

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

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html"); });

    server.on("/R30s", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/plain", R30s().c_str()); });

    server.on("/RPM5s", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/plain", RPM5s().c_str()); });

    server.on("/KM", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/plain", KM().c_str()); });

    server.on("/KMH5s", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/plain", KMH5s().c_str()); });

    server.on("/Tempo", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/plain", Tempo().c_str()); });

    server.on("/Calorie", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/plain", Calorie().c_str()); });
    server.begin();
}

void loop()
{
    CurrentTime = millis();
    if ((CurrentTime - LastTime) >= 30000)
    {
        RPM[minute] = int(count / 2);
        if (RPM[minute] > 0 && RPM[minute] < 10)
        {
            calorie += 0.5;
        }
        else if (RPM[minute] > 10 && RPM[minute] < 20)
        {
            calorie += 1;
        }
        else if (RPM[minute] > 20 && RPM[minute] < 30)
        {
            calorie += 1.5;
        }
        else if (RPM[minute] > 30 && RPM[minute] < 40)
        {
            calorie += 2;
        }
        else if (RPM[minute] > 40 && RPM[minute] < 50)
        {
            calorie += 2.5;
        }
        else if (RPM[minute] > 50 && RPM[minute] < 60)
        {
            calorie += 3.5;
        }
        else if (RPM[minute] > 60 && RPM[minute] < 70)
        {
            calorie += 6;
        }
        else if (RPM[minute] > 70 && RPM[minute] < 80)
        {
            calorie += 10;
        }
        else if (RPM[minute] > 80 && RPM[minute] < 90)
        {
            calorie += 15;
        }
        else if (RPM[minute] > 90 && RPM[minute] < 100)
        {
            calorie += 20;
        }
        else if (RPM[minute] > 100 && RPM[minute] < 110)
        {
            calorie += 30;
        }
        else if (RPM[minute] > 110 && RPM[minute] < 120)
        {
            calorie += 45;
        }
        else if (RPM[minute] > 120 && RPM[minute] < 130)
        {
            calorie += 65;
        }
        else if (RPM[minute] > 130 && RPM[minute] < 140)
        {
            calorie += 82;
        }
        else if (RPM[minute] > 140 && RPM[minute] < 150)
        {
            calorie += 100;
        }
        else if (RPM[minute] > 150)
        {
            calorie += 110;
        }
        LastTime = CurrentTime;
        minute = minute + 1;
        count = 0;
        Serial.println("Minuti;Giri");
        while (i < minute)
        {
            Serial.print(i);
            Serial.print(';');
            Serial.print(RPM[i]);
            Serial.println();
            i++;
        }
        i = 0;
        i2++;
    }
    if (digitalRead(4) == 0 && prev == 1 && (CurrentTime - LastTime2) > 150)
    {
        count++;
        counttot++;
        Serial.print(count);
        Serial.print("  ");
        Serial.println(CurrentTime - LastTime2);
        prev = digitalRead(4);
        LastTime2 = CurrentTime;
    }
    else
    {
        prev = digitalRead(4);
    }
}
