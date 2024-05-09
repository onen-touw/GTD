#pragma once


#define GTD_PIN_HALL_SENSOR                         25
#define GTD_PIN__I2C_SOFTWARE_SCL                   26
#define GTD_PIN__I2C_SOFTWARE_SDA                   27
#define GTD_PIN_CURRENT_ALLERT                      11


// I2C адреса датчиков 
#define GTD_DEVICE_PCA9685                          0
#define GTD_DEVICE_INA_1_ADDR                       0
#define GTD_DEVICE_INA_2_ADDR                       0
#define GTD_DEVICE_BME280_ADDR                      0




#define GTD_PCA9685_CHANEL_PWR_EN                   0
#define GTD_PCA9685_CHANEL_GLOW_PLUG_DRIVER         1
#define GTD_PCA9685_CHANEL_MAIN_FUEL_VALVE          3
#define GTD_PCA9685_CHANEL_START_FUEL_VALVE         4
#define GTD_PCA9685_CHANEL_FUEL_PUMP                5
#define GTD_PCA9685_CHANEL_STARTER                  6
#define GTD_PCA9685_CHANEL_LED_R                    7
#define GTD_PCA9685_CHANEL_LED_Y                    8
#define GTD_PCA9685_CHANEL_LED_G                    9
#define GTD_PCA9685_CHANEL_LED_B                    10

enum GTD_WiFiMode {
    STA,
    DEVICE,
};


// #define GTD_BME_REG_MAP 0

enum GTD_SUCCESS_CODE {
    GOOD,
    BAD,
};

enum GTD_TASKS_ID : uint8_t {
    NO_TASK,
    INITIALIZATION,
    TEST_TELEMETRY,
    TEST_FUEL_PUMP,
    TEST_STARTER,
    TEST_GLOW_PLUG,
    TEST_FUEL_STARTER_VALVE,
    TEST_FUEL_VALVE,
    MAIN,
};


struct GTD_TaskDescriptor 
{
    const char * const tName;
    GTD_TASKS_ID ID;
    const uint32_t tStackSize;
    const uint8_t tPriority;
    const uint64_t tUpdatePeriodMs;           // delay(200) (into task)
    void (* tFunction)( void * );
    const uint8_t tCoreID;
};