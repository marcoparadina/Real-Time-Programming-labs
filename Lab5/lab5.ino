#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <stdlib.h>
#include <IRremote.h>

#define POWER 0x00FF629D
#define A 0x00FF22DD
#define B 0x00FF02FD
#define C 0x00FFC23D
#define UP 0x00FF9867
#define DOWN 0x00FF38C7
#define LEFT 0x00FF30CF
#define RIGHT 0x00FF7A85
#define SELECT 0x00FF18E7

#define FORWARD_A 7
#define FORWARD_B 4
#define BACKWARD_A 3
#define BACKWARD_B 8
#define SPEED_A 5
#define SPEED_B 6

#define RECV_PIN 2

#define TRIG_PIN 9
#define ECHO_PIN 10

long duration;
int distance;

SemaphoreHandle_t mutex1 = NULL; //this semaphore protects the speedProfile common variable

IRrecv receiver(RECV_PIN);
decode_results results;

int speedProfile = 0;

void receiveSignal(){
  while(1){
    if (receiver.decode(&results)) {
      switch(results.value){
        case A:
          xSemaphoreTake(mutex1, portMAX_DELAY);           
          speedProfile = 255*0.5;
          xSemaphoreGive(mutex1);
          Serial.println("Speed profile set to 50%");
          break;
        case B:
          xSemaphoreTake(mutex1, portMAX_DELAY);
          speedProfile = 255*0.7;
          xSemaphoreGive(mutex1);     
          Serial.println("Speed profile set to 70%"); 
          break;
        case C: 
          xSemaphoreTake(mutex1, portMAX_DELAY);
          speedProfile = 255;
          xSemaphoreGive(mutex1);
          Serial.println("Speed profile set to 100%");       
          break; 
        case UP:
          Serial.println("forwards"); 
          digitalWrite(FORWARD_A, HIGH);
          digitalWrite(BACKWARD_A, LOW);
          digitalWrite(FORWARD_B, HIGH);
          digitalWrite(BACKWARD_B, LOW);        
          break;
        case DOWN:
          Serial.println("backwards");
          digitalWrite(FORWARD_A, LOW);
          digitalWrite(BACKWARD_A, HIGH);
          digitalWrite(FORWARD_B, LOW);
          digitalWrite(BACKWARD_B, HIGH);   
          break;
        case LEFT:
          Serial.println("left");
          digitalWrite(FORWARD_A, LOW);
          digitalWrite(BACKWARD_A, LOW);
          digitalWrite(FORWARD_B, HIGH);
          digitalWrite(BACKWARD_B, LOW);
          break;
        case RIGHT:
          Serial.println("right");
          digitalWrite(FORWARD_A, HIGH);
          digitalWrite(BACKWARD_A, LOW);
          digitalWrite(FORWARD_B, LOW);
          digitalWrite(BACKWARD_B, LOW);
          break;
        case SELECT:
          Serial.println("stop");
          digitalWrite(FORWARD_A, LOW);
          digitalWrite(BACKWARD_A, LOW);
          digitalWrite(FORWARD_B, LOW);
          digitalWrite(BACKWARD_B, LOW);
          break;
    }

      //Sets the motors speed accordingly to what is the value of the common variable speedProfile
      xSemaphoreTake(mutex1, portMAX_DELAY);
      int speedP = speedProfile;
      xSemaphoreGive(mutex1);  
      analogWrite(SPEED_A, speedP);
      analogWrite(SPEED_B, speedP);
      
      receiver.resume(); // Receive the next value 
    }
  }

}

void distanceMeasure(){
  while(1){
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);          // Sets the trigPin on HIGH state for 10 micro seconds
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);           // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2;      // Calculating the distance, considering velocity of sound is 340 m/s
    //Serial.print("Distance: ");
    //Serial.println(distance);
    
    xSemaphoreTake(mutex1, portMAX_DELAY);
    int currSpeed = speedProfile;
    xSemaphoreGive(mutex1);

    
    //Checks if critical distance is reached. Critical distance is differenct for every speed profile
    switch(currSpeed){
    case 127: //50%
      if(distance < 6){
        digitalWrite(FORWARD_A, LOW);
        digitalWrite(BACKWARD_A, LOW);
        digitalWrite(FORWARD_B, LOW);
        digitalWrite(BACKWARD_B, LOW);
        analogWrite(SPEED_A, speedProfile);
        analogWrite(SPEED_B, speedProfile);
        Serial.println("Critical distance reached: brakes engaged");
      }
      break;
    case 178: //70%
      if(distance < 9){
        digitalWrite(FORWARD_A, LOW);
        digitalWrite(BACKWARD_A, LOW);
        digitalWrite(FORWARD_B, LOW);
        digitalWrite(BACKWARD_B, LOW);
        analogWrite(SPEED_A, speedProfile);
        analogWrite(SPEED_B, speedProfile);
        Serial.println("Critical distance reached: brakes engaged");
      }
      break;
    case 255:
      if(distance < 12){
        digitalWrite(FORWARD_A, LOW);
        digitalWrite(BACKWARD_A, LOW);
        digitalWrite(FORWARD_B, LOW);
        digitalWrite(BACKWARD_B, LOW);
        analogWrite(SPEED_A, speedProfile);
        analogWrite(SPEED_B, speedProfile);
        Serial.println("Critical distance reached: brakes engaged");
      }
      break;
   
    }
    
  }
  
}

void setup(){
  Serial.begin(9600);
  receiver.enableIRIn();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(2, INPUT);
  pinMode(4, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(7, OUTPUT);
  
  mutex1 = xSemaphoreCreateMutex();
  
  xTaskCreate(receiveSignal, "Receivesignal", 128, NULL, 0, NULL);
  xTaskCreate(distanceMeasure, "Distancemeasured", 128, NULL, 0, NULL);
}

void loop(){
}
