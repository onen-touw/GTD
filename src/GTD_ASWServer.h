#pragma once

#include "GTD_config.h"
#include"GTD_FileData.h"
#include "ESPAsyncWebServer.h"
#include "AsyncWebSocket.h"

#ifdef USING_WIFI

AsyncWebServer server(80);
AsyncWebSocket socket("/ws");

// todo: write own block with data_protocol 
String getSensorReadings()      // for a while
{
    String jsonString = String("123") + " " + String(1000);
    return jsonString;
}

void ASWS_SendClient(String sensorReadings)
{
    socket.textAll(sensorReadings);
}


void ASWS_HandleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        // data[len] = 0;
        String message = (char *)data;
        // String sensorReadings = getSensorReadings();
        // Serial.print(sensorReadings);
        // ASWS_SendClient(sensorReadings);
        Serial.print("message:");
        Serial.println(message);
    }
}

void ASWS_OnEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        ASWS_HandleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void GTD_ASWS_Init()
{
    socket.onEvent(ASWS_OnEvent);
    
    server.addHandler(&socket);
    
    server.on("/", HTTP_GET,
              [](AsyncWebServerRequest *request)
              {
                  Serial.println("ON /");
                  request->send(SPIFFS, "/page.html", "text/html");
              });
    server.serveStatic("/", SPIFFS, "/");

    server.begin();
}



#endif
