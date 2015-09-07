// Maurice Ribble 
// 4-17-2008
// http://www.glacialwanderer.com/hobbyrobotics

// This code tests the DS1307 Real Time clock on the Arduino board.
// The ds1307 works in binary coded decimal or BCD.  You can look up
// bcd in google if you aren't familior with it.  There can output
// a square wave, but I don't expose that in this code.  See the
// ds1307 for it's full capabilities.

#include "Wire.h"
#define DS1307_I2C_ADDRESS 0x68
#define ledPin 13


int buttonPin = 28;
int buttonState = LOW;
int TimeWrittenYetFlag = 0;



// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
    return ( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
    return ( (val/16*10) + (val%16) );
}

// Stops the DS1307, but it has the side effect of setting seconds to 0
// Probably only want to use this for testing
/*void stopDs1307()
 {
 Wire.beginTransmission(DS1307_I2C_ADDRESS);
 Wire.write(0);
 Wire.write(0x80);
 Wire.endTransmission();
 }*/

// 1) Sets the date and time on the ds1307
// 2) Starts the clock
// 3) Sets hour mode to 24 hour clock
// Assumes you're passing in valid numbers
void setDateDs1307(byte second,        // 0-59
byte minute,        // 0-59
byte hour,          // 1-23
byte dayOfWeek,     // 1-7
byte dayOfMonth,    // 1-28/29/30/31
byte month,         // 1-12
byte year)          // 0-99
{
    Wire.beginTransmission(DS1307_I2C_ADDRESS);
    Wire.write(0);
    Wire.write(decToBcd(second));    // 0 to bit 7 starts the clock
    Wire.write(decToBcd(minute));
    Wire.write(decToBcd(hour));      // If you want 12 hour am/pm you need to set
    // bit 6 (also need to change readDateDs1307)
    Wire.write(decToBcd(dayOfWeek));
    Wire.write(decToBcd(dayOfMonth));
    Wire.write(decToBcd(month));
    Wire.write(decToBcd(year));
    Wire.endTransmission();
}

// Gets the date and time from the ds1307
void getDateDs1307(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
    // Reset the register pointer
    Wire.beginTransmission(DS1307_I2C_ADDRESS);
    Wire.write(0);
    Wire.endTransmission();

    Wire.requestFrom(DS1307_I2C_ADDRESS, 7);

    // A few of these need masks because certain bits are control bits
    *second     = bcdToDec(Wire.read() & 0x7f);
    *minute     = bcdToDec(Wire.read());
    *hour       = bcdToDec(Wire.read() & 0x3f);  // Need to change this if 12 hour am/pm
    *dayOfWeek  = bcdToDec(Wire.read());
    *dayOfMonth = bcdToDec(Wire.read());
    *month      = bcdToDec(Wire.read());
    *year       = bcdToDec(Wire.read());
}


void setup()
{
    delay(2000);

    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT);


    byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
    Wire.begin();
    Serial.begin(9600);
    Serial.println("STARTING...");

}

void loop()
{
    byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

    getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
    Serial.print(hour, DEC);
    Serial.print(":");
    if (minute < 10) {
        Serial.print("0");
    }
    Serial.print(minute, DEC);
    Serial.print(":");
    if (second < 10) {
        Serial.print("0");
    }
    Serial.print(second, DEC);
    Serial.print("  ");
    Serial.print(month, DEC);
    Serial.print("/");
    if (dayOfMonth < 10) {
        Serial.print("0");
    }
    Serial.print(dayOfMonth, DEC);
    Serial.print("/");
    Serial.print(year + 2000, DEC);
    Serial.print("  Day_of_week:");
    Serial.println(dayOfWeek, DEC);

    if (second == 0) {
        digitalWrite(ledPin, HIGH);
    } 
    else{
        digitalWrite(ledPin, LOW);
    }



    delay(500);
}

