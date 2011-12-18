/*
XBee Remote, code
By Harry Johnson
This file is part of Xbee Remote
 
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
 8: XBee Comm enable, active low.
 9: Switch 6
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
 
 EEPROM:
 0: LSB of PAN ID.
 1: MSB of PAN ID.
 
 Switch Assignments: 
 none: stop.
 0: Forward
 1: Backward
 */
#include <EEPROM.h> //to store XBee information.
byte XBeeAddress[2];
byte segVal[16];
byte latchPin = 12;
static byte clockPin = 13;
static byte dataPin = 11;

byte robotState = -1; //0: stopped. 1: forward. 2: backward.
int pinValues = 0;

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
 pinMode(8, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(10, LOW); //enable the whole LCD system. (had to be off since it shares the SPI bus, just in case. Need to redefine SS, though) 
  digitalWrite(latchPin, HIGH);
  displayValue(0xA);
}

//XBee Code
void getPAN_EEPROM() {
  XBeeAddress[0] = EEPROM.read(0);
  XBeeAddress[1] = EEPROM.read(1);
}

void setPAN_EEPROM(word value) {
  XBeeAddress[0] = lowByte(value);
  XBeeAddress[1] = highByte(value);
  EEPROM.write(0, XBeeAddress[0]);
  delay(4); //write takes 3.3ms to complete.
  EEPROM.write(1, XBeeAddress[1]);
  delay(4);
}

void resetPAN() {
  setPAN_EEPROM(0x3332);
  setPAN_XBEE(); 
}

void setPAN_XBEE() {
  enableXBee();
  Serial.begin(9600);
  Serial.print("+++");
  delay(5);
  Serial.print("ATID");
  Serial.print(word(XBeeAddress[1], XBeeAddress[0]), HEX);
  delay(5);
  Serial.print("ATWR");
  delay(5);
  disableXBee();
}

void enableXBee() {
  digitalWrite(8, LOW); //enable XBee communications.
  delay(1);
}

void disableXBee() {
  digitalWrite(8, HIGH); //enable XBee communications.
  delay(1);
}



unsigned int getPins() { //easy way to do switch statements based on pin states. Not required, though.
  unsigned int result =0;
  for(int ii = 0; ii<=5; ii++) {
    result |= ((1-(digitalRead(ii+2))) << ii); //if pin is HIGH (1), then activate appropriate bit.
  }
  result |= ((1-digitalRead(9)) << 6); //handling the straggler.
  return result;
}

//end of utility methods.

//Program here!
void setup() {
  Serial.begin(9600); //XBee uses 9600.
  Serial.flush(); //clear input buffer.
  disableXBee(); //tri-state the pins.

  segSetup(); //setup the segment values, etc. 
  for(byte ii = 2; ii<=7; ii++) { //switches 0-5 inputs.
    pinMode(ii, INPUT); //input
    digitalWrite(ii, HIGH); //internal pullups enabled.
  }
  pinMode(9, INPUT); //switch 6 input
  digitalWrite(9, HIGH);//internal pullup. 
  
  enableXBee(); //enable the XBee pins.
  displayValue(0xF);
  Serial.print("F"); //stop the rover.
 /* for(int jj = 0; jj<16; jj++) {
   displayValue(jj);
  delay(1000); 
  }*/
}

void loop() {
unsigned int value = getPins();
 switch(value) {
  case 0: if(robotState != 0) { } robotState = 0; break; //no buttons
  case 1: if(robotState != 1) { Serial.print("W"); displayValue(1); } robotState = 1; break; //button 0 = stop
  case 2: if(robotState != 2) { Serial.print("F"); displayValue(2); } robotState = 2; break; //button 1 = forward
  case 4: Serial.print("A"); displayValue(4);  while(getPins()==4); Serial.print("F"); break; //button 2 = backward.
  case 8: Serial.print("D"); displayValue(4);  while(getPins()==8); Serial.print("F"); break; //button 2 = backward.
  case 16: if(robotState != 16) { Serial.print("T"); displayValue(0xA); } robotState = 16; break; //button 3 = speed up
  case 32: if(robotState != 32) { Serial.print("Y"); displayValue(0xB); } robotState = 32; break; //button 4 = slow down.
  case 64: Serial.print("U"); displayValue(4);  while(getPins()==64); Serial.print("I"); break; //button 2 = lever switch off.
  default: if(robotState != 0) { Serial.print("F"); displayValue(0); } robotState = 0; break; //unknown combination
 } 
 delay(10);
}




