#include <Arduino_FreeRTOS.h>
#define OUTPUT_PIN_1 5
#define OUTPUT_PIN_2 6

void task1(void *param1){
  for(;;){
    digitalWrite(OUTPUT_PIN_1, HIGH);
    delay(2000);
    digitalWrite(OUTPUT_PIN_1, LOW);
    delay(1000);
  }
}

void task2(void *param){
  for(;;){
    digitalWrite(OUTPUT_PIN_2, HIGH);
    delay(2000);
    digitalWrite(OUTPUT_PIN_2, LOW);
    delay(1000);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(OUTPUT_PIN_1, OUTPUT);
  pinMode(OUTPUT_PIN_2, OUTPUT);
  xTaskCreate(task1, "T1", 100, NULL, 0, NULL);
  xTaskCreate(task2, "T2", 100, NULL, 1, NULL);
}

void loop() {
}
