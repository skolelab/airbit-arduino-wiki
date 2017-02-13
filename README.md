# Sensorkit for måling av luftkvalitet
I denne guiden vil dere finne den kildekoden dere trenger for å
programmere settene til å måle posisjon, tidspunkt, mengde svevestøv i lufta,
temperatur og luftfuktighet. Alle disse målingene skal dere skrive til et
minnekort og laste opp på egenhånd. 

I gjennomgangen under kommer vi til å først hente ut data fra de forskjellige
komponentene og skrive de ut på datamaskinen. Når vi har verifisert at
komponentene fungerer kommer vi til å skrive de til et minnekort slik at dere
kan gå ut og logge data! 

## Sensorene
Her har dere en oversikt over alle komponentene som sitter i settet og hvor dere
kan finne mer informasjon: 

- [Arduino Uno](https://www.arduino.cc/en/Main/ArduinoBoardUno).
- [Sharp GP2Y1010AU0F (støvsensor)](https://www.sparkfun.com/datasheets/Sensors/gp2y1010au_e.pdf).
- [DHT11 (temperatur + luftfuktighet, den dere får manger pin nr 3.)](http://www.micropik.com/PDF/dht11.pdf).
- [Catalex MicroSD Card Adapter (minnekortleser)](http://www.monocilindro.com/tag/catalex-microsd-adapter/).
- [U-blox NEO-6 GPS Module (gps-modul)](https://www.u-blox.com/sites/default/files/products/documents/NEO-6_DataSheet_%28GPS.G6-HW-09005%29.pdf?utm_source=en%2Fimages%2Fdownloads%2FProduct_Docs%2FNEO-6_DataSheet_%28GPS.G6-HW-09005%29.pdf).

## Vidre gjennomgang 
Vidre nedover kommer dere til å skrive egen kode. Det lureste kan være å ha en
sketch per steg, men det ser dere helt selv. I steg 5 kommer dere til å sette
alt sammen. 


## Steg 0: Skru av og på de to indikatorlysene
Det første vi gjør er å sjekke at vi klarer å skru av og på de to
indikatorlysene. Disse skal vi senere bruke til å se at Arduinoen har fått et
GPS-signal og dermed logger data. 

Under ser dere kode for å skru av og på en LED som er koblet i digital pin 6.
Dere skal se at koden fungerer og legge til støtte for å skru av og på digital
pin 7 hvor den andre LED-en sitter. 

```c
static const int GPS_OK_PIN = 6;

void setup() {
  pinMode(GPS_OK_PIN, OUTPUT);
}

void loop() {
  digitalWrite(GPS_OK_PIN, HIGH); 
  delay(1000);
  digitalWrite(GPS_OK_PIN, LOW);  
  delay(1000); 
}
```


## Steg 1: Temperatur og Luftfuktighet 
Det første vi skal gjøre er å hente ut temperaturen og luftfuktigheten. Dette
kommer vi til å gjøre med `DHT`-biblioteket som ligger tilgjengelig. Hvis dere
ikke har installert biblioteket kan dere i Arduino IDE-en gå til `Sketch ->
Include Library -> Manage Libraries ...` og installere det. 

Under har dere koden for å lese inn temperatur og luftfuktighet, og skrive de
begge til datamaskinen når Arduinoen er koblet til. 

```c
#include "DHT.h"

static const int DHT_PIN = 2
DHT dht(DHTPIN, DHT11);

void setup(){
  Serial.begin(9600);    
  dht.begin();
}

void loop(){
  Serial.print(" Humidity: ");
  Serial.print(dht.readHumidity());
  Serial.print(",");

  Serial.print(" Temperature: ");
  Serial.print(dht.readTemperature());
  Serial.print("\n");
}

```


## Steg 2: Svevestøv
Nå skal vi se om vi klarer å måle mengden støvpartikler i lufta. Koden nedenfor
er litt vanskelig å forstå seg på og bygger på [dataarket her](http://www.sharp-world.com/products/device-china/lineup/data/pdf/datasheet/gp2y1010au_appl_e.pdf)
og 
[denne guiden](http://arduinodev.woofex.net/2012/12/01/standalone-sharp-dust-sensor/). 

```c
static const int DUST_PIN = 0;
static const int DUST_LED_PIN = 8;

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;  
float calcVoltage = 0;
float dustDensity = 0;


void setup() {
  pinMode(DUST_POWER_PIN, OUTPUT);
  pinMode(DUST_PIN, INPUT);
}

void loop() {
    
    digitalWrite(DUST_POWER_PIN, LOW);  // power on the LED inside the dust
                                        // sensor
    delayMicroseconds(samplingTime);
    voMeasured = analogRead(DUST_PIN);  // read the dust value
    
    delayMicroseconds(deltaTime);
    digitalWrite(DUST_POWER_PIN,HIGH); // turn the LED off
    delayMicroseconds(sleepTime);

    calcVoltage = voMeasured * (5.0 / 1024);
    dustDensity = (0.17 * calcVoltage - 0.0999999) * 1000;
    
    if (dustDensity < 0) {
      dustDensity = 0;
    }
    
    Serial.print("Dust density (ug/m3):") 
    Serial.print(dustDensity);
    Serial.print("\n");
}
```

## Steg 3: GPS 
Denne er litt vanskeligere å teste siden dere må utendørs (evt. utfor vinduet),
så ta det en dag det ikke regner eller snør så mye. For å kommunisere med
GPS-modulen trenger vi et bibliotek (slik at vi ikke trenger å skrive så mye
kode). Dette finner dere [her](arduiniana.org/libraries/tinygpsplus/) og må
installeres manuelt. Dere trenger også et bibliotek som heter "Adafruit unified
sensor", men dette finner dere på vanlig måte i Arduino IDE-en. 

```c
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <stdlib.h>

static const int RX_PIN = 4;
static const int TX_PIN = 3;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RX_PIN, TX_PIN);

void setup() {

}

void loop() {
    while (ss.available() > 0) { 
        if (gps.encode(ss.read())){
            if(gps.location.isValid()){
                
                // Date
                Serial.print("Date: ")
                Serial.print(gps.date.day());
                Serial.print("/");
                Serial.print(gps.date.month());
                Serial.print("/");
                Serial.print(gps.date.year());
                Serial.print(" ");
                Serial.print(gps.time.hour());
                Serial.print(":");
                Serial.print(gps.time.minute());
                Serial.print(":");
                Serial.print(gps.time.second());
                Serial.print(","); 

                 // Latitude. 
                 // Note that we convert the float output to a string. This is
                 // for convenience when we're later going to write it to a file
                 // on a SD memory card. 
                 
                 char lat[15];
                 dtostrf(gps.location.lat(), 3, 6, lat);
                 
                 Serial.print(" Latitude: ")
                 Serial.print(lat);
                 Serial.print(",");
                 
                 // long
                 char lng[15];
                 dtostrf(gps.location.lng(), 3, 6, lng);
                 Serial.print(" Longitude: ")
                 Serial.print(lng);
                 Serial.print(",");


                
            } else { 
                Serial.print("Bad GPS signal.") 
            }
        }
    }

}

```


## Steg 4: Minnekort

```c
#include <SD.h>

static const int SD_PIN = 10;
File file;

static const int i = 0;

void setup() {
    pinMode(SD_PIN, OUTPUT);
    SD.begin();
    char filename[] = "fil.txt";

    // If file exsist on the memory card, append to it, if it doesn't exist 
    // create it and write one line to the file before continuing. 
    
    if(SD.exists(filename)){
        file.close(); 
        file = SD.open(filename, O_WRITE | O_APPEND); 
    } else {
        file = SD.open(filename, O_WRITE | O_CREAT); 
        file.print("Dette er første linje. Den vil aldri bli overskrevet.\n"); 
        file.flush();
    } 
}

void loop() {
    
    file.print("Linje ");
    file.print(i);
    file.print("\n"); 

    i = i + 1;

    delay(5000); 

}

```



## Steg 5: Sett alt sammen 
Dette må dere klare selv! Husk at det er viktig at fila dere skriver er en
komma-separert fil. Dette betyr at den skal starte med en linje som ser slik ut: 

```
Time, Latitude, Longitude, Dust, Humidity, Temperature\n
```

og alle de påfølgende linjene i fila skal følge samme mønster, f.eks 

```
13/2/2017 18:10:4,69.681290,18.976048,0.00,21.00,15.00
13/2/2017 18:10:4,69.681290,18.976048,0.00,22.00,14.00
13/2/2017 18:10:13,69.681313,18.976034,0.00,23.00,14.00
13/2/2017 18:10:13,69.681313,18.976034,60.21,22.00,14.00
13/2/2017 18:10:22,69.681252,18.975937,0.00,22.00,14.00
13/2/2017 18:10:22,69.681252,18.975937,62.70,22.00,14.00
```

For å se at dere har en fil på riktig format kan dere prøve å importere den i
Excel! 

Lykke til!! 
