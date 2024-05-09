#pragma once
#include "Arduino.h"
#include "GTD_config.h"

void /*IRAM_ATTR*/ GTD_Task_Initialization(void *arg);
void /*IRAM_ATTR*/ GTD_Task_Telemetry_Test(void *arg);
void /*IRAM_ATTR*/ GTD_Task_FuelPump_Test(void *arg);
void /*IRAM_ATTR*/ GTD_Task_Starter_Test(void *arg);
void /*IRAM_ATTR*/ GTD_Task_GlowPlug_Test(void *arg);
void /*IRAM_ATTR*/ GTD_Task_FuelStarterValve_Test(void *arg);
void /*IRAM_ATTR*/ GTD_Task_FuelValve_Test(void *arg);
void /*IRAM_ATTR*/ GTD_Task_Main(void *arg);

struct GTD_TaskDescriptor tasksDesc[] = {
    //  Name                ID                                      StackSize   Prior   Ms      Funck                   CoreID
    {"Init",                GTD_TASKS_ID::INITIALIZATION,           4096,       5,      100,    GTD_Task_Initialization,    0},
    {"TeleTst",             GTD_TASKS_ID::TEST_TELEMETRY,           2048,       5,      20,     GTD_Task_Telemetry_Test,    0},
    {"FlPmpTst",            GTD_TASKS_ID::TEST_FUEL_PUMP,           1024,       5,      20,     GTD_Task_FuelPump_Test,     0},
    {"StrtrTst",            GTD_TASKS_ID::TEST_STARTER,             1024,       5,      20,     GTD_Task_Starter_Test,      0},
    {"GlwPlgTst",           GTD_TASKS_ID::TEST_GLOW_PLUG,           1024,       5,      20,     GTD_Task_GlowPlug_Test,     0},
    {"FlStrtrVlvTst",       GTD_TASKS_ID::TEST_FUEL_STARTER_VALVE,  1024,       5,      20,     GTD_Task_Initialization,    0},
    {"FlVlvTst",            GTD_TASKS_ID::TEST_FUEL_VALVE,          4096,       5,      20,     GTD_Task_FuelValve_Test,    0},
    {"Main",                GTD_TASKS_ID::MAIN,                     4096,       5,      100,    GTD_Task_Main,              0},
};

struct GTD_SendFunckMinimal
{
    uint64_t updTime;
    void *arg;
};

void GTD_CreateTask(GTD_TASKS_ID id, void *dataToFunk = nullptr)
{
    uint8_t rID = static_cast<uint8_t>(id) - 1;
    GTD_SendFunckMinimal *data = new GTD_SendFunckMinimal{tasksDesc[rID].tUpdatePeriodMs, dataToFunk};

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
    GTD_SendFunckMinimal *data = (GTD_SendFunckMinimal *)arg;
     while (true)
    {
        /* code */
        Serial.println("main task");
        delay(data->updTime); // (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
    delete data;
}