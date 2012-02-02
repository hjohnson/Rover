/*
XBee Remote, demo 1
 By Harry Johnson
 This file is part of Xbee Remote
 
 This is the basic code, intended for combination with the XBee Remote Control.
 
 Xbee Remote is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 XBee Remote is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser Public License for more details.
 
 You should have received a copy of the GNU Lesser Public License
 along with XBee Remote.  If not, see <http://www.gnu.org/licenses/>.
 
 Reference:
 
 Pin Assignments:
 0: RX
 1: TX
 2: Switch 0
 3: Switch 1
 4: Switch 2
 5: Switch 3
 6: Switch 4
 7: Switch 5
 8: Switch 6
 9: XBee Comm enable, active low.
 10: LED Latch Enable, not on the SPI bus.
 11: Latch DS, data pin
 12: Latch ST_CP Latchpin
 13: Latch SH_CP clockPin.
 
 Seven Segment Assignments
 Q0: Decimal, LSB
 Q1: B
 Q2: A
 Q3: C
 Q4: D
 Q5: E
 Q6: F
 Q7: G, MSB
 */
byte segVal[16]; //to store the 16 patterns for the 7-segment display

const byte xbeeEnablePin = 9;
const byte sevenSegEnablePin = 10;
const byte dataPin = 11;
const byte latchPin = 12;
const byte clockPin = 13;

int previousPinValue = -1; 
int pinValue = 0; //see getPins


//Utility methods. You shouldn't have to modify these.
void displayValue(int value) {
  displayRawValue(segVal[value]);
}

void displayRawValue(byte value) {
  digitalWrite(latchPin, LOW); //keep the LEDs from updating while data is being shifted.
  shiftOut(dataPin, clockPin, MSBFIRST, value); //shift out all the data.
  digitalWrite(latchPin, HIGH); //update the LEDs.
}

void segSetup() { //The various 7 segment characters.
  segVal[0]   = ~0x7F; //~01111111
  segVal[1]   = ~0x0A; //~00001010
  segVal[2]   = ~0xB6; //~10110110
  segVal[3]   = ~0x9E; //~10011110
  segVal[4]   = ~0xCA; //~11001010
  segVal[5]   = ~0xDC; //~11011100
  segVal[6]   = ~0xFC; //~11111100
  segVal[7]   = ~0x0E; //~00001110
  segVal[8]   = ~0xFE; //~11111110
  segVal[9]   = ~0xCE; //~11001110
  segVal[0xA] = ~0xEF; //~11101111
  segVal[0xB] = ~0xF9; //~11111001
  segVal[0xC] = ~0x75; //~01110101
  segVal[0xD] = ~0xBB; //~10111011
  segVal[0xE] = ~0xF5; //~11110101
  segVal[0xF] = ~0xE5; //~11100101
  pinMode(sevenSegEnablePin, OUTPUT); 
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  digitalWrite(sevenSegEnablePin, LOW); //enable the whole LCD system. (had to be off since it shares the SPI bus, just in case. Need to redefine SS, though) 
  digitalWrite(latchPin, HIGH);
  displayValue(0x0);
}

//XBee Code
void enableXBee() {
  pinMode(xbeeEnablePin, OUTPUT);
  digitalWrite(8, LOW); //active low enable XBee communications.
  delay(1);
}

void disableXBee() {
  pinMode(xbeeEnablePin, INPUT); //switch XBee enable pin to high-impedance, external pull-up resistor keeps it disabled.
  delay(1);
}



unsigned int getPins() { //easy way to do switch statements based on pin states. Not required, though.
  unsigned int result =0;
  for(int ii = 0; ii<=6; ii++) {
    result |= ((1-(digitalRead(ii+2))) << ii); //if appropriate pin is LOW (active low switch), then activate appropriate bit.
  }
  return result;
}

//end of utility methods.

//Program here!
void setup() {
  Serial.begin(9600); //XBee uses 9600.
  Serial.flush(); //flush output buffer.
  Serial.println("Remote Control Ready");
  disableXBee(); //tri-state the pins.

  segSetup(); //setup the segment values, etc. 
  for(byte ii = 2; ii<=8; ii++) { //switches 0-5 inputs.
    pinMode(ii, INPUT); //input
    digitalWrite(ii, HIGH); //internal pullups enabled.
  }
  enableXBee(); //enable the XBee pins.
  displayValue(0xF);
}

void loop() {
  previousPinValue = pinValue; //so that when it reads the new pin values, it can tell if there's been a change.
  pinValue = getPins();
  if(pinValue != previousPinValue) {
  Serial.println(pinValue);
  }
  delay(10);
}






