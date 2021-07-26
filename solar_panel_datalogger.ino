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
  //#ifdef DEBUG
    Serial.begin(9600);
    while(!Serial);
    Serial.println("Debug mode active.");
    Serial.flush();
  //#endif

  if (!rtc.begin()) 
  {
    Serial.println("Couldn't find RTC.");
    Serial.flush();
    abort();
  }

  if (!SD.begin(chipSelect))
  {
    Serial.println("Couldn't initialize SD card.");
    Serial.flush();
    abort();
  }

  // let things settle down a bit
  delay(1000);
  
  // set up timer interrupt
  pinMode(interruptPin, INPUT_PULLUP);
  rtc.deconfigureAllTimers();
  //rtc.enableCountdownTimer(PCF8523_FrequencyMinute, 5);
  rtc.enableCountdownTimer(PCF8523_FrequencySecond, 90);
  attachInterrupt(digitalPinToInterrupt(interruptPin), timer_interrupt, FALLING);

  // get current date/time
  DateTime now = rtc.now();
  
  // choose name of file to write to
  //dataFileName = String(now.day()) + monthNames[now.month()-1] + String(now.year()) + "_solar_panel_data.csv";
  dataFileName = "solar_panel_data.csv";

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
  // turn on LED
  digitalWrite(LED_BUILTIN, HIGH);
  // collect data
  data++;
  // log data
  DateTime now = rtc.now();
  sd_write(String(now.day()) + monthNames[now.month()-1] + String(now.year()) + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + 
    + ", " + String(data) + "\n");
  // wait a bit, then turn off LED
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  // re-enable interrupts
  //attachInterrupt(digitalPinToInterrupt(interruptPin), timer_interrupt, FALLING);
  
  Serial.println("Finished one iteration of loop().");
  Serial.flush();
}

void sd_write(String data_item)
{
  // open file
  File dataFile = SD.open(dataFileName, FILE_WRITE);

  // write to file if it's available
  if (dataFile)
  {
    dataFile.println(data_item);
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
  // disable interrupts
  //detachInterrupt(digitalPinToInterrupt(interruptPin));
  Serial.println("Entered interrupt handler.");
  Serial.flush();
}
