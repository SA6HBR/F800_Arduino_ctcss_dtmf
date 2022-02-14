////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CTCSS for Ericson F80X by SA6HBR
// https://github.com/SA6HBR


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <NewTone.h>

LiquidCrystal_I2C lcd(0x27,16,2);

////////////////// HARDWAREINFO ////////////////////////////////////////////////////////////////////////////////////////////////////
// SM4EGB
// http://komradio.com/f800.html
// Ericson F802 F804

//////////////////  CTCSS OUT  /////////////////////////////////////////////////////////////////////////////////////////////////////
//
// AUTHOR/LICENSE:
// Created by Tim Eckel - teckel@leethost.com
// Copyright 2013 License: GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
//
// LINKS:
// Project home: https://bitbucket.org/teckel12/arduino-new-tone/wiki/Home
//
// SYNTAX:
//   NewTone( pin, frequency [, length ] ) - Play a note on pin at frequency in Hz.
//     Parameters:
//       * pin        - Pin speaker is wired to (other wire to ground, be sure to add an inline 100 ohm resistor).
//       * frequency  - Play the specified frequency indefinitely, turn off with noNewTone().
//       * length     - [optional] Set the length to play in milliseconds. (default: 0 [forever], range: 0 to 2^32-1)
//   noNewTone(pin)   - Stop playing note (pin is optional, will always stop playing on pin that was last used).

//////////////////  CTCSS IN ///////////////////////////////////////////////////////////////////////////////////////////////////////
//  YO3HJV, ON4ADI & ON7EQ
//  https://www.qsl.net/on7eq/projects/arduino_ctcss.htm
//
//////////////////  PARAMETERS WHICH CAN BE FINETUNED FOR OPTIMAL DECODING  /////////////////////////////////////////////////////////
//  Decoding of a CTCSS in presence of a signal properly modulated with a CTCSS subtone is no problem and can be easily implemented.
//  The challenge is however NOT TO DECODE in a random manner CTCSS in case of no RF signal, when only noise is present from the dicriminator output.
//  The sketch below includes a waveform analysis performed during the sampling period, to determine if a 'clean' CTCSS is present or not.
//  Some latitude is however permitted to take into account flutter, short spikes, interaction with voice modulation  etc. and still indicate a
//  stable and reliable CTCSS tone.

//  The parameters below allow to finetune the detection in accordance with CTCSS tone(s) to be detected.

//  If parameters are set too stringent, decoding will not be assured in a realiable manner in presence of voice signals
//  If parameters are not set 'tight' enough, stray decoding of 'FRIEND' CTCSS wil occur in presence of noise (signal from discriminator output)

//  the sketch allows decode of 'FRIEND' and 'FOE' CTCSS

//  Adjust the audio level fed to LPF and signal conditioner (making square waves out of sine) to a point where a reliable detection of highest desired CTCSS is possible, 
//  and in the noise the number of 'valid decodes' is 0 or 1, sometimes 2, but does not exceed 2.  Increasing further the audio level (valid decodes > 2) will only increase 
//  the risk of random decodes.  Normally, in preesence of only noise, maximum 1 stray decode of FRIEND CTCSS will occur per hour. 


volatile unsigned long ctcssBand = 10;  // This is +/- % allowed error in waveform format detected, to filter out noise (ideally , waveform should be a pure square wave)
                                        // Do not set too low or decoding will not be possible when voice signals are present

float validdecodes = 50;                // This is the % of valid CTCSS waveforms decoded in one sampling period, required to eliminate spikes & random decode in noise

int filterMinCtcss =  70;               // Lowest  CTCSS frequency to decode. This will set a spikes filter and prevent false decode in noise  
int filterMaxCtcss = 160;               // Highest CTCSS frequency to decode. This will set a spikes filter and prevent false decode in noise  

volatile unsigned long counter = 0;
volatile unsigned long oddEven = 0;
volatile unsigned long startStopOk = 0;
volatile unsigned long secondLastStartStopOk = 0;
volatile unsigned long startStopTime = 0;
volatile unsigned long lastOddTime = 0;
volatile unsigned long secondLastOddTime = 0;
volatile unsigned long lastEvenTime = 0;
volatile unsigned long numPeriodesOk = 0;
volatile unsigned long totalTimeOk = 0;
volatile unsigned long periodTimeMinus = 0;
volatile unsigned long periodTimePlus = 0;
volatile unsigned long periodTime = 0;

float result = 0;
float freq = 0.0;

volatile unsigned long low_passF  = 0;
volatile unsigned long high_passF = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SA6HBR

// ctcss array no
//  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 
// 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
// 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
// 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
// 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 

float ctcss[] = {
 67.0,  69.3,  71.9,  74.4,  77.0,  79.7,  82.5,  85.4,  88.5,  91.5,
 94.8,  97.4, 100.0, 103.5, 107.2, 110.9, 114.8, 118.8, 123.0, 127.3,
131.8, 136.5, 141.3, 146.2, 151.4, 156.7, 159.8, 162.2, 165.5, 167.9,
171.3, 173.8, 177.3, 179.9, 183.5, 186.2, 189.9, 192.8, 196.6, 199.5,
203.5, 206.5, 210.7, 218.1, 225.7, 229.1, 233.6, 241.8, 250.3, 254.1};

// hardware-settings:
int   channelPin[]      = { 3, 4, 5, 6}; // F800 pin for channel
int   ctcssInPin        = 2;
int   ctcssOutPin       = 8;
int   pttPin            = 13;
int   channelNo         = 0;          
float activeCtcssForRX  = 0.0;
float activeCtcssForTX  = 0.0;
char  statusRX          = ' ';
char  statusTX          = ' ';

// channel-settings: This excample 1 control and 2 in scanning, but add as many you want. :)
char *reciverChannelText[] = {"CH 1", "CH 2", "CH 3"}; //F802 Channel-name in display
byte      reciverChannel[] = {     1,      2,      3}; //F802 Channel-number
int           ctcssForRX[] = {     0,      7,      9}; //ctcss for RX
int           ctcssForTX[] = {     0,      9,      7}; //ctcss for TX

int scanOff    = 0; //ctcss 67.0 for stop scanning  
int scanOn     = 5; //ctcss 77.0 for starting scanning
int scanStatus = 1;  // 1 = starting with scanning, 0 = not scanning 
int channelActive = reciverChannel[1];

unsigned long  pttTime = 0;    // how long ptt has been active
unsigned long hangTime = 3000; //3 * 1000 = 3 seconds, how long time after last active ctcss to ptt off
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()

   {

      Serial.begin(9600);

      pinMode( ctcssInPin, INPUT_PULLUP);            //ctcss in
      
      pinMode( channelPin[0], OUTPUT);               //Channel 1
      pinMode( channelPin[1], OUTPUT);               //Channel 2
      pinMode( channelPin[2], OUTPUT);               //Channel 4
      pinMode( channelPin[3], OUTPUT);               //Channel 8
      setChannelActive(channelActive);               //set Channel
      
      pinMode(pttPin, OUTPUT);                       //PTT
      digitalWrite(pttPin, LOW);                     //set PTT off
      
      lcd.init();                                    // Analog pin 4 & 5
      lcd.backlight();
      
      ctcssBand    = ctcssBand * 100;
      validdecodes = validdecodes / 100;
      
      low_passF =   500000  / filterMaxCtcss; // Spikes filter
      high_passF = 1000000  / filterMinCtcss; // Spikes filter

      bootscreen();
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void loop()   
  {


// Looping channels in reciverChannel
for (byte i = 0; i < (sizeof(reciverChannel) / sizeof(reciverChannel[0])); i++) 
  {
    //If no ctcss - change channel
    if (scanStatus>0 && activeCtcssForRX == 0)
      {
        statusRX = 'S';
        statusTX = ' ';
        channelActive = reciverChannel[i];
        setChannelActive(channelActive);
        channelNo  = i;
        delay(50);
      }

      // Check if it is a valid ctcss
      freq = getFrequency(100);

      // ctcss for stop scanning on F80X first channel?
      if (channelActive==1 && freq >= ctcss[scanOff] - 2 && freq <= ctcss[scanOff] + 2)    
      {
        scanStatus=0;
        statusRX = ' ';
        statusTX = ' ';       
        activeCtcssForRX = 0;
        activeCtcssForTX = 0;
        }

      // ctcss for start scanning on F80X first channel?
      else if (channelActive==1 && freq >= ctcss[scanOn] - 2 && freq <= ctcss[scanOn] + 2)    
      {
        scanStatus=1;
        activeCtcssForRX = 0;
        activeCtcssForTX = 0;
        }

      // Active scanning and right ctcss for channel
      else if (scanStatus>0 && freq >= ctcss[ctcssForRX[channelNo]] - 2 && freq <= ctcss[ctcssForRX[channelNo]] + 2)    
      {
        pttTime = millis();
        statusRX = ' ';
        statusTX = 'P';
        activeCtcssForRX = ctcss[ctcssForRX[channelNo]];
        activeCtcssForTX = ctcss[ctcssForTX[channelNo]];
      }
      
      // Not active ctcss and out of hangtime
      else if (pttTime <= millis()-hangTime && activeCtcssForRX != 0)
      {
        activeCtcssForRX = 0;
        digitalWrite(pttPin, LOW);
        noNewTone(ctcssOutPin);
        delay(50);
        lcd.init();  
      }
      
          
      writeLcd(0,"RX", freq   , channelActive, statusRX);

      if (scanStatus>0  && activeCtcssForRX >0) 
      {
        digitalWrite(pttPin, HIGH);
        NewTone(ctcssOutPin, activeCtcssForTX, 0);
        writeLcdText(1,reciverChannelText[channelNo]);
      }
      else
      { 
        digitalWrite(pttPin, LOW);
        noNewTone(ctcssOutPin);
        
        if (scanStatus>0) 
        {
         writeLcdText(1,"Scanning...");
        }
        else
        {
          writeLcdText(1,"Scan off");
        }
      }    
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setChannelActive(byte channel)
{
  for (int i =0;i<4;i++)
  {
    if (bitRead(channel, i)==1)
    {
      digitalWrite(channelPin[i], HIGH);
    }
    else
    {
      digitalWrite(channelPin[i], LOW);
    }
  }  
}

void writeLcd(byte row, String text1, float _freq, byte channel, char text2 )
  {
    lcd.setCursor(1, row);
    lcd.print (text1);
    lcd.print("   ");
    int pos = 5;
    if (round(_freq*100) > 9999){pos -=1;}
    if (round(_freq*100) >  999){pos -=1;}
    lcd.setCursor(pos, row);
    lcd.print (_freq,2);
    lcd.print(" ");  
    lcd.setCursor(10, row);
    lcd.print("CH ");
    if (channel < 10){lcd.print(" ");}
    lcd.print (channel);
    lcd.print (text2);    
  }
  
void writeLcdText(byte row, String text )
  {
    lcd.setCursor(1, row);
    lcd.print (text);
    lcd.print ("                 ");
  }

void bootscreen()
  {
      writeLcdText(0,"CTCSS for F80X");
      writeLcdText(1,"*** SA6HBR ***");
      delay(5000);
  }
  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


float getFrequency(unsigned int sampleTime)   

   {                                           // START OF getFrequency FUNCTION

   counter = 1;
   totalTimeOk = 0;
   numPeriodesOk = 0;
   attachInterrupt(digitalPinToInterrupt(2), interruptHandlerChange, CHANGE);      
   delay(sampleTime);
   detachInterrupt(digitalPinToInterrupt(2));

   if (totalTimeOk < 40000) {
      result = 0.0;
      }
    else if ((totalTimeOk / numPeriodesOk) < 4000){            // digital filter Low Pass
      result = 0.0;
      }
    else if ((totalTimeOk / numPeriodesOk) > 16000){           // digital filter High Pass
      result = 0.0;
      }  
    else if (numPeriodesOk <= (((counter-1)/2)*validdecodes))   // check how many valid waves in one sample
      {
      result = (0.0);
      }


   else
      {
      result = (1000700.0 * (float)(numPeriodesOk))/(float)(totalTimeOk);
      
// NOTE: 1000700.0 is the value found by me. The theoretic value is "1.000.000,0"
// Start with this value and check the precision against a good frequency meter.
       }
                                                                                                  
   return result;
    }                                             // END OF getFrequency FUNCTION


////////////////////////////////////////////////////////////////////////////////

void interruptHandlerChange()

   {                                  // START OF interruptHandlerChange ROUTINE

   startStopTime = micros();

   if (counter == 1)                  // if counter is 1 : we have to initialize
      {  
      oddEven = 1;
      startStopOk = 2;   
      secondLastStartStopOk = 2;
      lastOddTime = startStopTime;
      secondLastOddTime = startStopTime;
      lastEvenTime = startStopTime;               
      }

   if (oddEven == 1)                    // lets pretend : odd is a rising change
      {
      oddEven = 2;
      startStopOk = 1;

      if (((startStopTime - lastEvenTime) < low_passF))      // Spikes filter
         {
         startStopOk = 2;
         }

      if (((startStopTime - lastOddTime) < (low_passF * 2)))       
         {
         startStopOk = 2;
         secondLastStartStopOk = 2;
         }

      if (((startStopTime - lastOddTime) > high_passF))     // Spikes filter
         {
         startStopOk = 2;
         secondLastStartStopOk = 2;
         }

      lastOddTime = startStopTime;
      }

   else                               // lets pretend : even is a falling change
      {
      oddEven = 1;

      if (((startStopTime - lastOddTime) < low_passF))
         {
         startStopOk = 2;
         }

      if ((startStopOk == 1) && (secondLastStartStopOk == 1)) {

         if (numPeriodesOk == 0)
            {  
            numPeriodesOk++;
            totalTimeOk = totalTimeOk + lastOddTime;
            totalTimeOk = totalTimeOk - secondLastOddTime;
            }

         else
            {
            periodTime = totalTimeOk * ctcssBand;  //  Wave form analysis 
            periodTimeMinus = (totalTimeOk * 10000);
            periodTimePlus = periodTimeMinus;
            periodTimeMinus = (periodTimeMinus - periodTime);
            periodTimePlus = (periodTimePlus + periodTime);
            periodTimeMinus = (periodTimeMinus / numPeriodesOk);
            periodTimePlus = (periodTimePlus / numPeriodesOk); 
            periodTime = (lastOddTime - secondLastOddTime);
            periodTime = (periodTime * 10000);

            if ((periodTime > periodTimeMinus) && (periodTime < periodTimePlus))
               {
               numPeriodesOk++;
               totalTimeOk = totalTimeOk + lastOddTime;
               totalTimeOk = totalTimeOk - secondLastOddTime;
               }
                         
             else {
                if (numPeriodesOk < 3) {
                numPeriodesOk = 1;
                totalTimeOk = lastOddTime - secondLastOddTime;
                }
             }
           } 
         }

      if ( startStopOk == 1 )
         {
         secondLastOddTime = lastOddTime;
         secondLastStartStopOk = startStopOk;
         }

      lastEvenTime = startStopTime;   
      }
 
   counter++;  // debug good decodes

   }                                    // END OF interruptHandlerChange ROUTINE
