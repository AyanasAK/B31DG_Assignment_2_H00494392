#include <B31DGMonitor.h>
#include <Ticker.h>

B31DGCyclicExecutiveMonitor mon;
Ticker mainTicker;

#define T1pin 25
#define T2pin 26
#define F1pin 34
#define F2pin 35
#define LED1pin 27
#define LED2pin 14
#define BTNpin 13

unsigned long lastTask1Time = 0;
unsigned long lastTask2Time = 0;
unsigned long lastTask3Time = 0;
unsigned long lastTask4Time = 0;
unsigned long lastTask5Time = 0;

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
  // Serial.println("T1");
}

void task2() {
  mon.jobStarted(2);
  // Optimized Task 2 pattern (total 350μs)
  digitalWrite(T2pin, HIGH);
  delayMicroseconds(100);
  digitalWrite(T2pin, LOW);
  delayMicroseconds(50);
  digitalWrite(T2pin, HIGH);
  delayMicroseconds(200);
  digitalWrite(T2pin, LOW);
  // Serial.println("T2");
  mon.jobEnded(2);
}

void task3() {
  mon.jobStarted(3);
  // Frequency measurement stub
  mon.jobEnded(3);
}

void task4() {
  mon.jobStarted(4);
  // Frequency measurement stub
  mon.jobEnded(4);
}

void task5() {
  mon.jobStarted(5);
  mon.doWork();
  mon.jobEnded(5);
}

void scheduler() {
  unsigned long currentTime = micros();
  // Serial.println(currentTime);
  while (currentTime - lastTask1Time >= 4000) {
    task1();
    lastTask1Time = currentTime; 
  }
  
  while (currentTime - lastTask2Time >= 3000) {
    task2();
    lastTask2Time = currentTime;
  }
  
  while (currentTime - lastTask3Time >= 10000) {
    task3();
    lastTask3Time = currentTime;
  }
  
  while (currentTime - lastTask4Time >= 10000) {
    task4();
    lastTask4Time = currentTime;
  }
  
  while (currentTime - lastTask5Time >= 5000) {
    task5();
    lastTask5Time = currentTime;
  }
  
  // Button and LED handling (non-critical)
  // static bool lastBtnState = HIGH;
  // bool currentBtnState = digitalRead(BTNpin);
  // if (currentBtnState == LOW && lastBtnState == HIGH) {
  //   static bool ledState = false;
  //   ledState = !ledState;
  //   digitalWrite(LED2pin, ledState);
  //   mon.doWork();
  // }
  // lastBtnState = currentBtnState;
}

void setup() {
  Serial.begin(9600);
  
  pinMode(T1pin, OUTPUT);
  pinMode(T2pin, OUTPUT);
  pinMode(F1pin, INPUT);
  pinMode(F2pin, INPUT);
  pinMode(LED1pin, OUTPUT);
  pinMode(LED2pin, OUTPUT);
  pinMode(BTNpin, INPUT_PULLUP);
  
  digitalWrite(T1pin, LOW);
  digitalWrite(T2pin, LOW);
  digitalWrite(LED1pin, LOW);
  digitalWrite(LED2pin, LOW);
  
  mon.startMonitoring();
  mainTicker.attach_ms(1, scheduler);
}

void loop() {
  // Serial.println(currentTime);
// scheduler();
  }