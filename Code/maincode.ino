/**
   Detects 'Active' and 'Inactive' motion and logs it with timestamps to a .csv file in SDCard.
   Uses  HCSR505 PIR Passive Infra Red Motion Detector.
   Modified code from https://learn.adafruit.com/adafruit-data-logger-shield/using-the-real-time-clock-3
   https://github.com/adafruit/Light-and-Temp-logger
   Author: Niroshinie Fernando 
*/

//Libraries
#include "SD.h"
#include <Wire.h>
#include "RTClib.h"

#define LOG_INTERVAL  1000 // mills between entries. 
// how many milliseconds before writing the logged data permanently to disk
// set it to the LOG_INTERVAL to write each time (safest)
// set it to 10*LOG_INTERVAL to write all data every 10 datareads, you could lose up to
// the last 10 reads if power is lost but it uses less power and is much faster!
#define SYNC_INTERVAL 1000 // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0; // time of last sync()


/*
  determines whether to send the stuff thats being written to the card also out to the Serial monitor.
  This makes the logger a little more sluggish and you may want the serial monitor for other stuff.
  On the other hand, its hella useful. We'll set this to 1 to keep it on. Setting it to 0 will turn it off.
*/
#define ECHO_TO_SERIAL   1 // echo data to serial port. 


//Variables
char activeMotion1 [] = "1";              // create variable for active mode of sensor 1
char inactiveMotion1 [] = "0";            // create variable for inactive mode of sensor 1           
char activeMotion2 [] = "1";              // create variable for active mode of sensor 2
char inactiveMotion2 [] = "0";            // create variable for inactive mode of sensor 2           
char activeMotion3 [] = "1";              // create variable for active mode of sensor 3
char inactiveMotion3 [] = "0";            // create variable for inactive mode of sensor 3           
char activeMotion4 [] = "1";              // create variable for active mode of sensor 4
char inactiveMotion4 [] = "0";            // create variable for inactive mode of sensor 4           
char* state1 ;
char* state2 ;
char* state3 ;
char* state4 ;

RTC_DS1307 RTC; // define the Real Time Clock object

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

// the logging file
File logfile;

void setup()
{
  Serial.begin(9600);

  // initialize the SD card
  initSDcard();

  // create a new file
  createFile();


  /**
   * connect to RTC
     Now we kick off the RTC by initializing the Wire library and poking the RTC to see if its alive.
  */
  initRTC();


  /**
     Now we print the header. The header is the first line of the file and helps your spreadsheet or math program identify whats coming up next.
     The data is in CSV (comma separated value) format so the header is too: "millis,stamp, datetime,hum,temp" the first item millis is milliseconds since the Arduino started,
     stamp is the timestamp, datetime is the time and date from the RTC in human readable format, hum is the humidity and temp is the temperature read.
  */
  logfile.println("millis,stamp,datetime,motion1,motion2,motion3,motion4");             // Create 7 column millis, stamp, datetime and data of 4 motion
#if ECHO_TO_SERIAL
  Serial.println("millis,stamp,datetime,motion1,motion2,motion3,motion4");              // Display 7 column of data in serial monitor    
#endif //ECHO_TO_SERIAL

  pinMode(5, INPUT);                  // Assign input pin 1 for motion 1
  pinMode(6, INPUT);                  // Assign input pin 2 for motion 2
  pinMode(7, INPUT);                  // Assign input pin 3 for motion 3
  pinMode(8, INPUT);                  // Assign input pin 4 for motion 4
}

void loop()
{
  DateTime now;

  // delay for the amount of time we want between readings
  delay((LOG_INTERVAL - 1) - (millis() % LOG_INTERVAL));

  // log milliseconds since starting
  uint32_t m = millis();
  logfile.print(m);           // milliseconds since start
  logfile.print(", ");
#if ECHO_TO_SERIAL
  Serial.print(m);         // milliseconds since start
  Serial.print(", ");
#endif

  // fetch the time
  now = RTC.now();
  // log time
  logfile.print(now.unixtime()); // seconds since 2000
  logfile.print(", ");
  logfile.print(now.year(), DEC);
  logfile.print("/");
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print(" ");
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  logfile.print(now.second(), DEC);
#if ECHO_TO_SERIAL
  Serial.print(now.unixtime()); // seconds since 2000
  Serial.print(", ");
  Serial.print(now.year(), DEC);
  Serial.print("/");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
#endif //ECHO_TO_SERIAL

  //Read data from 4 motion sensors and store it to variables

  //Sensor 1
  if (digitalRead(5) == HIGH) {
    //    Serial.println("Active");
    state1 = activeMotion1;
  }
  if (digitalRead(5) == LOW) {
    //    Serial.println("Inactive");
    state1 = inactiveMotion1;
  }
  
  //Sensor 2
  if (digitalRead(6)== HIGH) {
    //    Serial.println("Active");
    state2 = activeMotion2;
  }
  if (digitalRead(6) == LOW) {
    //    Serial.println("Inactive");
    state2 = inactiveMotion2;
  }
  
  //Sensor 3
  if (digitalRead(7) == HIGH) {
    //    Serial.println("Active");
    state3 = activeMotion3;
  }
  if (digitalRead(7) == LOW) {
    //    Serial.println("Inactive");
    state3 = inactiveMotion3;
  }
  
  //Sensor 4
   if (digitalRead(8) == HIGH) {
    //    Serial.println("Active");
    state4 = activeMotion3;
  }
  if (digitalRead(8) == LOW) {
    //    Serial.println("Inactive");
    state4 = inactiveMotion3;
  }
  
  // Ordering the data of 4 input motion sensor into the CSV file
  logfile.print(", ");
  logfile.print(state1);        //state of motion 1 in real time
  logfile.print(", ");
  logfile.print(state2);        //state of motion 2 in real time
  logfile.print(", ");
  logfile.print(state3);        //state of motion 3 in real time
  logfile.print(", ");
  logfile.println(state4);      //state of motion 4 in real time
  

  //Printing the data into serial monitor
#if ECHO_TO_SERIAL
  Serial.print(", ");
  Serial.print(state1);        //state of motion 1 in real time
  Serial.print(", ");
  Serial.print(state2);        //state of motion 2 in real time
  Serial.print(", ");
  Serial.print(state3);        //state of motion 3 in real time
  Serial.print(", ");
  Serial.println(state4);      //state of motion 4 in real time

#endif //ECHO_TO_SERIAL

  if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();

  logfile.flush();
}


/**
   The error() function, is just a shortcut for us, we use it when something Really Bad happened.
   For example, if we couldn't write to the SD card or open it.
   It prints out the error to the Serial Monitor, and then sits in a while(1); loop forever, also known as a halt
*/
void error(char const *str)
{
  Serial.print("error: ");
  Serial.println(str);

  while (1);
}

void initSDcard()
{
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

}

void createFile()
{
  //file name must be in 8.3 format (name length at most 8 characters, follwed by a '.' and then a three character extension.
  char filename[] = "MLOG00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[4] = i / 10 + '0';
    filename[5] = i % 10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE);
      break;  // leave the loop!
    }
  }

  if (! logfile) {
    error("couldnt create file");
  }

  Serial.print("Logging to: ");
  Serial.println(filename);
}

void initRTC()
{
  Wire.begin();
  if (!RTC.begin()) {
    logfile.println("RTC failed");
#if ECHO_TO_SERIAL
    Serial.println("RTC failed");
#endif  //ECHO_TO_SERIAL

  }
}
