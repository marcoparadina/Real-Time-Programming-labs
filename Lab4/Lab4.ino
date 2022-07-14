#include <Arduino_FreeRTOS.h>
#include <timers.h>
#define FIRSTPIN 5
#define LASTPIN 13
#define NUMBER_OF_PATTERNS 3
/* 
 *  There are 3 patterns. The description of the patterns are written inside each pattern's call back function. 
 *  The serial monitor shows which pattern is being executed. Patterns are activated in this order: pattern0, pattern1, pattern2, pattern0, ... .
 *  When the button on PIN 2 is pressed, the current pattern stops and the next one starts. Everything is handled with timers, as required.
 *  When the sketch is run the first pattern to be activated is pattern0.
 */

//Pattern 0 variables
int currPin = -1;

//Pattern 1 variables
bool forward = true;
int offset = 0;

//Pattern 2 variables
bool onOrOff = false;

//Interrupt variables
int currPattern = 0;

TimerHandle_t autoReloadHandle0;
TimerHandle_t autoReloadHandle1;
TimerHandle_t autoReloadHandle2;

void pattern0(TimerHandle_t timerHandle){
  //Turns on and off one pin at a time from FIRSTPIN to LASTPIN
  
  if(currPin == -1){
    currPin = FIRSTPIN;
  }
  
  if(currPin == FIRSTPIN){    
    digitalWrite(currPin, HIGH);
    digitalWrite(LASTPIN, LOW);
    currPin++;
  }
  
  if(currPin == LASTPIN){
    digitalWrite(currPin-1, LOW);
    digitalWrite(currPin, HIGH);
    currPin = FIRSTPIN;
  }

  if(currPin > FIRSTPIN && currPin < LASTPIN){
    digitalWrite(currPin-1,LOW);
    digitalWrite(currPin, HIGH);
    currPin++;
  }  
  
}

void pattern1(TimerHandle_t timerHandle){
//Turns on all pins from the outside in and then turns them off from the inside out
  
  if((offset >= 0 && offset < 4) && forward==true){
    digitalWrite(FIRSTPIN+offset, HIGH);
    digitalWrite(LASTPIN-offset, HIGH);
    offset++;
  }
  
  if(offset != 4 && forward==false){
    digitalWrite(FIRSTPIN+offset, LOW);
    digitalWrite(LASTPIN-offset, LOW);
    offset--;
  }
  
  if(offset == 4 && forward){
    digitalWrite(FIRSTPIN+offset, HIGH);
    forward=false;
  }
  
  if(offset == 4 && forward == false){
    digitalWrite(FIRSTPIN+offset, LOW);
    offset--;
  }
  
  if(offset==0 && forward==false){
    digitalWrite(FIRSTPIN+offset, LOW);
    digitalWrite(LASTPIN-offset, LOW);
    forward = true;
  }
  
}

void pattern2(TimerHandle_t timerHandle){
//Makes LEDs blink all at the same time in regular intervals
  
  if(onOrOff == true){
    for(int i=FIRSTPIN;i<=LASTPIN;i++){
      digitalWrite(i, LOW);
    }
    onOrOff = false;
  }
  else{
    for(int i=FIRSTPIN;i<=LASTPIN;i++){
    digitalWrite(i, HIGH);
    }
    onOrOff = true;
  }
}

void interrupt(void){
  BaseType_t xHigherPriorityTaskWoken = pdFALSE; 
  volatile int16_t msg = 1;
  
  switch(currPattern){
    case 0:
      xTimerStop(autoReloadHandle0, 0);
      
      //Initialize pattern0 variables
      for(int i=FIRSTPIN; i<=LASTPIN; i++){
        digitalWrite(i, LOW); 
      }
      currPin = -1;      
      
      break;
      
    case 1:
      xTimerStop(autoReloadHandle1, 0);
      
      //Initialize pattern1 variables
      for(int i=FIRSTPIN; i<=LASTPIN; i++){
        digitalWrite(i, LOW); 
      }
      forward = true;
      offset = 0;
      
      break;
      
    case 2:
    
      xTimerStop(autoReloadHandle2, 0);
      
      //Initialize pattern2 variables
      for(int i=FIRSTPIN; i<=LASTPIN; i++){
        digitalWrite(i, LOW); 
      }
      onOrOff = false;
      
      break;
  }
  Serial.print("Dectivated pattern #");
  Serial.println(currPattern);
  
  currPattern = ++currPattern % NUMBER_OF_PATTERNS;
  
  switch(currPattern){
    case 0:
      xTimerReset(autoReloadHandle0, 0);
      break;
    case 1:
      
      xTimerReset(autoReloadHandle1, 0);
      break;
    case 2:
      
      xTimerReset(autoReloadHandle2, 0);
      break;
  }
  Serial.print("Activated pattern #");
  Serial.println(currPattern);
}

void setup() {
  Serial.begin(9600);
  
  for(int i=5; i<14; i++){
    pinMode(i, OUTPUT); 
  }
  
  pinMode(2, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(2), interrupt, RISING);  
  
  autoReloadHandle0 = xTimerCreate("AutoReload", pdMS_TO_TICKS(100), pdTRUE, NULL, pattern0);
  autoReloadHandle1 = xTimerCreate("AutoReload", pdMS_TO_TICKS(500), pdTRUE, NULL, pattern1);
  autoReloadHandle2 = xTimerCreate("AutoReload", pdMS_TO_TICKS(500), pdTRUE, NULL, pattern2);
  
  if(autoReloadHandle0 != NULL && autoReloadHandle1 !=NULL && autoReloadHandle2 !=NULL){
    Serial.println("Timers created succesfully");
    xTimerStart(autoReloadHandle0, 0);
  }

  currPattern = 0;
  Serial.print("Activated pattern #");
  Serial.println(currPattern);
  
}

void loop() {
  // put your main code here, to run repeatedly:
}
