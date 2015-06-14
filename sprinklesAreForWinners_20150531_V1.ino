/************************************************************************* 
@Author      :  Timothy Nehring
@Name        :  sprinklesAreForWinners.ino
@Description :  The sketch gathers data from the sensors, packages the data
                into a payload string and then sends the data to via NRF24
                radio chip.
**************************************************************************/


#include <SPI.h>
#include "DHT.h"
#include "nRF24L01.h"
#include "RF24.h"
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 

DHT dht(DHTPIN, DHTTYPE);


//for nrf24 debug
int serial_putc( char c , FILE * ) 
{
  Serial.write( c );
  return c;
} 

//for nrf24 debug
void printf_begin(void)
{
  fdevopen( &serial_putc , 0 );
}

//nRF24 set the pin 9 to CE and 10 to CSN/SS
// Cables are:
//     SS       -> 10
//     MOSI     -> 11
//     MISO     -> 12
//     SCK      -> 13

RF24 radio( 9 , 10 );

//setup communication pipes
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL , 0xF0F0F0F0D2LL };

//set up the payload strings
char payload[31];  //= "TEST1";
char tempStr[10] ; //= "TEST1";
char received[31]; //= "TEST1";

void setup(void) 
{
  Serial.begin( 9600 );
  printf_begin();
  rf_setup();
}

void loop() 
{
//   bool start_flag = false;
//   while( !start_flag )
//   {
//     start_flag = listenForPi();
//   }
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
  
  //radio.enableAckPayload();
  radio.setPayloadSize(32);
  radio.openReadingPipe(1,pipes[0]);
  radio.openWritingPipe(pipes[1]);
  radio.startListening();
  //radio.printDetails(); //for Debugging
}

void read_and_send()
{
  delay(1000);  //Get temperature from sensor
  
  // Read temperature as Fahrenheit
  
  float fahren = dht.readTemperature( true );
  Serial.print("\n");
  Serial.print("Temp: ");
  Serial.println(fahren);
  Serial.print("\n");
  
  float humid = dht.readHumidity();
  Serial.print("humid: ");
  Serial.println(humid);
  Serial.print("\n");
  
  float audio = analogRead(A0);
  Serial.print("audio: ");
  Serial.println(audio);
  Serial.print("\n");
  
  //Take each sensor value then sandwich it between
  //a 'tag' then load it onto the payload
  
  //Temp
  strcat( payload , "T" );
  dtostrf( fahren , 2 , 2 , tempStr );
  strcat( payload , tempStr );
  strcat( payload , "T" );
//  Serial.print("payload1:");
//  Serial.println(payload);
//  Serial.print(" \n");
  
  //Humidity
  strcat( payload , "H");
  dtostrf( humid , 2 , 2 , tempStr);
  strcat( payload , tempStr );
  strcat( payload , "H" );
//  Serial.print("payload2:");
//  Serial.println(payload);
//  Serial.print(" \n");
  
  //Sound
  strcat( payload , "dB" );
  dtostrf( audio , 2 , 2 , tempStr);
  strcat( payload , tempStr );
  strcat( payload , "dB" );
//  Serial.print("payload3:");
//  Serial.println(payload);
//  Serial.print(" \n");
  
  //send the payload
  radio.stopListening();
  bool okSend = radio.write( &payload , strlen(payload) );       
  if (okSend)
  {
    Serial.print("\n");
    Serial.print("sent");
  }
  radio.startListening();
   
}








bool listenForPi()
{
 
  if( radio.available() )
  {
    
    bool done = false;
    while( !done )
    {
      done = radio.read( &received , sizeof(received) );
      //Serial.print(received);
    }
    
    if( strcmp( received , "HELLO" ) == 0)
    {
      Serial.print( "got message \n" );
      Serial.println( received );
      radio.stopListening();
      bool okSend = radio.write( &received , sizeof( received ) );
      //radio.writeAckPayload( 1 , &received , sizeof(received) );
      if (okSend)
      {
        Serial.print( "returned hello \n");
        
      }
      radio.startListening();
      return true;
    }
    else
    {
      return false;
    }
  } 
}




