/* SEEDIOT MAIN STATION V0.2
 * THIS CODE COLLECTS DATA FROM NRF24L01s
 * IN THIS CASE, THERE ARE 2 TRANSMITTERS.
 * AFTER RECEIVING DATAS, IT SENDS THEM TO
 * THINGSPEAK.
 * EMRAH ANIL
 * 22.12.2018
 */

#include <SoftwareSerial.h>
#include "nRF24L01.h" // NRF24L01 library created by TMRh20 https://github.com/TMRh20/RF24
#include "RF24.h"
#include "SPI.h"

//Arduino pins for ESP usage
#define RX 2
#define TX 3

//IDs of Nodes
#define FIRST_NODE_ID 0
#define SECOND_NODE_ID 1

//Fault or success LED pins so we can understand if our
//process finished successfully
#define FaultPin 4 
#define SuccessPin 5

// Used to store value received by the NRF24L01
int ReceivedMessage[3]; 

// NRF24L01 used SPI pins + Pin 9 and 10 on the UNO
RF24 radio(9,10); 

// Needs to be the same for communicating between 2 NRF24L01 
const uint64_t pipe = 0xE6E6E6E6E6E6; 

//ESP8266 settings
String HOST = "184.106.153.149";//Thingspeak IP
String PORT = "80";//Thingspeak Port
String AP = "CENSORED"; //Wifi Name
String PASS = "CENSORED"; //Wifi Password
String API = "CENSORED"; //API

//Thingspeak field names
String field1 = "field1";
String field2 = "field2";
String field3 = "field3";
String field4 = "field4";

int countTrueCommand;
int countTimeCommand;

boolean found = false;

//Sensor values
int tempSensor1;
int humSensor1;
int tempSensor2;
int humSensor2;

String getData1;

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
  
  String readDat="GET https://api.thingspeak.com/channels/9/feeds.json?...";
  
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

     getData1 = "GET /update?api_key="+ API +"&"+ field1 +"="+String(tempSensor1) +
                      "&"+ field2 +"="+String(humSensor1) +
                      "&"+ field3 +"="+String(tempSensor2) + 
                      "&"+ field4 +"="+String(humSensor2);
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
    
      Serial.print("Temp1: ");
      Serial.println(tempSensor1);
      Serial.print("Hum1: ");
      Serial.println(humSensor1);
      Serial.print("Temp2: ");
      Serial.println(tempSensor2);
      Serial.print("Hum2: ");
      Serial.println(humSensor2);
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
    radio.read(&ReceivedMessage, sizeof(ReceivedMessage)); // Read information from the NRF24L01

      if (ReceivedMessage[2] == FIRST_NODE_ID) {
        tempSensor1 = ReceivedMessage[0];
        humSensor1 = ReceivedMessage[1] * 10;
      }
      else if (ReceivedMessage[2] == SECOND_NODE_ID) {
        tempSensor2 = ReceivedMessage[0];
        humSensor2 = ReceivedMessage[1] * 10; 
      }
      
      Serial.print("MES: TEMP1[");
      Serial.print(tempSensor1);
      Serial.print("] HUM1[");
      Serial.print(humSensor1);
      Serial.print("] TEMP2[");
      Serial.print(tempSensor2);
      Serial.print("] HUM2[");
      Serial.print(humSensor2);
      Serial.print("]");
      Serial.println(" ");
      digitalWrite(SuccessPin,HIGH);
      delay(400);
      digitalWrite(SuccessPin,LOW); 
      delay(10);
  }
}
