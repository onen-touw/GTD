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
#include "Adafruit_INA219.h"
// #include "GTD_Device_Bme280.h"
// #include "Adafruit_BME280.h"

GTD_Device_Pca9685 pca(GTD_DEVICE_PCA9685);
Adafruit_INA219 INA(0x44);
// Adafruit_BME280 bme;
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
    // if (!bme.begin(0x77, &Wire))
    // {
    //     Serial.println("ERROR");
    // }
    if (INA.begin(&Wire))
    {
        Serial.println("INA ERROR");
    }

    pca.begin(Wire);
    pca.setPWMFreq(50);
    ASWS_FromWS = [&](int chanel, int pwm)
    {
        pwm = constrain(pwm, 0, 4096);
        // pwm = map(pwm, 1000, 2000, 4096/2, 4096);
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

    if (millis() - ti > 5000)
    {
        Serial.println(INA.getBusVoltage_V());
        Serial.println(INA.getCurrent_mA());
        Serial.println(INA.getShuntVoltage_mV());
        Serial.println(INA.getPower_mW());
    }
    //     ti = millis();
    //     // bme.readTemperature();
    //     // String _data = "T"
    //     // + String(bme.readTemperature())
    //     // + ";"
    //     // + "P"
    //     // + String(bme.readPressure())
    //     // + ";"
    //     // + "H"
    //     // + String(bme.readHumidity())
    //     // + ";";
    //     // ASWS_SendClient(_data);
    // }
    delay(1000);
}
