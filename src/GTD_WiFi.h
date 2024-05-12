#pragma once

#include "GTD_config.h"

#ifdef USING_WIFI

#include <WiFi.h>

void GTD_WiFi_Init()
{
    WiFi.mode(WIFI_MODE_NULL);
#if (GTD_WF_MODE == WF_MODE_STA)

    WiFi.mode(WIFI_STA);
    WiFi.begin(GTD_WIFI_NET_NAME, GTD_WIFI_NET_PASS);
    Serial.print("Connecting to ");
    Serial.println(GTD_WIFI_NET_NAME);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(3000);
        ESP.restart();
    }
    WiFi.printDiag(Serial);
    Serial.println(WiFi.localIP());
    return;

#elif (GTD_WF_MODE == WF_MODE_AP)
    WiFi.softAP(GTD_WIFI_OWN_NAME, GTD_WIFI_OWN_PASS, 1, 0, GTD_WIFI_MAX_CONNECTION);
    WiFi.softAPConfig(GTD_WIFI_local_ip, GTD_WIFI_gateway, IPAddress(255, 255, 255, 0));
    delay(1000);
    Serial.println(WiFi.softAPIP());
    return;

#else
    int64_t ti = millis();

    WiFi.mode(WIFI_STA);
    WiFi.begin(GTD_WIFI_NET_NAME, GTD_WIFI_NET_PASS);
    Serial.print("Connecting to ");
    Serial.println(GTD_WIFI_NET_NAME);
    while (WiFi.status() != WL_CONNECTED && (millis() - ti < LIMIT_WiFi_DEVICE_CONNETION_TIME))
    {
        Serial.print(".");
        delay(200);
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println(WiFi.localIP());
        WiFi.printDiag(Serial);
        return;
    }
    else
    {
        Serial.println("Non Connecting to WiFi..");
        Serial.println("Start  WiFiAp..");

        WiFi.softAP(GTD_WIFI_OWN_NAME, GTD_WIFI_OWN_PASS, 1, 0, GTD_WIFI_MAX_CONNECTION);
        WiFi.softAPConfig(GTD_WIFI_local_ip, GTD_WIFI_gateway, IPAddress(255, 255, 255, 0));
        delay(1000);
        Serial.println(WiFi.softAPIP());
    }
#endif
    return;
}

#endif