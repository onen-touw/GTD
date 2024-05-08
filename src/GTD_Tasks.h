#pragma once
#include "Arduino.h"


// инициализация (проверка всего и т.д.)
void /*IRAM_ATTR*/ GTD_Task_Initialization(){
    while (true)
    {
        /* code */

        delay(200); // for a while  (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
}

// Проверка связи(получение и отправка) от сервера(управляющего устройства)
void /*IRAM_ATTR*/ GTD_Task_Telemetry_Test(){
    while (true)
    {
        /* code */

        delay(200); // for a while  (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
}


// Проверка главного насоса
void /*IRAM_ATTR*/ GTD_Task_FuelPump_Test(){
     while (true)
    {
        /* code */

        delay(200); // for a while  (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
}

// Проверка стартера 
void /*IRAM_ATTR*/ GTD_Task_Starter_Test(){
     while (true)
    {
        /* code */

        delay(200); // for a while  (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
}


// Проверка свечи
void /*IRAM_ATTR*/ GTD_Task_GlowPlug_Test(){
     while (true)
    {
        /* code */

        delay(200); // for a while  (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
}


// Проверка насоса на свече
void /*IRAM_ATTR*/ GTD_Task_FuelStarterPump_Test(){
     while (true)
    {
        /* code */

        delay(200); // for a while  (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
}


// Главный цикл
void /*IRAM_ATTR*/ GTD_Task_Main(){
     while (true)
    {
        /* code */

        delay(200); // for a while  (this delay is neccesary for yeild)
    }
    vTaskDelete(NULL);
}