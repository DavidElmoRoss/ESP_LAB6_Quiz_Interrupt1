/**********************************************************************
  This program will send the message   IntrudEr  to the 4 digit display
  every time that motion is detected.
  Pick the hex values you will need to have it say
  StoP IntrudEr   and add it to the message where shown below:
**********************************************************************/
#include <Arduino.h>              // include arduino library
#include <TM1637Display.h>

//DEFINE SECTION
#define CLK  33                // The ESP32 pin GPIO33 connected to CLK
#define DIO  32                // The ESP32 pin GPIO32 connected to DIO
#define Colon_On 0x40          // mask value to turn ON Colon on display
#define Colon_Off 0x00         // mask value to turn OFF Colon on display
int sensorPin =19 ;               // Pin # of the infrared motion sensor pin
uint8_t i;                    // index_value defined as having 8 bit data values
volatile int motion_detected=0;
//HARDWARE DEFINITION SECTION
TM1637Display segment = TM1637Display(CLK, DIO);

static portMUX_TYPE my_mutex = portMUX_INITIALIZER_UNLOCKED;                                   

void IRAM_ATTR isr()              // puts isr code into RAM for fast response
{
  portENTER_CRITICAL(&my_mutex);  // Block any processes accessing sensorPin
                                  // since IRQ PB_PIN access is MUTually EXclusive. 
      motion_detected=1;          // make counter 1 to indicate isr() has executed
  portEXIT_CRITICAL(&my_mutex);   // allow other processes to access sensorPin  
}
unsigned char SEG_DONE[] =        // This has the scrolling message
{
 0x00,0x00,0x00,0x00, 
                                  // Hex values for StoP
 0,0x06,0x54,0x78,0x50,0x1c,    
 0x5e,0x79,0x50,                          
 0x00,0x00,0x00,0x00               // 4 trailing blanks
};


void setup() 
{
  Serial.begin(9600);            // initialize serial
  segment.setBrightness(4);    // set the brightness to 4 in range (0:dimmest, 7:brightest)
  segment.clear();             // clear the 4 digit display
  pinMode(sensorPin, INPUT);     // set ESP32 pin to input mode 
                                 // trigger isr on RISING edge from IR Sensor
  attachInterrupt(digitalPinToInterrupt(sensorPin),&isr,RISING);
                                 // print DETECTION STATUS at (10,32) on screen
  printf("\e[2J\e[10;32HDetection Status");
                                 // set future text to BOLD CYAN
  printf("\e[1;36m"); 
}

void loop() 
{
  if (motion_detected==1)                    // if motion detected
  {   
    printf("\e[11;36HDetected");   // print DETECTED at (11,36) on Screen
    fflush(stdout);                // flush output to screen
    for(i=0;i<=13;++i)                 // modify loop for 3 more values
    {
     segment.setSegments(SEG_DONE+i);  
     delay(250);
    }
    motion_detected=0;             // reset motion_detected to 0
  }
  printf("\e[11;36H        ");     // clear DETECTED message
}
