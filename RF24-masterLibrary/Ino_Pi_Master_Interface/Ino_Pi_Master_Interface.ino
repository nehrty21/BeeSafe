/******************************************************** 
@Author:  Timothy Nehring
@Name: Ino_Pi_Master_Interface.pi
@Description:  
   
**********************************************************/

#include <SPI.h>
#include "DHT.h"
#include "nRF24L01.h"
#include "RF24.h"
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 

DHT dht(DHTPIN, DHTTYPE);
// NOTE: For working with a faster chip, like an Arduino Due or Teensy, you
// might need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// Example to initialize DHT sensor for Arduino Due:
//DHT dht(DHTPIN, DHTTYPE, 30);


//for nrf24 debug
int serial_putc( char c, FILE * ) 
{
  Serial.write( c );
  return c;
} 

//for nrf24 debug
void printf_begin(void)
{
  fdevopen( &serial_putc, 0 );
}

//nRF24 set the pin 9 to CE and 10 to CSN/SS
// Cables are:
//     SS       -> 10
//     MOSI     -> 11
//     MISO     -> 12
//     SCK      -> 13

RF24 radio(9,10);

//set up a write and read pipe.  
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL,0xF0F0F0F0D2LL };

//These are the payload strings
char sendTemp[31];
char sendHumid[31];
char sendVolume[31];

void setup(void) 
{
  
  printf_begin();
  rf_setup();
}

void loop() 
{
  
  read_and_send();
  
}

void rf_setup()
{
  //nRF24 configurations
  radio.begin();
  radio.setChannel(0x4c);
  radio.setAutoAck(1);
  radio.setRetries(15,15);
  radio.setDataRate(RF24_250KBPS);
  radio.setPayloadSize(32);
  radio.openReadingPipe(1,pipes[0]);
  radio.openWritingPipe(pipes[1]);
  radio.startListening();
  radio.printDetails(); //for Debugging
}

void read_and_send()
{
  delay(2000);  //Get temperature from sensor
  
  float humid = dht.readHumidity();
  // Read temperature as Fahrenheit
  float fahren = dht.readTemperature(true);
  float audio = analogRead(A0);
  
  //Convert sensor value to string then assign to a payload
  dtostrf(fahren,2,2,sendTemp);
  dtostrf(humid,2,2,sendHumid);
  dtostrf(audio,2,2,sendVolume);
  
  //concatenate the number with a units tag
  strcat(sendTemp, " Degrees Fahrenheit");   // add first string
  strcat(sendHumid, " % Humidity");
  strcat(sendVolume, " decibels ");
  
  //send a heartbeat
  radio.stopListening();
  bool okTemp = radio.write(&sendTemp,strlen(sendTemp));
  radio.startListening(); 

  radio.stopListening();
  bool okHumid = radio.write(&sendHumid,strlen(sendHumid));
  radio.startListening();

  radio.stopListening();  
  bool okVolume = radio.write(&sendVolume,strlen(sendVolume));
  radio.startListening();  

  // slow down a bit
  delay(1000);  
}




