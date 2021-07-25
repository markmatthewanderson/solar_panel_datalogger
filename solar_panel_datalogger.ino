#define DEBUG

// include low power functionality
#include <LowPower.h>

// include the SD library:
#include <SPI.h>
#include <SD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// change this to match your SD shield or module;
// Adafruit SD shields and modules: pin 10
const int chipSelect = 10;

// Date and time functions using a PCF8523 RTC connected via I2C and Wire lib
#include "RTClib.h"
RTC_PCF8523 rtc;

// pin receiving interrupt from RTC
const uint8_t interruptPin = 2;

// array of months
String monthNames[12] = 
  {"January", "February", "March", 
  "April", "May", "June", 
  "July", "August", "September",
  "October", "November", "December"};

// name of file to write to
String dataFileName;

// variable to store data
uint32_t data;

void setup() 
{
  // set up timer interrupt
  rtc.begin();
  rtc.deconfigureAllTimers();
  pinMode(interruptPin, INPUT_PULLUP);
  //rtc.enableCountdownTimer(PCF8523_FrequencyMinute, 5);
  rtc.enableCountdownTimer(PCF8523_FrequencySecond, 30);
  attachInterrupt(digitalPinToInterrupt(interruptPin), timer_interrupt, FALLING);

  // get current date/time
  DateTime now = rtc.now();
  
  // choose name of file to write to
  dataFileName = String(now.day()) + monthNames[now.month()-1] + String(now.year()) + "_solar_panel_data.csv";

  // setup onboard LED
  pinMode(LED_BUILTIN, OUTPUT);

  // initialize data variable
  data = 0;

  //#ifdef DEBUG
    Serial.begin(57600);
    Serial.println("Debug mode active.");
  //#endif
}

void loop() 
{
  // wait for interrupt
  //asm volatile ("sleep");
  // sleep forever, analog to digital converter on, brown out detector on
  //LowPower.powerDown(SLEEP_FOREVER, ADC_ON, BOD_ON);
  delay(10000);
  
  //#ifdef DEBUG
    Serial.println("Finished one iteration of loop().");
  //#endif
}

void sd_write(String data)
{
  // open file
  File dataFile = SD.open(dataFileName, FILE_WRITE);

  // write to file if it's available
  if (dataFile)
  {
    dataFile.println(data);
    dataFile.close();
  }
  else
  {
    // error indication here?
  }
}

void timer_interrupt()
{
  // disable interrupts
  noInterrupts();
  
  //#ifdef DEBUG
    Serial.flush();
    Serial.println("Entered interrupt handler.");
  //#endif
  
  // turn on LED
  digitalWrite(LED_BUILTIN, HIGH);

  // collect data
  data++;

  // log data
  DateTime now = rtc.now();
  //sd_write(String(now.day()) + monthNames[now.month()-1] + String(now.year()) + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + 
  //  + ", " + String(data) + "\n");
  
  // allow timer interrupt to clear (default interrupt length is 3/64th of a second)
  delay(250);

  // turn off LED
  digitalWrite(LED_BUILTIN, LOW);

  //#ifdef DEBUG
    Serial.println("Exited interrupt handler.");
  //#endif

  // re-enable interrupts
  interrupts();
}
