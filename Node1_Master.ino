/*
Created by Yvan / https://Brainy-Bits.com

This code is in the public domain...
You can: copy it, use it, modify it, share it or just plain ignore it!
Thx!

*/

// NRF24L01 Module Tutorial - Code for Transmitter using Arduino NANO

//Include needed Libraries at beginning
#include "nRF24L01.h" //NRF24L01 library created by TMRh20 https://github.com/TMRh20/RF24
#include "RF24.h"
#include "SPI.h"

#define SwitchPin 8 // Arcade switch is connected to Pin 8 on NANO
#define FaultPin 4 // Arcade switch is connected to Pin 8 on NANO
#define SuccessPin 5 // Arcade switch is connected to Pin 8 on NANO

int tempPin=0;
int tempSensor;

int SentMessage[2]; // Used to store value before being sent through the NRF24L01

RF24 radio(9,10); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO

const uint64_t pipe = 0xE6E6E6E6E6E6; // Needs to be the same for communicating between 2 NRF24L01 


void setup(void){

Serial.begin(9600);

pinMode(SwitchPin, INPUT_PULLUP); // Define the arcade switch NANO pin as an Input using Internal Pullups
digitalWrite(SwitchPin,HIGH); // Set Pin to HIGH at beginning

pinMode(FaultPin, OUTPUT);
pinMode(SuccessPin, OUTPUT);

Serial.println("TX Started");
radio.begin(); // Start the NRF24L01
radio.openWritingPipe(pipe); // Get NRF24L01 ready to transmit
}

void loop(){

  tempSensor=analogRead(tempPin); // Reading data from the temperatur sensor.
  Serial.print("Sensor: ");
  Serial.println(tempSensor);
  tempSensor=(5.0*tempSensor*100.0)/(1024*10);
  Serial.print("Sensor2: ");
  Serial.println(tempSensor);

if (digitalRead(SwitchPin) == LOW){ // If Switch is Activated*/
  
SentMessage[0] = tempSensor;
SentMessage[1] = 100;

radio.write(SentMessage, 2); // Send value through NRF24L01
Serial.print("YES: ");
digitalWrite(SuccessPin,HIGH); // Set Pin to HIGH at beginning
delay(400);
digitalWrite(SuccessPin,LOW); // Set Pin to HIGH at beginning
}
else {
SentMessage[0] = 000;
SentMessage[1] = 000;

radio.write(SentMessage, 2);
Serial.println("NO");
digitalWrite(FaultPin,HIGH); // Set Pin to HIGH at beginning
delay(400);
digitalWrite(FaultPin,LOW); // Set Pin to HIGH at beginning

}
}
