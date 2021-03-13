/*
 * 
 * An NRF24L01 based RC transmitter for RC vehicles
 * An NRF24L01 PA/LNA module is used on this side
 * Designed to run a BLDC motor and 3 servos in it's original form (for rudder and elevator controls)
 * 
 * First created for the YY Series fixed wing aircraft
 * 
 * Erlantz 'CrashLogger' Alonso, 2021
 * 
*/
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

int joyX1 = A3;
int joyY1 = A4;
int joyX = A0;
int joyY = A1;
int joySw1 = 9;
int joySw = 8;

int redLED = 5;
int blueLED = 3;
int greenLED = 4; 

//Creates object
RF24 radio(6, 7); //CE, CSN

struct Package{
  int thrust;
  int rudder;
  int elevator;
  int aileron;
  bool brake;
  bool buzzer;
};

Package data;

uint8_t address[][6] = {"000FA", "000FF"};

bool radioNumber = 0; // 0 uses address[0] to transmit, 1 uses address[1] to transmit

void setup() {

  data.thrust = 0;
  data.rudder = 0;
  data.elevator = 0;
  data.aileron = 0;
  data.brake = false;
  
  pinMode(joySw, INPUT_PULLUP);
  pinMode(joySw1, INPUT_PULLUP);
  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);
  pinMode(joyX1, INPUT);
  pinMode(joyY1, INPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  if (!radio.begin()) {
    while (1) {} //Holds for checking
    RadioFail();
  }

  radioNumber = 0;
  radio.setPALevel(RF24_PA_HIGH);  // RF24_PA_MAX
  //OPTIONS ARE: "MIN" "LOW" "HIGH" "MAX"
  
  //Sets size of payload for reliability
  radio.setPayloadSize(sizeof(Package));

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[radioNumber]);
  
  //Sets the RX address of the TX node into an RX pipe for acknowledgements
  radio.openReadingPipe(1, address[radioNumber + 1]);
  //Puts radio in TX mode
  radio.stopListening();

  digitalWrite(greenLED, HIGH);
  digitalWrite(blueLED, HIGH);
  digitalWrite(redLED, HIGH);
  delay(50);
  digitalWrite(redLED, LOW);
  delay(50);
  digitalWrite(redLED, HIGH);
  delay(50);
  digitalWrite(redLED, LOW);
  delay(100);
  digitalWrite(blueLED, LOW);

}

void RadioFail(){
  digitalWrite(redLED, HIGH);
  delay(250);
  digitalWrite(redLED, LOW);
  delay(125);
  digitalWrite(redLED, HIGH);
  delay(50);
  digitalWrite(redLED, LOW);
  delay(250);
}

void loop() {
    unsigned long start_timer = micros();
    bool report = radio.write(&data, sizeof(Package));
    unsigned long end_timer = micros();
      data.thrust = map(analogRead(joyX), 0, 1023, 180, 0);
      data.elevator = map(analogRead(joyX1), 0, 1023, 180, 0);
      data.rudder = map (analogRead(joyY), 0, 1023, 0, 180);
      data.aileron = map (analogRead(joyY1), 0, 1023, 0, 180);
      data.brake = !digitalRead(joySw);
      data.buzzer = !digitalRead(joySw1);
      //analogWrite(3, data.thrust);
      //analogWrite(5, data.elevator);


    if (report) {
      digitalWrite(greenLED, HIGH);
      if ((start_timer - end_timer) > 500000){
        digitalWrite(blueLED, HIGH);
      }
      else{
        digitalWrite(blueLED, LOW);
      }
    } else {
       digitalWrite(greenLED, LOW);
       RadioFail();
    }
    //delay(10);
}
