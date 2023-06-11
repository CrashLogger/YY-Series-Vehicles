   /*
 * 
 * An NRF24L01 based RC receiever for L298N based 4 wheel or two wheel drive vehciles with tank steering
 * A standard short pcb laid antenna NRF24L01 is used on this side
 * Designed to run a two pairs of DC motors for tank steering
 * 
 * First created for the CS-25 "Car System 25cm" 4WD vehicle
 * 
 * Erlantz 'CrashLogger' Alonso, 2021
 * 
*/

//To go forward       Left side: 6H 9L                Right side: 4H 5L
//To go backward      Left side: 6L 9H                Right side: 4L 5H

#include <SPI.h>
#include "printf.h"
#include "RF24.h"

struct Package{
  int thrust;
  int rudder;
  int elevator;
  int aileron;
  bool brake;
  bool buzzer;
};
Package data;

//A3 is the reversing light
//A2 is the left indicator
//A1 is the right indicator
//A0 are the braking lights

RF24 radio(7, 8); // using pin 7 for the CE pin, and pin 8 for the CSN pin

uint8_t address[][6] = {"000FA", "000FF"};    //REC, ACK
bool radioNumber = 1; //Receiver will use pipe [1] to transmit acknowledgements

int PowerR = 0;
int PowerL = 0;

int Margin = 64;

int turningRight = 0;
int turningLeft = 0;

unsigned long blinkPeriod = 500;
unsigned long lastMark = 0;
bool blinkerState = false;
unsigned long currentTime;

void setup() {

  // Initialise the SPI bus transciever
  if (!radio.begin()) {
    ////Serial.println(F("radio hardware is not responding!!"));
    while (1) {} // Hold in infinite loop if it can't connect to the controller
  }

  //radio.enableAckPayload();

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);

  radioNumber = 1;
  radio.setPALevel(RF24_PA_HIGH);  // RF24_PA_MAX is default.
  //Optimise sending time by setting the maximum to the variables size
  radio.setPayloadSize(sizeof(data));
  
  //Sets the pipes in the receiver order, opposite to the transmitter order.
  radio.openWritingPipe(address[radioNumber]);
  radio.openReadingPipe(1, address[!radioNumber]);
  radio.startListening();
}

void loop() {
  
  currentTime = millis();
  checkBlinkers();
  blinkers();
  brakeLights();
  reversingLight();

    uint8_t pipe;
    if (radio.available(&pipe)) {                 // Check if there's a package waiting
      uint8_t bytes = radio.getPayloadSize();     // Read the package size
      radio.read(&data, bytes);                   // Read the package
   }
  Power();
}

 void checkBlinkers(){


  if(currentTime - lastMark >= blinkPeriod){
    lastMark = millis();
    if(blinkerState == false){
      blinkerState = true;
    }
    else{
      blinkerState = false;
    }
  }
}

void Power(){
  Left();
  Right();
  if (PowerL>255){
    PowerL = 255;
  }

  if (PowerR > 255){
    PowerR = 255;
  }

  analogWrite(3, PowerR);
  analogWrite(10, PowerL);
}

void Left(){
  if (data.thrust>((1024/2)+Margin)){
    digitalWrite(6, HIGH);
    digitalWrite(9, LOW);
    PowerL = map(data.thrust, (1024/2)+Margin, 1024, 64, 255);
    turningLeft = -1;
  }
  else if (data.thrust<((1024/2)-Margin)){
    digitalWrite(6, LOW);
    digitalWrite(9, HIGH);
    PowerL = map(data.thrust, (1024/2)-Margin, 24, 64, 255);
    turningLeft = 1;
  }
  else{
    digitalWrite(6, LOW);
    digitalWrite(9, LOW);
    PowerL = 0;
    turningLeft = 0;
  }
}

void Right(){
  if (data.elevator>((1024/2)+Margin)){
    digitalWrite(5, HIGH);
    digitalWrite(4, LOW);
    PowerR = map(data.elevator, (1024/2)+Margin, 1024, 64, 255);
    turningRight = 1;
  }
  else if (data.elevator<((1024/2)-Margin)){
    digitalWrite(5, LOW);
    digitalWrite(4, HIGH);
    PowerR = map(data.elevator, (1024/2)-Margin, 24, 64, 255);
    turningRight = -1;
  }
  else{
    digitalWrite(5, LOW);
    digitalWrite(4, LOW);
    PowerR = 0;
    turningRight = 0;
  }
  
}

void brakeLights(){
  if(PowerL < 64 && PowerR < 64){
    digitalWrite(A0, HIGH);
  }
  else{
    digitalWrite(A0, LOW);
  }
}

void reversingLight(){
    if(turningRight == -1 && turningLeft == -1){
    digitalWrite(A3, HIGH);
  }
  else{
    digitalWrite(A3, LOW);
  }
}

void blinkers(){

  if((turningRight > 0 && turningLeft <= 0) || (turningLeft < 0 && turningRight >= 0)){
    digitalWrite(A2, blinkerState);
  }
  else if((turningLeft > 0 && turningRight <= 0) || (turningRight < 0 && turningLeft >= 0)){
    digitalWrite(A1, blinkerState);
  }
  else{
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
  }

}
