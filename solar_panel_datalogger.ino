// include low power functionality
#include <LowPower.h>

// include the SD library:
#include <SPI.h>
#include <SD.h>

// include date and time functions using a PCF8523 RTC connected via I2C and Wire lib
#include "RTClib.h"

// Define pins
const uint8_t buttonInterruptPin  = 2;  // pin receiving interrupt from push button
const uint8_t timerInterruptPin   = 3;  // pin receiving interrupt from RTC
const uint8_t led1                = 4;  // pin driving LED1
const uint8_t led2                = 5;  // pin driving LED2
const uint8_t chipSelect          = 10; // Adafruit SD shields and modules: pin 10
const uint8_t inputPin            = A0; // analog pin reading input voltage

// file to write to
File dataFile;

// variable to store data
uint32_t data;

// real-time clock object
RTC_PCF8523 rtc;

// variables to track status
bool timer_interrupt_enabled    = false;
bool timer_interrupt_triggered  = false;

void setup() 
{
  // set up pins
  pinMode(buttonInterruptPin, INPUT_PULLUP);
  pinMode(timerInterruptPin,  INPUT_PULLUP);
  pinMode(led1,               OUTPUT);
  pinMode(led2,               OUTPUT);
  pinMode(chipSelect,         OUTPUT);
  //pinMode(inputPin,           INPUT); // Not necessary for analog pin.
  pinMode(LED_BUILTIN,        OUTPUT);

  // initialize RTC
  if (!rtc.begin()) 
  {
    error_signal();
  }

  // initialize SD card
  if (!SD.begin(chipSelect))
  {
    error_signal();
  }

  // let things settle down a bit
  delay(1000);
  
  // set up timer interrupt
  rtc.deconfigureAllTimers();
  rtc.enableCountdownTimer(PCF8523_FrequencyMinute, 5);
  attachInterrupt(digitalPinToInterrupt(timerInterruptPin), timer_interrupt, FALLING);
  timer_interrupt_enabled = true;

  // set up button interrupt
  attachInterrupt(digitalPinToInterrupt(buttonInterruptPin), button_interrupt, FALLING);

  // initialize data variable
  data = 0;
}

void loop() 
{
  // wait for interrupt
  // sleep forever, analog to digital converter on, brown out detector on
  LowPower.powerDown(SLEEP_FOREVER, ADC_ON, BOD_ON);

  // timer interrupt was detected, process data
  if(timer_interrupt_triggered)
  {
    // turn on LED, then wait a bit
    digitalWrite(led2, HIGH);
    delay(1000);
    
    // collect data
    data = analogRead(inputPin);
  
    // log data
    DateTime now = rtc.now();
    sd_write(String(now.month()) + "/" + String(now.day()) + "/" +  String(now.year()) + " " + 
      String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + 
      + ", " + String(data));
  
    // wait a bit, then turn off LED
    delay(1000);
    digitalWrite(led2, LOW);

    // clear timer interrupt signal
    timer_interrupt_triggered = false;
  }
}

void sd_write(String data_item)
{
  // select file name
  char dataFileName[] = "data.csv";
  
  // open file
  dataFile = SD.open(dataFileName, FILE_WRITE);
  
  // write to file if it's available
  if (dataFile)
  {
    dataFile.println(data_item);
    dataFile.close();
  }
  else
  {
    error_signal();
  }
}

void timer_interrupt()
{
  //__asm__("nop\n\t");
  timer_interrupt_triggered = true;
}

void button_interrupt()
{
  if (timer_interrupt_enabled)
  {
    digitalWrite(led1, HIGH);
    detachInterrupt(digitalPinToInterrupt(timerInterruptPin));
    timer_interrupt_enabled = false;
  }
  else
  {
    digitalWrite(led1, LOW);
    attachInterrupt(digitalPinToInterrupt(timerInterruptPin), timer_interrupt, FALLING);
    timer_interrupt_enabled = true;
  }

  // debounce button
  delay(500);

  // make it clear that this wasn't a timer interrupt
  timer_interrupt_triggered = false;
}

void error_signal()
{
    while(1)
    {
      delay(500);
      digitalWrite(led2, !digitalRead(led2));
    }
}
