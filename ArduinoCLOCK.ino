/*
  ARDUINO CLOCK USING TIME AND TIME LIBRARY
  Written by Aaron Williams
  4/18/2024
*/

#include <Time.h>
#include <TimeLib.h>
#include <SevSeg.h>

// From Arduino Website 'Time no RTC' Tutorial
#define TIME_MSG_LEN 11 // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER 'T' // Header tag for serial time sync message
#define TIME_REQUEST 7 // ASCII bell character requests a time sync message
// End of from tutorial

bool timeIsSet = false;

SevSeg sevseg;

void processSyncMessage() {
  // THIS METHOD, processSyncMessage() IS NOT MY OWN, 
  // ITS FROM A TUTORIAL ON ARDUINO WEBSITE
  
  // if time sync available from serial port, update time and return true
  while (Serial.available() >= TIME_MSG_LEN ) { // time message consists of header & 10 ASCII digits
    char c = Serial.read() ;
    Serial.print(c);
    if ( c == TIME_HEADER ) {
      time_t pctime = 0;
      for (int i = 0; i < TIME_MSG_LEN - 1; i++) {
        c = Serial.read();
        if ( c >= '0' && c <= '9') {
          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number
        }
      }
      setTime(pctime); // Sync Arduino clock to the time received on the serial port
    }
  }
}

void updateDisplayTime(int hours, int minutes){
  int localHour = hours;
  int localMinute = minutes;

  int currHour = hour();
  int currMinute = minute();

  char timeString[4];
  char tempHour[2];
  char tempMinute[2];

  sprintf(tempHour, "%d", currHour);
  sprintf(tempMinute, "%d", currMinute);

  if(localHour < 10){
    strcat(timeString, "0");
    strcat(timeString, tempHour);
  }
  else{
    strcat(timeString, tempHour);
  }

  if(localMinute < 10){
    strcat(timeString, "0");
    strcat(timeString, tempMinute);
  }
  else{
    strcat(timeString, tempMinute);
  }
  sevseg.setChars(timeString);
}

void updateDisplayTime(){
  updateDisplayTime(hour(), minute());
}

void validateHour(int hour){
  if(hour > 12){
    hour = 1;
  }
  else if(hour < 1){
    hour =12;
  }
}

void validateMinute(int minute){
  if(minute > 59){
    minute = 0;
  }
  else if(minute < 0){
    minute = 59;
  }
}

void setTheTime(){

  int hour = 1;
  int minute = 0;
  bool adjustingHour = true;
  bool adjustingMinute = false;
  int timeAsInt = 0100;

  while(true){
    //DISPLAY FAKE TIME METHOD HERE // ADD THIS

    if(adjustingHour == true){
      sevseg.refreshDisplay();
      if(digitalRead(A1) == HIGH){
        hour += 1;
        validateHour(hour);
      }
      else if(digitalRead(A0) == HIGH){
        hour -= 1;
        validateHour(hour);
      }
      else if(digitalRead(A2) == HIGH){
        adjustingHour = false;
        adjustingMinute = true;
      }
    }
    else if(adjustingMinute == true){
      sevseg.refreshDisplay();
      if(digitalRead(A1) == HIGH){
        minute += 1;
        validateMinute(minute);
      }
      if(digitalRead(A0) == HIGH){
        minute -= 1;
        validateMinute(minute);
      }
      else if(digitalRead(A2) == HIGH){
        adjustingHour = false;
        adjustingMinute = false;
        break;
      }
    }
  }
  // sets time to HOUR, MINUTE, SECOND
  updateDisplayTime(hour, minute);
  setTime(hour, minute, 0, 1, 1, 20);
}

void setup() {
  // Block of code from SevSeg library tutorial for display
  byte numDigits = 4;
  byte digitPins[] = {10, 11, 12, 13};
  byte segmentPins[] = {9, 2, 3, 5, 6, 8, 7, 4};
  bool resistorsOnSegments = true;
  byte hardwareConfig = COMMON_CATHODE;

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(100);
  // endof SevSeg library tutorial code

  // Set up the buttons as input
  pinMode(A0, INPUT); // Down button
  pinMode(A1, INPUT); // Up button
  pinMode(A2, INPUT); // reset button

  // This  sets the bit-per-second data rate, or baud rate
  // It's used for "Serial Data Communication". See here for more info: (https://www.javatpoint.com/arduino-serial-serial-begin#:~:text=The%20Serial.,(pins%201%20and%200).)
  Serial.begin(9600);

  // Built in to the Time library, pretty nice
  setTheTime();
}

void loop() {
  // code from TimeLibrary tutorial
  if (Serial.available() )
  {
    processSyncMessage();
  }
  if (timeStatus() == timeNotSet && timeIsSet == false){
    setTheTime(); // I added this
    Serial.println("waiting for sync message");
  }
  else{
    updateDisplayTime(); // I added this
    delay(1000);
  }
  // end of code fromm TimeLibrary tutorial
}
