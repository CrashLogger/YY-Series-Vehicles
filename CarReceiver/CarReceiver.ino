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

  
  int powerR = 128;
  int powerL = 128;

void setup() {

  // Initialise the SPI bus transciever
  if (!radio.begin()) {
    ////Serial.println(F("radio hardware is not responding!!"));
    while (1) {} // hold in infinite loop if it can't connect to the controller
  }

  //radio.enableAckPayload();

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
  
    uint8_t pipe;
    if (radio.available(&pipe)) {                 // Check if there's a package waiting
      uint8_t bytes = radio.getPayloadSize();     // Read the package size
      radio.read(&data, bytes);                   // Read the package
      powerL = map(data.thrust, 0, 1023, 0, 9);
      powerR = map(data.elevator, 0, 1023, 0, 9);
      digitalWrite(2, data.buzzer);
   }
  digitalWrite(2, data.buzzer);
  Right();
  Left();
}

void Right(){
  digitalWrite(2, data.buzzer);
  if (powerR > 6){
    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);
    if (powerR >= 8){
      analogWrite(3, 255);
    }
    else{
      analogWrite(3, 132);
    }
  }
  else if (powerR <3){
    digitalWrite(5, LOW);
    digitalWrite(4, HIGH);
    if (powerR < 1){
      analogWrite(3, 255);
    }
    else{
      analogWrite(3,132);
    }
  }
  else{
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
  }
}
void Left(){
  digitalWrite(2, data.buzzer);  
  if (powerL > 6){
    digitalWrite(6, LOW);
    digitalWrite(9, HIGH);
    if (powerL >= 8){
      analogWrite(10, 255);
    }
    else{
      analogWrite(10, 132);
    }
  }
  else if (powerL <3){
    digitalWrite(9, LOW);
    digitalWrite(6, HIGH);
    if (powerL < 1){
      analogWrite(10, 255);
    }
    else{
      analogWrite(10,132);
    }
  }
  else{
    digitalWrite(9, LOW);
    digitalWrite(6, LOW);
  }
}
