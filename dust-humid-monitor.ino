
#include <I2CIO.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"


#define I2C_ADDR    0x3F
LiquidCrystal_I2C lcd(0x3f,2,1,0,4,5,6,7,3,POSITIVE);

int measurePin = 0;
int ledPower = 8;

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

// INIT DHT sensor 
#define DHTPIN 2
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);

#define MICPIN A1


void setup(){
  Serial.begin(9600);
  pinMode(ledPower,OUTPUT);

  lcd.begin (16,2); 
  lcd.setBacklight(HIGH);
  lcd.home (); // go home

  dht.begin();
  
}


void loop(){
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin); // read the dust value
  
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);

  // 0 - 5.0V mapped to 0 - 1023 integer values 
  calcVoltage = voMeasured * (5.0 / 1024); 
  
  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = (0.172 * calcVoltage - 0.0999) * 1000; 
  
  //Serial.print("Raw Signal Value (0-1023): ");
  //Serial.print(voMeasured);
  
  //Serial.print(" - Voltage: ");
  //Serial.print(calcVoltage);
  
  //Serial.print(" - Dust Density [ug/m3]: ");
  Serial.println(dustDensity);

  lcd.setCursor(0,0);

  lcd.print("Voltage: ");
  lcd.print(calcVoltage);

  lcd.setCursor(0,1);
  lcd.print("Dust(ug/m3):");
  lcd.print(dustDensity);
  
  delay(2000);
  lcd.clear();

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    lcd.print("TMPERR"); 
    return;
  }
  lcd.setCursor(0,0);
  lcd.print("Humidity: ");
  lcd.print(h);
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print("C");
  
  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  int val = analogRead(MICPIN);
  lcd.print("Vol: ");
  lcd.print(val) ;
  delay(2000);

  
  
}
