#include "GTD_Tasks.h"

void setup() {
Serial.begin(115200);

    GTD_CreateTask(GTD_TASKS_ID::MAIN);

}

void loop() {
    Serial.println("loop");
    delay(2000);
}

