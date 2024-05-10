#pragma once
#include "Arduino.h"
#include "GTD_config.h"

#ifdef USING_WIFI
#include "WiFi.h"
#endif

// header functions
void /*IRAM_ATTR*/ GTD_Task_Initialization(void *arg);
void /*IRAM_ATTR*/ GTD_Task_Telemetry_Test(void *arg);
void /*IRAM_ATTR*/ GTD_Task_FuelPump_Test(void *arg);
void /*IRAM_ATTR*/ GTD_Task_Starter_Test(void *arg);
void /*IRAM_ATTR*/ GTD_Task_GlowPlug_Test(void *arg);
void /*IRAM_ATTR*/ GTD_Task_FuelStarterValve_Test(void *arg);
void /*IRAM_ATTR*/ GTD_Task_FuelValve_Test(void *arg);
void /*IRAM_ATTR*/ GTD_Task_WiFi(void *arg);
void /*IRAM_ATTR*/ GTD_Task_Main(void *arg);


// WARNING!!! ORDER should be same as in  GTD_TASKS_ID enum (whitout NO_TASK)
struct GTD_TaskDescriptor tasksDesc[] = {
    //  Name                ID                                      StackSize   Prior   Ms      Funck                   CoreID
    {"Init",                GTD_TASKS_ID::INITIALIZATION,           4096,       5,      100,    GTD_Task_Initialization,    0},
    {"TeleTst",             GTD_TASKS_ID::TEST_TELEMETRY,           2048,       5,      20,     GTD_Task_Telemetry_Test,    0},
    {"FlPmpTst",            GTD_TASKS_ID::TEST_FUEL_PUMP,           1024,       5,      20,     GTD_Task_FuelPump_Test,     0},
    {"StrtrTst",            GTD_TASKS_ID::TEST_STARTER,             1024,       5,      20,     GTD_Task_Starter_Test,      0},
    {"GlwPlgTst",           GTD_TASKS_ID::TEST_GLOW_PLUG,           1024,       5,      20,     GTD_Task_GlowPlug_Test,     0},
    {"FlStrtrVlvTst",       GTD_TASKS_ID::TEST_FUEL_STARTER_VALVE,  1024,       5,      20,     GTD_Task_Initialization,    0},
    {"FlVlvTst",            GTD_TASKS_ID::TEST_FUEL_VALVE,          4096,       5,      20,     GTD_Task_FuelValve_Test,    0},
    {"WiFi",                GTD_TASKS_ID::WIFI,                     4096,       5,      200,    GTD_Task_WiFi,              1},
    {"Main",                GTD_TASKS_ID::MAIN,                     4096,       5,      100,    GTD_Task_Main,              0},
};

struct GTD_SendToFunckMinimal
{
    uint64_t updTime;
    GTD_TASKS_ID taskID;
    void *arg;
};

void GTD_CreateTask(GTD_TASKS_ID id, void *dataToFunk = nullptr)
{
    uint8_t rID = static_cast<uint8_t>(id) - 1;
    GTD_SendToFunckMinimal *data = new GTD_SendToFunckMinimal{tasksDesc[rID].tUpdatePeriodMs, id, dataToFunk};

    xTaskCreatePinnedToCore(
        tasksDesc[rID].tFunction,
        tasksDesc[rID].tName,
        tasksDesc[rID].tStackSize,
        (void *)data,
        tasksDesc[rID].tPriority,
        NULL,
        tasksDesc[rID].tCoreID);
    delete data;
}




//WARNING!!! 
// ESP_DRAM_LOGx for interraption (maybe for IRAM_ATTR also )





// инициализация (проверка всего и т.д.)
void /*IRAM_ATTR*/ GTD_Task_Initialization(void *arg)
{
    while (true)
    {
        /* code */

        delay(200); // for a while  (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
}

// Проверка связи(получение и отправка) от сервера(управляющего устройства)
void /*IRAM_ATTR*/ GTD_Task_Telemetry_Test(void *arg)
{
    while (true)
    {
        /* code */

        delay(200); // for a while  (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
}

// Проверка главного насоса
void /*IRAM_ATTR*/ GTD_Task_FuelPump_Test(void *arg)
{
    while (true)
    {
        /* code */

        delay(200); // for a while  (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
}

// Проверка стартера
void /*IRAM_ATTR*/ GTD_Task_Starter_Test(void *arg)
{
    while (true)
    {
        /* code */

        delay(200); // for a while  (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
}

// Проверка свечи
void /*IRAM_ATTR*/ GTD_Task_GlowPlug_Test(void *arg)
{
    while (true)
    {
        /* code */

        delay(200); // for a while  (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
}

// Проверка насоса на свече
void /*IRAM_ATTR*/ GTD_Task_FuelStarterValve_Test(void *arg)
{
    while (true)
    {
        /* code */

        delay(200); // for a while  (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
}

// Проверка насоса на свече
void /*IRAM_ATTR*/ GTD_Task_FuelValve_Test(void *arg)
{
    while (true)
    {
        /* code */

        delay(200); // for a while  (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
}



// Главный цикл
void /*IRAM_ATTR*/ GTD_Task_Main(void *arg)
{
    GTD_SendToFunckMinimal *data = (GTD_SendToFunckMinimal *)arg;
     while (true)
    {
        /* code */
        Serial.println("main task");
        delay(data->updTime); // (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
    delete data;
}










#ifdef USING_WIFI

// #if WiFiMode == AUTO 
const char *ssid = "TP-Link_2722";
const char *password = "89565544";
const char *ssidAp = "esp32";
const char *passwordAp = "12345678";
IPAddress local_ip(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
// #endif

void /*IRAM_ATTR*/ GTD_Task_WiFi(void *arg)
{
    GTD_SendToFunckMinimal *data = (GTD_SendToFunckMinimal *)arg;

    WiFi.begin(ssid, password);
    uint64_t time = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - time < LIMIT_WiFi_DEVICE_CONNETION_TIME))
    {
        Serial.print(".");
        log_d("GTD_WiFi", ".");
        delay(100);
    }
    log_d("GTD_WiFi", "\n");

    if (WiFi.status() != WL_CONNECTED)
    {
        ESP_LOGW ("GTD_WiFi", "LOG::WiFi DEVICE INITIALIZATION ERROE\n");
        WiFi.softAP(ssidAp, passwordAp, 1, 0, MAX_CONNECTION);

    }else{
        ESP_LOGW ("GTD_WiFi", "LOG::WiFi DEVICE INITIALIZED\n");
    }


    

    // while (true)
    {
        /* code */
        Serial.println("WiFi task");
        delay(data->updTime); // (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
    delete data;
}

#endif
