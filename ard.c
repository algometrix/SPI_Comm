#include <Wire.h>
#include <ADXL345.h>
#include <SPI.h>

byte buf [8];
ADXL345 adxl;
volatile byte pos;
volatile byte received_data_length;
volatile byte current_message_type;
volatile boolean process_it;
boolean debug=true;
volatile char registers[10];
volatile char reg[4];
volatile bool reading;

void print_debug(String message){
  Serial.println(message);
}
char read_register(byte address){
  
  return registers[address];
}
void write_register(byte address,char value){
  
  registers[address]=value;
 
}
void process_request(){
  byte command_type,read_address,write_address,write_value;
  command_type=buf[0];
  if(0x1==command_type){
    read_address=buf[1];
    char val=read_register(read_address);
    SPDR=val;
    process_it=false;
  }
  if(0x2==command_type){
    write_address=buf[1];
    write_value=buf[2];
    write_register(write_address,write_value);
    process_it=false;
  }
 
  
}
void generate_response(){

}
__inline void handler(byte c){
  
  if (received_data_length < sizeof buf)
    {
      // if first byte of data
    if(0==received_data_length){
      current_message_type=c;
     }
    buf[received_data_length++]=c;
    //Serial.print(current_message_type);
    if(0x2==current_message_type){
      if(3==received_data_length){
        process_it=true;
      }
    }
    else if(0x1==current_message_type){
      if(2==received_data_length){
        process_it=true;
      }
    }
    else if(0x3==current_message_type){
      received_data_length=0;
    }
    else if(0x4==current_message_type){
      
      SPDR=reg[received_data_length-1];
      
      if(4==received_data_length){
        received_data_length=0;
      }
      
    }
    
  }
}
void setup (void)
{
  Serial.begin (115200);   // debugging
  reading=false;
  // turn on SPI in slave mode
  SPCR |= bit (SPE);
  SPCR |= _BV(SPIE);
  reg[0]=11;
  reg[1]=12;
  reg[2]=13;
  reg[3]=100;
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);

  // get ready for an interrupt
  pos = 0;   // buffer empty
  received_data_length=0x0;
  process_it = false;
  
  // now turn on interrupts
  SPI.attachInterrupt();
  adxl.powerOn();

  //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); //62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
 
  //look of activity movement on this axes - 1 == on; 0 == off 
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);
 
  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);
 
  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(0);
  adxl.setTapDetectionOnY(0);
  adxl.setTapDetectionOnZ(1);
 
  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(50); //62.5mg per increment
  adxl.setTapDuration(15); //625us per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment
 
  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
 
  //setting all interrupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );
 
  //register interrupt actions - 1 == on; 0 == off  
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);

}  // end of setup


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;  // grab byte from SPI Data Register
  handler(c);


}

// main loop - wait for flag set in interrupt routine
void loop (void)
{
   
   if(process_it){
      process_request();
      received_data_length=0;
      
   }
  int x,y,z;  
  adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
  reg[0]=x;
  reg[1]=y;
  reg[2]=z;
  reg[3]=100;
 
}  // end of loop
#include <Wire.h>
#include <ADXL345.h>
#include <SPI.h>

byte buf [8];
ADXL345 adxl;
volatile byte pos;
volatile byte received_data_length;
volatile byte current_message_type;
volatile boolean process_it;
boolean debug=true;
volatile char registers[10];
volatile char reg[4];
volatile bool reading;

void print_debug(String message){
  Serial.println(message);
}
char read_register(byte address){
  
  return registers[address];
}
void write_register(byte address,char value){
  
  registers[address]=value;
 
}
void process_request(){
  byte command_type,read_address,write_address,write_value;
  command_type=buf[0];
  if(0x1==command_type){
    read_address=buf[1];
    char val=read_register(read_address);
    SPDR=val;
    process_it=false;
  }
  if(0x2==command_type){
    write_address=buf[1];
    write_value=buf[2];
    write_register(write_address,write_value);
    process_it=false;
  }
 
  
}
void generate_response(){

}
__inline void handler(byte c){
  
  if (received_data_length < sizeof buf)
    {
      // if first byte of data
    if(0==received_data_length){
      current_message_type=c;
     }
    buf[received_data_length++]=c;
    //Serial.print(current_message_type);
    if(0x2==current_message_type){
      if(3==received_data_length){
        process_it=true;
      }
    }
    else if(0x1==current_message_type){
      if(2==received_data_length){
        process_it=true;
      }
    }
    else if(0x3==current_message_type){
      received_data_length=0;
    }
    else if(0x4==current_message_type){
      
      SPDR=reg[received_data_length-1];
      
      if(4==received_data_length){
        received_data_length=0;
      }
      
    }
    
  }
}
void setup (void)
{
  Serial.begin (115200);   // debugging
  reading=false;
  // turn on SPI in slave mode
  SPCR |= bit (SPE);
  SPCR |= _BV(SPIE);
  reg[0]=11;
  reg[1]=12;
  reg[2]=13;
  reg[3]=100;
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);

  // get ready for an interrupt
  pos = 0;   // buffer empty
  received_data_length=0x0;
  process_it = false;
  
  // now turn on interrupts
  SPI.attachInterrupt();
  adxl.powerOn();

  //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); //62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
 
  //look of activity movement on this axes - 1 == on; 0 == off 
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);
 
  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);
 
  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(0);
  adxl.setTapDetectionOnY(0);
  adxl.setTapDetectionOnZ(1);
 
  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(50); //62.5mg per increment
  adxl.setTapDuration(15); //625us per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment
 
  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
 
  //setting all interrupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );
 
  //register interrupt actions - 1 == on; 0 == off  
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);

}  // end of setup


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;  // grab byte from SPI Data Register
  handler(c);


}

// main loop - wait for flag set in interrupt routine
void loop (void)
{
   
   if(process_it){
      process_request();
      received_data_length=0;
      
   }
  int x,y,z;  
  adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
  reg[0]=x;
  reg[1]=y;
  reg[2]=z;
  reg[3]=100;
 
}  // end of loop

