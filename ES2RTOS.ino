#include <B31DGMonitor.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

B31DGCyclicExecutiveMonitor mon;

// Pin Definitions
#define T1pin 22
#define T2pin 23
#define F1pin 12
#define F2pin 14
#define freqLED 19
#define toglLED 18
#define btn 17

// Task handles
TaskHandle_t taskHandles[5] = {NULL};
SemaphoreHandle_t freqMutex;
volatile bool buttonPressed = false;

// Frequency measurement variables
volatile float F1 = 0;
volatile float F2 = 0;
volatile bool led2State = false;

void task1(void *pvParameters) {
    const TickType_t xFrequency = pdMS_TO_TICKS(4);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    for(;;) {
        mon.jobStarted(1);
        digitalWrite(T1pin, HIGH);
        delayMicroseconds(250);
        digitalWrite(T1pin, LOW);
        delayMicroseconds(50);
        digitalWrite(T1pin, HIGH);
        delayMicroseconds(300);
        digitalWrite(T1pin, LOW);
        mon.jobEnded(1);
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void task2(void *pvParameters) {
    const TickType_t xFrequency = pdMS_TO_TICKS(3);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    for(;;) {
        mon.jobStarted(2);
        digitalWrite(T2pin, HIGH);
        delayMicroseconds(100);
        digitalWrite(T2pin, LOW);
        delayMicroseconds(50);
        digitalWrite(T2pin, HIGH);
        delayMicroseconds(200);
        digitalWrite(T2pin, LOW);
        mon.jobEnded(2);
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void task3(void *pvParameters) {
    const TickType_t xFrequency = pdMS_TO_TICKS(10);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    for(;;) {
        mon.jobStarted(3);
        
        // Simplified frequency measurement (replace with actual implementation)
        float measuredF1 = 666.0; // Example value
        
        if(xSemaphoreTake(freqMutex, portMAX_DELAY) == pdTRUE) {
            F1 = measuredF1;
            xSemaphoreGive(freqMutex);
        }
        
        mon.jobEnded(3);
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void task4(void *pvParameters) {
    const TickType_t xFrequency = pdMS_TO_TICKS(10);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    for(;;) {
        mon.jobStarted(4);
        
        // Simplified frequency measurement (replace with actual implementation)
        float measuredF2 = 833.0; // Example value
        
        if(xSemaphoreTake(freqMutex, portMAX_DELAY) == pdTRUE) {
            F2 = measuredF2;
            xSemaphoreGive(freqMutex);
        }
        
        mon.jobEnded(4);
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void task5(void *pvParameters) {
    const TickType_t xFrequency = pdMS_TO_TICKS(5);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    for(;;) {
        mon.jobStarted(5);
        mon.doWork();
        mon.jobEnded(5);
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}



void handleButton() 
{
    if(digitalRead(btn)==HIGH) {
        led2State = !led2State;
        digitalWrite(toglLED, led2State);
        mon.doWork();
    }
}

void checkFrequencies() {
    float sumF = 0;
    
    if(xSemaphoreTake(freqMutex, portMAX_DELAY) == pdTRUE) {
        sumF = F1 + F2;
        xSemaphoreGive(freqMutex);
    }
    
    digitalWrite(freqLED, (sumF > 1500) ? HIGH : LOW);
}

void setup() {
    Serial.begin(115200);
    
    // Initialize pins
    pinMode(T1pin, OUTPUT);
    pinMode(T2pin, OUTPUT);
    pinMode(F1pin, INPUT);
    pinMode(F2pin, INPUT);
    pinMode(freqLED, OUTPUT);
    pinMode(toglLED, OUTPUT);
    pinMode(btn, INPUT_PULLUP);
    
    digitalWrite(T1pin, LOW);
    digitalWrite(T2pin, LOW);
    digitalWrite(freqLED, LOW);
    digitalWrite(toglLED, LOW);
    
    // Create semaphore
    freqMutex = xSemaphoreCreateMutex();
    
    mon.startMonitoring();
    // Create tasks with priorities (higher number = higher priority)
    xTaskCreate(task1, "Task1", 1024, NULL, 0, &taskHandles[0]); // 250Hz
    xTaskCreate(task2, "Task2", 1024, NULL, 2, &taskHandles[1]); // 333Hz
    xTaskCreate(task5, "Task5", 1024, NULL, 0, &taskHandles[2]); // 200Hz
    xTaskCreate(task3, "Task3", 1024, NULL, 0, &taskHandles[3]); // 100Hz
    xTaskCreate(task4, "Task4", 1024, NULL, 0, &taskHandles[4]); // 100Hz
}

void loop() {
    handleButton();
    
}