#include <SoftwareSerial.h>

#define DEBUG true
#define RX 2
#define TX 3

int tempPin=0;
int humPin=1;

String HOST = "184.106.153.149";
String PORT = "80";
String AP = "CENSORED"; //Wifi Name
String PASS = "CENSORED"; //Wifi Password
String API = "CENSORED"; //API

String field1 = "field1";
String field2 = "field2";

int countTrueCommand;
int countTimeCommand;

boolean found = false;

int tempSensor;
int humSensor;

SoftwareSerial esp8266(RX,TX);

void setup() {

  Serial.begin(115200);
  esp8266.begin(115200);
  Serial.println("AT+CWJAP=\""+AP+"\",\""+PASS+"\"");
 
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+AP+"\",\""+PASS+"\"",15,"OK");
 
  countTrueCommand = 0;

}

void loop() {

  tempSensor=analogRead(tempPin); // Reading data from the temperatur sensor.
  tempSensor=(5.0*tempSensor*100.0)/(1024*10);
  humSensor=analogRead(humPin); // Reading data from the humidity sensor.
  
  String readDat="GET https://api.thingspeak.com/channels/9/feeds.json?...";
  String getData1 = "GET /update?api_key="+ API +"&"+ field1 +"="+String(tempSensor)+"&"+ field2 +"="+String(humSensor);
  
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
      
      sendCommand("AT+CIPSEND=" +String(getData1.length()+4),4,">"); 
      break;
    
    case 5: 

      esp8266.println(getData1);
      delay(1500);
      countTrueCommand++; 
      break;
    
    case 6: 
    
      sendCommand("AT+CIPCLOSE=0",5,"OK"); 
      break;
    
    case 7:
    
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
