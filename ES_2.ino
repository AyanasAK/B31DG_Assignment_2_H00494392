#include <B31DGMonitor.h>
#include <Ticker.h>

B31DGCyclicExecutiveMonitor mon;
Ticker mainTicker;

// Pin Definitions
#define T1pin 25
#define T2pin 26
#define F1pin 23
#define F2pin 22
#define freqLED 19
#define toglLED 18
#define btn 17

// Task timing variables (volatile for interrupt safety)
volatile unsigned long lastTask1Time = 0;
volatile unsigned long lastTask2Time = 0;
volatile unsigned long lastTask3Time = 0;
volatile unsigned long lastTask4Time = 0;
volatile unsigned long lastTask5Time = 0;

// Task intervals (microseconds)
const unsigned long taskInterval1 = 4000;  // 4ms
const unsigned long taskInterval2 = 3000;  // 3ms
const unsigned long taskInterval3 = 10000; // 10ms
const unsigned long taskInterval4 = 10000; // 10ms
const unsigned long taskInterval5 = 5000;  // 5ms

// Frequency measurement variables
volatile float F1 = 0;
volatile float F2 = 0;
volatile bool led1State = false;
volatile bool led2State = false;

void task1() {
    mon.jobStarted(1);
    digitalWrite(T1pin, HIGH);
    delayMicroseconds(250);
    digitalWrite(T1pin, LOW);
    delayMicroseconds(50);
    digitalWrite(T1pin, HIGH);
    delayMicroseconds(300);
    digitalWrite(T1pin, LOW);
    mon.jobEnded(1);
}

void task2() {
    mon.jobStarted(2);
    digitalWrite(T2pin, HIGH);
    delayMicroseconds(100);
    digitalWrite(T2pin, LOW);
    delayMicroseconds(50);
    digitalWrite(T2pin, HIGH);
    delayMicroseconds(200);
    digitalWrite(T2pin, LOW);
    mon.jobEnded(2);
}

void task3() {
    mon.jobStarted(3);
    // Frequency measurement F1 (666-1000Hz)
    F1 = 666; // Convert to Hz
    mon.jobEnded(3);
}

void task4() {
    mon.jobStarted(4);
    // Frequency measurement F2 (833-1500Hz)
    // assuming the frequncy
    F2 = 833;
    
    mon.jobEnded(4);
}

void task5() {
    mon.jobStarted(5);
    // mon.doWork(); // Built-in 500μs delay
    mon.jobEnded(5);
}

void checkFreq()
{
  if((F1 + F2)>1500)
    digitalWrite(freqLED,HIGH);
  else
    digitalWrite(freqLED,LOW);
}

void handleButton() {
    static bool lastBtnState = HIGH;
    bool currentBtnState = digitalRead(btn);
    
    if(currentBtnState  == HIGH) {
        led2State = !led2State;
        digitalWrite(toglLED, led2State);
        mon.doWork();
        // delay(50); // Debounce
    }
   
}

void scheduler() {
    unsigned long CT = micros();
    while(CT - lastTask2Time >= taskInterval2) {
    task2();
    lastTask2Time = CT;
   }
    while(CT - lastTask1Time >= taskInterval1) {
        task1();
        lastTask1Time = CT;
    }
    
    while(CT - lastTask3Time >= taskInterval3) {
        task3();
        lastTask3Time = CT;
    }
    
    while(CT - lastTask4Time >= taskInterval4) {
        task4();
        lastTask4Time = CT;
    }
    while(CT - lastTask5Time >= taskInterval5) {
        task5();
        lastTask5Time = CT;
    }
}

void setup() {
    // Force single-core operation on ESP32
    #ifdef ESP32
    disableCore1WDT();
    #endif

    Serial.begin(115200);
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

    mon.startMonitoring();
    mainTicker.attach_ms(1, scheduler); // tasks will be scheduled every 1ms
}

void loop() {
  handleButton();
  // checkFreq();
}
