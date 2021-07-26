// include low power functionality
#include <LowPower.h>

// include the SD library:
#include <SPI.h>
#include <SD.h>

// change this to match your SD shield or module;
// Adafruit SD shields and modules: pin 10
const int chipSelect = 10;

// Date and time functions using a PCF8523 RTC connected via I2C and Wire lib
#include "RTClib.h"
RTC_PCF8523 rtc;

// pin receiving interrupt from RTC
const uint8_t interruptPin = 2;

// array of months
//String monthNames[12] = 
//  {"January", "February", "March", 
//  "April", "May", "June", 
//  "July", "August", "September",
//  "October", "November", "December"};

// name of file to write to
//String dataFileName;

// file to write to
File dataFile;

// variable to store data
uint32_t data;

void setup() 
{
  //#ifdef DEBUG
    Serial.begin(9600);
    while(!Serial);
    Serial.println("Debug mode active.");
    Serial.flush();
  //#endif

  if (!rtc.begin()) 
  {
    Serial.println("Unable to find RTC.");
    Serial.flush();
    while(1)
    {
      delay(500);
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
  }

  pinMode(chipSelect, OUTPUT);
  if (!SD.begin(chipSelect))
  {
    Serial.println("Unable to find SD.");
    Serial.flush();
    while(1)
    {
      delay(500);
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
  }

  // let things settle down a bit
  delay(1000);
  
  // set up timer interrupt
  pinMode(interruptPin, INPUT_PULLUP);
  rtc.deconfigureAllTimers();
  //rtc.enableCountdownTimer(PCF8523_FrequencyMinute, 5);
  rtc.enableCountdownTimer(PCF8523_FrequencySecond, 90);
  attachInterrupt(digitalPinToInterrupt(interruptPin), timer_interrupt, FALLING);

  // setup onboard LED
  pinMode(LED_BUILTIN, OUTPUT);

  // initialize data variable
  data = 0;
}

void loop() 
{
  // wait for interrupt
  // sleep forever, analog to digital converter on, brown out detector on
  Serial.println("Enter sleep.");
  Serial.flush();
  LowPower.powerDown(SLEEP_FOREVER, ADC_ON, BOD_ON);

  // interrupt was detected, process data
  // turn on LED, then wait a bit
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  // collect data
  data++;
  // log data
  DateTime now = rtc.now();
  sd_write(String(now.month()) + "/" + String(now.day()) + "/" +  String(now.year()) + " " + 
    String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + 
    + ", " + String(data));
  // wait a bit, then turn off LED
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.println("Finished one iteration of loop().");
  Serial.flush();
}

void sd_write(String data_item)
{
  // select file name
  char dataFileName[] = "test.csv";
  
  // open file
  dataFile = SD.open(dataFileName, FILE_WRITE);
  
  // write to file if it's available
  if (dataFile)
  {
    dataFile.println(data_item);
    //dataFile.flush();
    dataFile.close();
  }
  else
  {
    // error indication here?
    Serial.println("Unable to write to SD card.");
    Serial.flush();
  }
}

void timer_interrupt()
{
  __asm__("nop\n\t");
  Serial.println("Entered interrupt handler.");
  Serial.flush();
}
