// #include "GTD_Tasks.h"

// #include "Adafruit_PWMServoDriver.h"
#include <Wire.h>
#include "Adafruit_BME280.h"

#include <WiFi.h>
#include <ArduinoOTA.h>
#define SEALEVELPRESSURE_HPA (1013.25)
const char *ssid = "KULON";
const char *password = "1Qwertyuiop";
// Adafruit_PWMServoDriver pca9685;
Adafruit_BME280 bme;

void setup()
{
    Serial.begin(115200);
    Serial.println("Booting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    ArduinoOTA
        .onStart([]()
                 {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type); })
        .onEnd([]()
               { Serial.println("\nEnd"); })
        .onProgress([](unsigned int progress, unsigned int total)
                    { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
        .onError([](ota_error_t error)
                 {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      } });

    ArduinoOTA.begin();

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Wire.setPins(27, 26);
    // pca9685 = Adafruit_PWMServoDriver(0x40, Wire);
    // pca9685.begin();

    // if(!bme.begin(0x76, &Wire))
    // {
    //     Serial.println("ERRROR");
    // }

    // pca9685.setPWMFreq(50);
    // int pwm0 = map(90, 0, 1000, 80, 2000);

    // pca9685.setPin(7, 4096);
    // pca9685.setPin(8, 4096);
    // pca9685.setPin(9, 4096);
    // pca9685.setPin(10, 4096);

    // GTD_CreateTask(GTD_TASKS_ID::MAIN);
    // GTD_CreateTask(GTD_TASKS_ID::WIFI);
}
void printValues()
{
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}
void loop()
{
    ArduinoOTA.handle();

    Serial.println("OTAWorks normal");

    // if (Serial.available() > 1)
    // {
    //     char key = Serial.read();
    //     int val = Serial.parseInt();
    //     switch (key)
    //     {
    //     case 'R':
    //         Serial.print("R typed\n");

    //         val =  constrain(val, 0, 4096);
    //         pca9685.setPin(7, val);
    //         pca9685.setPin(8, val);
    //         pca9685.setPin(9, val);
    //         pca9685.setPin(10, val);

    //         break;
    //     }
    // }

    //   printValues();
    // Serial.println(digitalRead(25));
    // Serial.println(analogRead(25));
    // Serial.println(hallRead());
    // Serial.println("============");
    //   delay(5);
}

// #include <ESPmDNS.h>
// #include <NetworkUdp.h>



// void setup()
// {

//     // Port defaults to 3232
//     // ArduinoOTA.setPort(3232);

//     // Hostname defaults to esp3232-[MAC]
//     // ArduinoOTA.setHostname("myesp32");

//     // No authentication by default
//     // ArduinoOTA.setPassword("admin");

//     // Password can be set with it's md5 value as well
//     // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
//     // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
// }

