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

#include "GTD_Device_Pca9685.h"
// #include "Adafruit_INA219.h"
// #include "GTD_Device_Bme280.h"
#include "Adafruit_BME280.h"
// #include"Adafruit_INA219.h"
#include "INA219.h"

#define SHUNT_MAX_V 0.1 /* Rated max for our shunt is 75mv for 50 A current: \
                            we will mesure only up to 20A so max is about 75mV*20/50 lets put some more*/
#define BUS_MAX_V 12.6  /* with 12v lead acid battery this should be enough*/
#define MAX_CURRENT 20  /* In our case this is enaugh even shunt is capable to 50 A*/
#define SHUNT_R 0.005   /* Shunt resistor in ohm */

GTD_Device_Pca9685 pca(GTD_DEVICE_PCA9685);
INA219 INA(0x41);
Adafruit_BME280 bme;
int32_t ti = 0;

void setup()
{
    Serial.begin(115200);
    GTD_FileInit();
    GTD_FileList();
    GTD_WiFi_Init();
    GTD_ASWS_Init();

#ifdef USING_OTA
    GTD_OTA_init();
#endif

    Wire.setPins(GTD_PIN__I2C_SOFTWARE_SDA, GTD_PIN__I2C_SOFTWARE_SCL);
    delay(200);
    if (!bme.begin(0x76, &Wire))
    {
        Serial.println("ERROR");
    }
    delay(200);
    if (INA.begin())
    {
        Serial.println("INA ERROR");
    }
    delay(200);
    pca.begin(Wire);
    pca.setPWMFreq(50);
    for (size_t i = 0; i < 15; ++i)
    {
        pca.setPin(i, 0);
    }
    delay(200);
    INA.calibrate(SHUNT_R, SHUNT_MAX_V, BUS_MAX_V, MAX_CURRENT);
    delay(200);
    ASWS_FromWS = [&](int chanel, int pwm)
    {
        if (chanel > 15)
        {
            for (size_t i = 0; i < 15; ++i)
            {
                pca.setPin(i, 4096);
            }
        }
        else if (chanel < 0)
        {
            for (size_t i = 0; i < 15; ++i)
            {
                pca.setPin(i, 0);
            }
        }

        pwm = constrain(pwm, 0, 4096);
        pca.setPin(chanel, pwm);
    };

    // GTD_CreateTask(GTD_TASKS_ID::MAIN);
    // GTD_CreateTask(GTD_TASKS_ID::WIFI);
}

void loop()
{
#ifdef USING_OTA
    GTD_OTA_handle();
#endif

    if (millis() - ti > 1000)
    {
        String _data = "T" + String(bme.readTemperature()) + ";" + "P" + String(bme.readPressure()) + ";" + "H" + String(bme.readHumidity()) + ";" + "V" + String(INA.busVoltage()) + ";" + "I" + String(INA.shuntCurrent()) + ";" + "v" + String(INA.shuntVoltage()) + ";" + "W" + String(INA.busPower()) + ";";
        _data += digitalRead(36);
        _data += ";";
        _data += digitalRead(39);
        ASWS_SendClient(_data);
    }
    delay(1000);
}
