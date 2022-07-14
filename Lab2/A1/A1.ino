#include <Arduino_FreeRTOS.h>
#define OUTPUT_PIN 5

void task(void *param){
  for(;;){
    digitalWrite(OUTPUT_PIN, HIGH);
    delay(2000);
    digitalWrite(OUTPUT_PIN, LOW);
    delay(1000);
  }
}


void setup() {
  Serial.begin(9600);
  pinMode(OUTPUT_PIN, OUTPUT);
  xTaskCreate(task, "T", 100, NULL, 0, NULL);
}

void loop() {
}
