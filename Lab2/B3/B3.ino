#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "queue.h"  
#define OUTPUT_PIN_1 5
#define OUTPUT_PIN_2 6

QueueHandle_t queueTask1;
QueueHandle_t queueTask2;
const TickType_t xTicksToWait = pdMS_TO_TICKS(100UL);

void task1(void *arg){  
  while(true){ 
    
    BaseType_t xStatus;
    int valueTask1 = 0;
    
    xStatus = xQueueReceive(queueTask1, &valueTask1, xTicksToWait);
    while(xStatus != pdPASS){
       vTaskDelay(pdMS_TO_TICKS(1000));
       xStatus = xQueueReceive(queueTask1, &valueTask1, xTicksToWait);
    }
    
    if (xStatus == pdPASS){   
      Serial.println("Read value from queue 1");   
      analogWrite(OUTPUT_PIN_1, valueTask1);
    }
    
    vTaskDelay(pdMS_TO_TICKS(2000));
    analogWrite(OUTPUT_PIN_1, 0);
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
      
  vTaskDelete(NULL);
}

void task2(void *arg){  
  BaseType_t xStatus;
  int valueTask2 = 0;
  
  while(true){ 
    
    xStatus = xQueueReceive(queueTask2, &valueTask2, xTicksToWait);
    while(xStatus != pdPASS){
       vTaskDelay(pdMS_TO_TICKS(1000));
       xStatus = xQueueReceive(queueTask2, &valueTask2, xTicksToWait);
    }
    
    if (xStatus == pdPASS){ 
      Serial.println("Read value from queue 2");     
      analogWrite(OUTPUT_PIN_2, valueTask2);
    }
    
    vTaskDelay(pdMS_TO_TICKS(2000));
    analogWrite(OUTPUT_PIN_2, 0);
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
      
  vTaskDelete(NULL);
}

void task3(void *arg){ 
    
  while(true){
    
    int valueTask1 = 0;
    int valueTask2 = 0;
    BaseType_t xStatus;       
    
    float input = Serial.parseFloat();
    
    if(input != 0.00){
      
      int task = (int)input;
      
      if(task == 1){
        valueTask1 = (input - (int)input)*255;
        Serial.print("Brightness value for task 1: ");
        Serial.println(valueTask1);
        xStatus = xQueueSendToBack(queueTask1, &valueTask1, xTicksToWait);
        if(xStatus ==  pdPASS){
          Serial.println("Value succesfully sent to queue 1");
        }
        if(xStatus == errQUEUE_FULL){
          Serial.println("Queue 1 full: could not send value");
        }
      }
      
      if(task == 2){
        valueTask2 = (input - (int)input)*255;
        Serial.print("Brightness value for task 2: ");
        Serial.println(valueTask2);
        xStatus = xQueueSendToBack(queueTask2, &valueTask2, xTicksToWait);
        if(xStatus ==  pdPASS){
          Serial.println("Value succesfully sent to queue 2");
        }
        if(xStatus == errQUEUE_FULL){
          Serial.println("Queue 2 full: could not send value");
        }
      }
      
    }
    
    input = 0;

  }
  
  vTaskDelay(pdMS_TO_TICKS(100));    
  vTaskDelete(NULL);
}

void interrupt(void){
  BaseType_t xHigherPriorityTaskWoken = pdFALSE; 
  volatile int16_t msg = 1;

  BaseType_t xStatusQueue1 = xQueueReset(queueTask1);
  BaseType_t xStatusQueue2 = xQueueReset(queueTask2);

  if(xStatusQueue1 == pdPASS && xStatusQueue2 == pdPASS){
    Serial.println("Queues cleared");
  }
}

void setup() {
  Serial.begin(9600);
  
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(2, INPUT_PULLUP);

  queueTask1 = xQueueCreate(5, sizeof(int));
  queueTask2 = xQueueCreate(5, sizeof(int));

  attachInterrupt(digitalPinToInterrupt(2), interrupt, RISING);
  
  xTaskCreate(task1, "T1", 128, NULL, 1, NULL);
  xTaskCreate(task2, "T2", 128, NULL, 1, NULL);
  xTaskCreate(task3, "T3", 128, NULL, 0, NULL);
 
}

void loop() {
}
