#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#define OUTPUT_LED_1 5
#define OUTPUT_LED_2 6

SemaphoreHandle_t mutex1 = NULL;
volatile bool turnOnLED1 = false;
volatile bool turnOnLED2 = false;
volatile int brightness = 0;

void task1(void *arg)
{  
  for(;;)
  { 
    xSemaphoreTake(mutex1, portMAX_DELAY);
    //read the variable in common with task3 and act accordingly
    if(turnOnLED1 == true){
      analogWrite(OUTPUT_LED_1, brightness);
      turnOnLED1 = false;
    }
    xSemaphoreGive(mutex1);
    vTaskDelay(pdMS_TO_TICKS(2000));
    analogWrite(OUTPUT_LED_1, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
      
  vTaskDelete(NULL);
}

void task2(void *arg)
{  
  for(;;)
  { 
    xSemaphoreTake(mutex1, portMAX_DELAY);
    //read the variable in common with task3 and act accordingly
    if(turnOnLED2 == true){
      analogWrite(OUTPUT_LED_2, brightness);
      turnOnLED2 = false;
    }
    xSemaphoreGive(mutex1);
    vTaskDelay(pdMS_TO_TICKS(2000));
    analogWrite(OUTPUT_LED_2, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
      
  vTaskDelete(NULL);
}

void task3(void *arg)
{  
  xSemaphoreTake(mutex1, portMAX_DELAY);
  while(true){   
    float input = Serial.parseFloat();
    if(input != 0.00){
      int task = (int)input;
      if(task == 1){
        turnOnLED1 = true;
        brightness = (input - (int)input)*255;
      }
      if(task == 2){
        turnOnLED2 = true;
        brightness = (input - (int)input)*255;
      }
    }
    input = 0;
    xSemaphoreGive(mutex1);
  }
  vTaskDelay(pdMS_TO_TICKS(100));    
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  mutex1 = xSemaphoreCreateMutex();
  if(mutex1 != NULL){
    xTaskCreate(task1, "T1", 128, NULL, 1, NULL);
    xTaskCreate(task2, "T2", 128, NULL, 1, NULL);
    xTaskCreate(task3, "T3", 128, NULL, 0, NULL);
  }
}

void loop() {
}
