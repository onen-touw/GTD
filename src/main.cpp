
#include "GTD_config.h"
#include "GTD_FileData.h"

#ifdef USING_OTA
#include "GTD_OTA.h"
#endif

#ifdef USING_WIFI
#include "GTD_WiFi.h"
#endif

#ifdef USING_ASWSERVER
#include "GTD_ASWServer.h"
#endif




#include "Adafruit_BME280.h"


// Adafruit_PWMServoDriver pca9685;
volatile int data__ = 0;
Adafruit_BME280 bme;





int32_t ti = 0;

void setup()
{
    Serial.begin(115200);
    GTD_FileInit();
    GTD_FileList();
   
    GTD_WiFi_Init();
    GTD_ASWS_Init();

    // GTD_OTA_init();


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
// void printValues()
// {
//     Serial.print("Temperature = ");
//     Serial.print(bme.readTemperature());
//     Serial.println(" *C");

//     Serial.print("Pressure = ");
//     Serial.print(bme.readPressure() / 100.0F);
//     Serial.println(" hPa");

//     Serial.print("Approx. Altitude = ");
//     Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
//     Serial.println(" m");

//     Serial.print("Humidity = ");
//     Serial.print(bme.readHumidity());
//     Serial.println(" %");

//     Serial.println();
// }

void loop()
{
    GTD_OTA_handle();

    ++data__;
    if (millis() - ti > 5000)
    {
        // ASWS_SendClient(getSensorReadings());
        ti = millis();
    }
    delay(1000);
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
