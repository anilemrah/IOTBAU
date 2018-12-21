#include <SoftwareSerial.h>
//Include needed Libraries at beginning
#include "nRF24L01.h" // NRF24L01 library created by TMRh20 https://github.com/TMRh20/RF24
#include "RF24.h"
#include "SPI.h"

#define DEBUG true
#define RX 2
#define TX 3

#define FaultPin 4 // Arcade switch is connected to Pin 8 on NANO
#define SuccessPin 5 // Arcade switch is connected to Pin 8 on NANO

int ReceivedMessage[2]; // Used to store value received by the NRF24L01

RF24 radio(9,10); // NRF24L01 used SPI pins + Pin 9 and 10 on the UNO

const uint64_t pipe = 0xE6E6E6E6E6E6; // Needs to be the same for communicating between 2 NRF24L01 

int tempPin=0;
int humPin=1;

String HOST = "184.106.153.149";
String PORT = "80";
String AP = "CENSORED"; //Wifi Name
String PASS = "CENSORED"; //Wifi Password
String API = "CENSORED"; //API

String field1 = "field1";
String field2 = "field2";
String field3 = "field3";
String field4 = "field4";

int countTrueCommand;
int countTimeCommand;

boolean found = false;

int tempSensor;
int humSensor;

SoftwareSerial esp8266(RX,TX);

void setup() {

  Serial.begin(115200);
  esp8266.begin(115200);

  pinMode(FaultPin, OUTPUT);
  pinMode(SuccessPin, OUTPUT);

  radio.begin(); // Start the NRF24L01

  Serial.println("RX STARTED");

  radio.openReadingPipe(1,pipe); // Get NRF24L01 ready to receive

  radio.startListening(); // Listen to see if information received
  
  Serial.println("AT+CWJAP=\""+AP+"\",\""+PASS+"\"");
 
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+AP+"\",\""+PASS+"\"",15,"OK");
 
  countTrueCommand = 0;

}

void loop() {

  humSensor = 900;
  
  String readDat="GET https://api.thingspeak.com/channels/9/feeds.json?...";
  String getData1; /*= "GET /update?api_key="+ API +"&"+ field1 +"="+String(tempSensor) +
                      "&"+ field2 +"="+String(humSensor) +
                      "&"+ field3 +"="+String(tempSensor + 1) + 
                      "&"+ field4 +"="+String(humSensor + 1);*/
  
  switch(countTrueCommand) {
  
    case 0: 
      
      sendCommand("AT",5,"OK");
      break;
    
    case 1: 
     
      sendCommand("AT+RST",10,"OK");
      break;
    
    case 2: 
    
      sendCommand("AT+CIPMUX=1",5,"OK"); 
      break;
    
    case 3: 
      
      sendCommand("AT+CIPSTART=\"TCP\",\""+ HOST +"\","+ PORT,15,"OK"); 
      break;
    
    case 4: 

     readSensorDatas();

     getData1 = "GET /update?api_key="+ API +"&"+ field1 +"="+String(tempSensor) +
                      "&"+ field2 +"="+String(humSensor) +
                      "&"+ field3 +"="+String(tempSensor + 1) + 
                      "&"+ field4 +"="+String(humSensor + 1);
    countTrueCommand++;
    break;
   
    case 5: 
      
      sendCommand("AT+CIPSEND=" +String(getData1.length()+4),4,">"); 
      break;
    
    case 6: 

      esp8266.println(getData1);
      delay(1500);
      countTrueCommand++; 
      break;
    
    case 7: 
    
      sendCommand("AT+CIPCLOSE=0",5,"OK"); 
      break;
    
    case 8:
    
      Serial.print("Temp: ");
      Serial.println(tempSensor);
      Serial.print("Hum: ");
      Serial.println(humSensor);
      Serial.print(getData1);
      Serial.print(",");
      Serial.println(getData1.length());
      
      countTrueCommand = 0;
      
      delay(10000);
      break;
  }
}

void sendCommand(String command, int maxTime, char readReplay[]) {

  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");

  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
    countTimeCommand++;
  }
  
  if(found == true)
  
  {
    Serial.println("OK");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
  
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  found = false;
}

void readSensorDatas() {

if (radio.available())
  {
    radio.read(ReceivedMessage, 2); // Read information from the NRF24L01

      tempSensor = ReceivedMessage[0];
      humSensor = 1000;
    //if (ReceivedMessage[0] == 025) // Indicates switch is pressed
    //{
      Serial.print("MES RCVD: ");
      Serial.print(tempSensor);
      Serial.println(" ");
      digitalWrite(SuccessPin,HIGH); // Set Pin to HIGH at beginning
      delay(400);
      digitalWrite(SuccessPin,LOW); // Set Pin to HIGH at beginning

    /*}
    else
    {
      Serial.println("MSG IS WRONG");
      digitalWrite(FaultPin,HIGH); // Set Pin to HIGH at beginning
      delay(400);
      digitalWrite(FaultPin,LOW); // Set Pin to HIGH at beginning
      tempSensor = 22;
      humSensor = 900;
    }*/
    delay(10);
  }
}
