// include the SD library:
#include <SPI.h>
#include <SD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// RTC chip pcf8523

// change this to match your SD shield or module;
// Adafruit SD shields and modules: pin 10
const int chipSelect = 10;

// name of file to write to
String dataFileName = "data.csv";

void setup() 
{

}

void loop() 
{

}

void log(String data)
{
  // open file
  File dataFile = SD.open(dataFileName, FILE_WRITE);

  
}
