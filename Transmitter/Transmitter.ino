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

uint8_t turn; 

// int redLED = 5;
// int blueLED = 3;
// int greenLED = 4; 

//Creates object
RF24 radio(6, 7); //CE, CSN

struct Package{
  uint8_t control;
  uint8_t val;
};

Package data;

uint8_t address[][6] = {"gyroc", "gyroc"};

bool radioNumber = 0; // 0 uses address[0] to transmit, 1 uses address[1] to transmit

void setup() {

  Serial.begin(115200);

  // data.thrust = 0;
  // data.rudder = 0;
  // data.elevator = 0;
  // data.aileron = 0;
  // data.brake = false;

  data.control = 0;
  data.val = 0;
  
  pinMode(joySw, INPUT_PULLUP);
  pinMode(joySw1, INPUT_PULLUP);
  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);
  pinMode(joyX1, INPUT);
  pinMode(joyY1, INPUT);
  // pinMode(blueLED, OUTPUT);
  // pinMode(greenLED, OUTPUT);
  // pinMode(redLED, OUTPUT);

  if (!radio.begin()) {
    while (1) {} //Holds for checking
    Serial.println("AA");
    RadioFail();
  }
  Serial.println("Connected");
  radio.setChannel(60);

  radioNumber = 0;
  radio.setPALevel(RF24_PA_HIGH);  // RF24_PA_MAX
  //OPTIONS ARE: "MIN" "LOW" "HIGH" "MAX"
  
  //Sets size of payload for reliability
   Serial.print(F("SIZE"));
   Serial.println(sizeof(Package));
  radio.setPayloadSize(sizeof(Package));
  radio.setAutoAck(false);

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[radioNumber]);
  
  //Sets the RX address of the TX node into an RX pipe for acknowledgements
  radio.openReadingPipe(1, address[radioNumber + 1]);
  //Puts radio in TX mode
  radio.stopListening();

  // digitalWrite(greenLED, HIGH);
  // digitalWrite(blueLED, HIGH);
  // digitalWrite(redLED, HIGH);
  // delay(50);
  // digitalWrite(redLED, LOW);
  // delay(50);
  // digitalWrite(redLED, HIGH);
  // delay(50);
  // digitalWrite(redLED, LOW);
  // delay(100);
  // digitalWrite(blueLED, LOW);

}

void RadioFail(){
  // digitalWrite(redLED, HIGH);
  // delay(100);
  // digitalWrite(redLED, LOW);
  // delay(250);
  // digitalWrite(redLED, HIGH);
  // delay(100);
  // digitalWrite(redLED, LOW);
  // delay(500);
}

void loop() {
    unsigned long start_timer = micros();
    bool report = radio.write(&data, sizeof(Package));
    unsigned long end_timer = micros();

      switch(turn){
        case(0):
          Serial.println("Sending 0");
          data.control = 0b00000000;
          data.val = analogRead(joyX);
          turn++;
          break;
        case(1):
          Serial.println("Sending 1");
          data.control = 0b00000001;
          data.val = analogRead(joyY);
          turn++;
          break;
        case(2):
          Serial.println("Sending 255");
          data.control = 0b11111111;
          data.val = analogRead(joyX1);
          turn++;
          break;
        default:
          turn = 0;
          break;
      }

      // data.thrust = map (analogRead(joyX), 0, 1023, 1023, 0);
      // data.elevator = map (analogRead(joyX1), 0, 1023, 1023, 0);
      // data.rudder = analogRead(joyY);
      // data.aileron = analogRead(joyY1);
      // data.brake = !digitalRead(joySw);
      // data.buzzer = !digitalRead(joySw1);


    // if (report) {
    //   digitalWrite(greenLED, HIGH);
    //   if ((start_timer - end_timer) > 500000){
    //     digitalWrite(blueLED, HIGH);
    //   }
    //   else{
    //     digitalWrite(blueLED, LOW);
    //   }
    // } else {
    //    digitalWrite(greenLED, LOW);
    //    RadioFail();
    // }
    //delay(10);
}
