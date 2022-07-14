#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 50

typedef struct Monitor{
  SemaphoreHandle_t mutex;
  SemaphoreHandle_t bufFullSem;
  SemaphoreHandle_t bufEmptySem;
  int buffer[MAX_SIZE];
  int index = 0;
}myMonitor;

myMonitor monitor;
SemaphoreHandle_t mutexForPrints = NULL;

void init(myMonitor* monitor){
  monitor->mutex = xSemaphoreCreateMutex();
  monitor->bufFullSem = xSemaphoreCreateCounting(MAX_SIZE, MAX_SIZE-1);
  monitor->bufEmptySem = xSemaphoreCreateCounting(MAX_SIZE, 0);
}

void read(int* x, myMonitor* monitor){
  xSemaphoreTake(monitor->bufEmptySem, portMAX_DELAY);
  xSemaphoreTake(monitor->mutex, portMAX_DELAY);
  int returnValueIndex = --(monitor->index);
  *x = monitor->buffer[returnValueIndex];
  xSemaphoreGive(monitor->mutex);
  xSemaphoreGive(monitor->bufFullSem);
}

void add(int x, myMonitor* monitor){
  xSemaphoreTake(monitor->bufFullSem, portMAX_DELAY);
  xSemaphoreTake(monitor->mutex, portMAX_DELAY);
  monitor->buffer[monitor->index++] = x;
  xSemaphoreGive(monitor->mutex);
  xSemaphoreGive(monitor->bufEmptySem);
}

void producer1(void *arg){
  while(true){
    int r = rand();
    add(r, &monitor);
    xSemaphoreTake(mutexForPrints, portMAX_DELAY);
    Serial.println("Producer 1 added value to buffer");
    xSemaphoreGive(mutexForPrints);
  }  
}

void producer2(void *arg){
  while(true){
    int r = rand();
    add(r, &monitor);
    xSemaphoreTake(mutexForPrints, portMAX_DELAY);
    Serial.println("Producer 2 added value to buffer");
    xSemaphoreGive(mutexForPrints);
  } 
}

void consumer1(void *arg){
  while(true){
    int x = 0;
    read(&x, &monitor);
    xSemaphoreTake(mutexForPrints, portMAX_DELAY);
    Serial.print("Consumer 1 read from buffer: ");
    Serial.println(x);
    xSemaphoreGive(mutexForPrints);
  }
}

void consumer2(void *arg){
  while(true){
    int x = 0;
    read(&x, &monitor);
    xSemaphoreTake(mutexForPrints, portMAX_DELAY);
    Serial.print("Consumer 2 read from buffer: ");
    Serial.println(x);
    xSemaphoreGive(mutexForPrints);
  }
}

void setup() {
  Serial.begin(9600);
  init(&monitor);
  srand(time(NULL));
  
  xTaskCreate(producer1, "producer1", 128, NULL, 0, NULL);
  xTaskCreate(producer2, "producer2", 128, NULL, 0, NULL);
  xTaskCreate(consumer1, "consumer1", 128, NULL, 0, NULL);
  xTaskCreate(consumer2, "consumer2", 128, NULL, 0, NULL);

  mutexForPrints = xSemaphoreCreateMutex();
}

void loop() {

}
