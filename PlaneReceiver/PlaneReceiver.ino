/*
 * 
 * An NRF24L01 based RC receiever for servo-controlled parts
 * A standard short pcb laid antenna NRF24L01 is used on this side
 * Designed to run a BLDC motor and 3 servos in it's original form (for rudder and elevator controls)
 * 
 * First created for the YY-120
 * 
 * Erlantz 'CrashLogger' Alonso, 2021
 * 
*/
#include <SPI.h>
#include "printf.h"
#include "RF24.h"
#include <Servo.h>

Servo ESC;
Servo Elevator;
Servo RudderR;
Servo RudderL;
//Servo RudderR;
//Servo RudderL;

bool buzzerState = false;

struct Package{
  int thrust;
  int rudder;
  int elevator;
  int aileron;
  bool brake;
  bool buzzer;  
};
Package data;

RF24 radio(7, 5);
uint8_t address[][6] = {"000FA", "000FF"};    //REC, ACK
bool radioNumber = 1; // 0 uses address[0] to transmit, 1 uses address[1] to transmit

void setup() {

  //Serial.begin(9600);
  ESC.attach(3, 1000, 2000);
  RudderR.attach(A3);
  RudderL.attach(A2);
  Elevator.attach(A0);
  
  // Initialise the SPI bus transciever
  if (!radio.begin()) {

    while (1) {
    delay(1000);
    } // Hold in infinite loop if it cannot connect to the controller
  }

  radio.setPALevel(RF24_PA_HIGH);  // RF24_PA_MAX is default.

  //Optimise sending time by setting the maximum to the variables maximum
  radio.setPayloadSize(sizeof(data));
  
  //Sets the pipes in the receiver order, opposite to the transmitter order.
  radio.openWritingPipe(address[radioNumber]);
  radio.openReadingPipe(1, address[!radioNumber]);
  radio.startListening();
}

void loop() {
    uint8_t pipe;
    if (radio.available(&pipe)) {                 // Check if there's a package waiting
      uint8_t bytes = radio.getPayloadSize();     // Read the package size
      radio.read(&data, bytes);                   // Read the package

      Power();
      //Serial.print(F(","));
      Rudders();
      //Serial.print(F(","));
      Elevators();
      //Serial.println();
   }
}

void Power(){
  int thrust = map (data.thrust, 1023, 512, 0, 175);
  if (data.thrust < 480){
    ESC.write(thrust);
    //Serial.print(F("Thrust:"));
    //Serial.print(thrust);
  }
  else{
    ESC.write(0);
    //Serial.print(F("0"));
  }
}

void Rudders(){
  int rudderAngle = map(data.rudder, 0, 1023, 45, 135);
  RudderR.write(rudderAngle);
  RudderL.write(rudderAngle);
  //Serial.print(F("Rudders:"));
  //Serial.print(rudderAngle);
}

void Elevators(){
  int elevatorAngle = map(data.elevator, 0, 1023, 40, 140);
  Elevator.write(elevatorAngle);
  //Serial.print(F("Elevator:"));
  //Serial.print(elevatorAngle);
}
