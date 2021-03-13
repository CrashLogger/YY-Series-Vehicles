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

  Serial.begin(9600);
  Serial.println("start");
  ESC.attach(3, 1000, 2000);
  Elevator.attach(A0, 1000, 2000);
  pinMode(9, OUTPUT);
  analogWrite(9, 200);
  delay(500);
  analogWrite(9, 0);
  
  // Initialise the SPI bus transciever
  if (!radio.begin()) {

    while (1) {    analogWrite(9, 130);
    delay(100);
    analogWrite(9, 0);
    delay(1000);
    } // hold in infinite loop
  }

  radioNumber = 1;
    Serial.println("2");

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

  //Optimise sending time by setting the maximum to the variables maximum
  radio.setPayloadSize(sizeof(data));
  
  //Sets the pipes in the receiver order, opposite to the transmitter order.
  radio.openWritingPipe(address[radioNumber]);
  radio.openReadingPipe(1, address[!radioNumber]);
  radio.startListening();
}

void loop() {
    Serial.println("5");
    uint8_t pipe;
    if (radio.available(&pipe)) {                 // Check if there's a package waiting
      uint8_t bytes = radio.getPayloadSize();     // Read the package size
      radio.read(&data, bytes);                   // Read the package
      ESC.write(data.thrust);
      Elevator.write(data.elevator);
      Serial.print("Thrust:");
      Serial.println(data.thrust);
      if (data.buzzer){
        Buzz();
      }
      else{
        analogWrite(9, 0);
      }
   }
}

void Buzz(){
  unsigned long tempo = millis();
  if (tempo % 250){
    buzzerState = !buzzerState;
  }
  digitalWrite(9, buzzerState);
}
