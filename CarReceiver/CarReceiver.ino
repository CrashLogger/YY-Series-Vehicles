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

RF24 radio(7, 8); // using pin 7 for the CE pin, and pin 8 for the CSN pin

uint8_t address[][6] = {"000FA", "000FF"};    //REC, ACK
bool radioNumber = 1; //Receiver will use pipe [1] to transmit acknowledgements

  
  int powerR = 90;
  int powerL = 90;

void setup() {
  ////Serial.begin(9600);
  pinMode(10, OUTPUT); //EN  1
  pinMode(3, OUTPUT);  //EN  2
  pinMode(4, OUTPUT);  //Pos 1
  pinMode(5, OUTPUT);  //Neg 1
  pinMode(6, OUTPUT);  //Pos 2
  pinMode(9, OUTPUT);  //Neg 2
  pinMode(2, OUTPUT);  //Neg 2

  digitalWrite(5, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(3, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(6, LOW);
  digitalWrite(10, HIGH);
  delay(1000);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(3, LOW);
  digitalWrite(6, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  delay(1000);
  digitalWrite(5, LOW);
  digitalWrite(4, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(9, LOW);
  digitalWrite(6, HIGH);
  digitalWrite(10, HIGH);
  delay(1000);

  // Initialise the SPI bus transciever
  if (!radio.begin()) {
    ////Serial.println(F("radio hardware is not responding!!"));
    while (1) {} // hold in infinite loop
  }

  radioNumber = 1;
  radio.setPALevel(RF24_PA_HIGH);  // RF24_PA_MAX is default.
  //Optimise sending time by setting the maximum to the variables maximum
  radio.setPayloadSize(sizeof(data));
  
  //Sets the pipes in the receiver order, opposite to the transmitter order.
  radio.openWritingPipe(address[radioNumber]);
  radio.openReadingPipe(1, address[!radioNumber]);
  radio.startListening();
}

void Right(){
  if (powerR > 160){
    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);
    if (powerR > 170){
      analogWrite(3,255);
    }
    else{
      analogWrite(3, 255);
    }
  }
  else if(powerR < 20){
    digitalWrite(4, HIGH);
    digitalWrite(5, LOW);
    if (powerR < 10){
      analogWrite(3,255);
    }
    else{
      analogWrite(3, 255);
    }    
  }
  else{
    analogWrite(3, 0);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
  }
}

void Left(){
  if (powerL > 160){
    digitalWrite(6, LOW);
    digitalWrite(9, HIGH);
        if (powerR > 170){
      analogWrite(3,255);
    }
    else{
      analogWrite(10, 255);
    }   
  }
  else if (powerL < 20){
    digitalWrite(6, HIGH);
    digitalWrite(9, LOW);
    if (powerL < 10){
      analogWrite(10,255);
    }
    else{
      analogWrite(10, 255);
    }   
  }
  else{
    analogWrite(10, 0);
    digitalWrite(6, LOW);
    digitalWrite(9, LOW);
  }
}
void loop() {
    uint8_t pipe;
    if (radio.available(&pipe)) {                 // Check if there's a package waiting
      uint8_t bytes = radio.getPayloadSize();     // Read the package size
      radio.read(&data, bytes);                   // Read the package
      powerL = map(data.thrust, 0, 180, 0, 255);
      powerR = map(data.elevator, 0, 180, 0, 255);
      digitalWrite(2, data.buzzer);
   }
   
  Right();
  Left();
}
