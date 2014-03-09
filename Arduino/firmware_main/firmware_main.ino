// RTC SD Card Datalogger
//
// Logs data from analog pins 0-3 to files on SD card.
// Files are names by date, a log entry is appended per second with raw readings
// 
// RTClib can be obtained from https://github.com/adafruit/RTClib
//
// The SD card lib is GPL so be aware of licence implications.
//
// This code is public domain by Colin Riley, @domipheus on twitter, labs.domipheus.com
//

#include <Wire.h>
#include "RTClib.h"
#include <SD.h>

// we use the hardware CS pin 
const int chipSelect = 10;

RTC_DS1307 rtc;

void setup ()
{
    Serial.begin(9600);
    Wire.begin();
    rtc.begin();

    Serial.print("Initializing SD card...");
    // make sure that the default chip select pin is set to
    // output
    pinMode(10, OUTPUT);
     
    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
      Serial.println("Card failed, or not present");
      // don't do anything more:
      return;
    }
    Serial.println("card initialized.");  
}

uint8_t lastSecond = 0;
void loop ()
{
    DateTime now = rtc.now();
    if (now.second() != lastSecond) {
        char timebuffer[16];
        char filename[100];
        lastSecond = now.second();
        
        //make the filename from the date
        sprintf(filename, "%04u%02u%02u.log", now.year(), now.month(), now.day());

        // format the time into the string
        sprintf(timebuffer, "%02d:%02d:%02d,", now.hour(), now.minute(), now.second());
                
        // make a string for assembling the data to log:
        String dataString = "";
        
        dataString += timebuffer;
      
        // read sensors and append to the string:
        for (int analogPin = 0; analogPin < 4; analogPin++) {
          int sensor = analogRead(analogPin);
          dataString += String(sensor);
          if (analogPin < 3) {
            dataString += ",";
          }
        }
      
        // open the file. Reopening and closing each loop is inefficient,
        // but is easiest to implement and realiable for poweroff.
        File dataFile = SD.open(filename, FILE_WRITE);
      
        // if the file is available, write to it:
        if (dataFile) {
          dataFile.println(dataString);
          dataFile.close();
          // print to the serial port too:
          Serial.print(filename);
          Serial.print(" += ");
          Serial.println(dataString);
        }  
        else 
        {
          Serial.print("error opening file:");
          Serial.println(filename);
        }

    }
    delay(200);
}
