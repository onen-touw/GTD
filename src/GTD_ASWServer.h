#pragma once

#include "GTD_config.h"
#include "GTD_FileData.h"
#include "ESPAsyncWebServer.h"
#include "AsyncWebSocket.h"
#include "functional"

#ifdef USING_WIFI

std::function<void(int, int)> ASWS_FromWS;
AsyncWebServer server(80);
AsyncWebSocket socket("/ws");

// todo: write own block with data_protocol
String getSensorReadings() // for a while
{
    String jsonString = String("123") + " " + String(1000);
    return jsonString;
}

void ASWS_SendClient(String& sensorReadings)
{
    socket.textAll(sensorReadings);
}

void ASWS_HandleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;
        String message = reinterpret_cast<const char *>(data);
        Serial.print("message:");
        Serial.println(message);
        int32_t endPoint = message.length();
        if (message.length() < 0)
        {
            return;
        }
        while (endPoint != 0 && message[endPoint] != '!')
        {
            --endPoint;
        }
        Serial.println(message);
        int32_t arD[2];
        int32_t i = 0, i_ar = 0, i_tm = 0;
        while (i != endPoint)
        {
            if (message[i] == ';')
            {
                arD[i_ar++] = message.substring(i_tm, i).toInt();
                i_tm = i + 1;
            }
            ++i;
        }

        Serial.print("parsed: pin=");
        Serial.print(arD[0]);
        Serial.print(" ; val=");
        Serial.println(arD[1]);
        ASWS_FromWS(arD[0], arD[1]);
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
