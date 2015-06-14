#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
// For using the temperature sensor DS18S20
//#include <OneWire.h> 
//int DS18S20_Pin = 8; //DS18S20 Signal pin on digital 2
//OneWire ds(DS18S20_Pin);  // on digital pin 2

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

//we only need a write pipe, but am planning to use it later
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL,0xF0F0F0F0D2LL };

// here we can send up to 30 chars
char SendPayload[31];

void setup(void) {
  Serial.begin(57600); //Debug 
  printf_begin();
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

void loop() {
  
  //Get temperature from sensor
    float temperature = 12.2;
  // Assign temperature to payload, here am sending it as string
  dtostrf(temperature,2,2,SendPayload);
  
  //add a tag
  strcat(SendPayload, " deg Farh");   // add first string

  //send a heartbeat
  radio.stopListening();
  bool ok = radio.write(&SendPayload,strlen(SendPayload));
  radio.startListening(); 
  //Serial.println(SendPayload);  

  // slow down a bit
  delay(1000);  
}





