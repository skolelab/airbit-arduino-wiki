#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include<stdlib.h> // float to string 

// GPS things
static const int RXPin = 4;
static const int TXPin = 3;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

// sd card things
int CS_PIN = 10;
File file;

// indicator leds
static const int GPS_OK_PIN = 6;
static const int GPS_NOT_OK_PIN = 7;

void setup() {
  // init gps connection
  ss.begin(GPSBaud);


  // init indicator leds
  pinMode(GPS_OK_PIN, OUTPUT);
  pinMode(GPS_NOT_OK_PIN, OUTPUT);

  digitalWrite(GPS_OK_PIN, HIGH);
  digitalWrite(GPS_NOT_OK_PIN, HIGH);

  // init sd card 
  pinMode(CS_PIN, OUTPUT);
  SD.begin(); 

  char filename[] = "data.csv";

  // check for file. if exisist append to it, else 
  // open & write csv header 
  if(SD.exists(filename)){
    file.close(); 
    file = SD.open(filename, O_WRITE | O_APPEND); 
  }
  else{
    file = SD.open(filename, O_WRITE | O_CREAT); 
    file.println("time, lat, long"); 
    file.flush();
  } 
  
}

void loop() {
  // put your main code here, to run repeatedly:

    while (ss.available() > 0) { 
      if (gps.encode(ss.read())){
        if(gps.location.isValid()){
          digitalWrite(GPS_OK_PIN, HIGH);
          digitalWrite(GPS_NOT_OK_PIN, LOW);
          writeLocation();
        } else {
          digitalWrite(GPS_OK_PIN, LOW);
          digitalWrite(GPS_NOT_OK_PIN, HIGH);
        }
      }
    }
}


void writeLocation(){
  // time 
  file.print(gps.date.day());
  file.print("/");
  file.print(gps.date.month());
  file.print("/");
  file.print(gps.date.year());
  file.print(" ");
  file.print(gps.time.hour());
  file.print(":");
  file.print(gps.time.minute());
  file.print(":");
  file.print(gps.time.second());
  file.print(","); 

  // lat
  char lat[15];
  dtostrf(gps.location.lat(), 3, 6, lat);
  file.print(lat);
  file.print(",");

  // long
  char lng[15];
  dtostrf(gps.location.lng(), 3, 6, lng);
  file.print(lng);
  file.print("\n");

  // ensure that data is written 
  file.flush();
  
}
