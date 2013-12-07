#include <QuadEncoder.h>
#include <LiquidCrystal.h>
#include <Bounce.h> 


int qe1Move=0;

QuadEncoder qe(6,7);  // Rotary encoder connecter to pins 6 nd 7


#define BM 8     //Digital pin 8 - mode switch 

#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }
 #define W_CLK A2       // Pin  - connect to AD9850 module word load clock pin (CLK)
 #define FQ_UD A3      // Pin  - connect to freq update pin (FQ)
 #define DATA  A4      // Pin  - connect to serial data load pin (DATA)
 #define RESET A5      // Pin  - connect to reset pin (RST).

int prev=HIGH;
Bounce bouncer = Bounce( BM,5 ); 
double freq=1901700; //7.1mhz = IF - freq . This is initial start up frequency .Change here as per  your need.
double if_freq= 9001700; //change to your IF .See the display freq comment  below.
double dp;
int mode=1000;
Bounce bm = Bounce( BM,10 );
int value;
 LiquidCrystal lcd(11, 9, 5, 4, 3, 2);

// arduino dds  control code stolen from http://nr8o.dhlpilotcentral.com/?p=83 :)

//stolen code begins
// transfers a byte, a bit at a time, LSB first to the 9850 via serial DATA line
void tfr_byte(byte data)
{
  for (int i=0; i<8; i++, data>>=1) {
    digitalWrite(DATA, data & 0x01);
    pulseHigh(W_CLK);   //after each bit sent, CLK is pulsed high
  }
}

 // frequency calc from datasheet page 8 = <sys clock> * <frequency tuning word>/2^32
void sendFrequency(double frequency) {
  int32_t freq = frequency * 4294967295/125000000;  // note 125 MHz clock on 9850
  for (int b=0; b<4; b++, freq>>=8) {
    tfr_byte(freq & 0xFF);
  }
  tfr_byte(0x000);   // Final control byte, all 0 for 9850 chip
  pulseHigh(FQ_UD);  //  
}
//stolen code ends

void setup() {
    Serial.begin(9600);
    
       pinMode(BM,INPUT);
   digitalWrite(BM, HIGH ); //enable pull up
    
     lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("VU2SWX");
    lcd.setCursor(7, 0);
    lcd.print("Step:");
     lcd.print("1Khz ");
     
     //set up dds
     
  pinMode(FQ_UD, OUTPUT);
  pinMode(W_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT);
 
  pulseHigh(RESET);
  pulseHigh(W_CLK);
  pulseHigh(FQ_UD);  // t 
}

void loop() {
 //Serial.print("\n");
 //Serial.print(freq);
      delay(1);
      
      
       lcd.setCursor(0, 1);
  // print the  frequency
delay(1);  // avoid lcd flicker (on blue lcd ?) you can try setting this to 0 for better keypad responese
lcd.print("  ");
lcd.print(if_freq-freq);  // Change here for IF + freq 
lcd.print("Hz");
sendFrequency(freq);
dp=if_freq-freq;
     bouncer.update ( );
   value = bouncer.read();
 
 // Turn on or off the LED
 if ( value == LOW && prev==HIGH) {
   lcd.setCursor(7, 0);
    lcd.print("Step:");
 
    switch ( mode ) {
      case 10000:
         mode =1000;
           lcd.print("1kHz ");
         break;
    case 1:
         mode =10000;
           lcd.print("10kHz ");
         break;
    case 10:
        mode= 1;
        lcd.print("1Hz  ");
      break;
  case 1000:
       mode= 100;
       lcd.print("100Hz");
      break;
  case 100:
       mode= 10;
       
        lcd.print("10hz ");
      break;

  }
   
  lcd.print(mode);
  
   Serial.print(mode);
 } else {
 }
 prev=value;
  

   qe1Move=qe.tick();
  if (qe1Move=='<'){
    Serial.print(char(qe1Move));
    
    freq=freq+mode;
 //    if (dp>= 7.2e6) // limit the band
  //     freq= 1.815e6;
     Serial.print(freq);
    
  }
  else if (qe1Move=='>'){
    Serial.print(char(qe1Move));
    
    freq=freq-mode;
    
    // if (dp<= 7.0e6) //limit the band
       
 //   freq= 2.015e6;
    Serial.print(freq);
  }
 
      
  
  
   
}
