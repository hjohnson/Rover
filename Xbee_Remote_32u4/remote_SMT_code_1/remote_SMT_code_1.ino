/*
Surface Mount Rover XBee Remote Control
UNPROVEN, UNTESTED CODE
Harry Johnson
CC BY/SA (Until I have the time to set up a better license).
Intended for use with custom 32u4 based remote control.
*/

//Variable Declaration
byte segVal[16] =    { ~0x7E, ~0x30, ~0x6D, ~0x79, ~0x33, ~0x5B, ~0x5F, ~0x70, ~0x7F, ~0x7B, ~0x77, ~0x1F, ~0x4E, ~0x3D, ~0x4F, ~0x47}; //7-segment output patterns
byte segPins[7] =    { 14, 16, 2, 3, 11, 17, 15}; //7-segment LED output pins.
byte switchPins[8] = { 5, 13, 18, 19, 20, 21, 22, 23}; //8 input pins
byte robotState = -1; //0: stopped. 1: forward. 2: backward.

//7-segment control calls
void enableSevenSeg() {
  for(int ii = 0; ii<7; ii++) {
    pinMode(segPins[ii], OUTPUT);
    digitalWrite(segPins[ii], HIGH); 
  }
  digitalWrite(12, HIGH); //enable the MOSFET that controls the seven segment display.  
}

void disableSevenSeg() {
  for(int ii = 0; ii<7; ii++) {
    pinMode(segPins[ii], INPUT); //all pins Hi-Z
    digitalWrite(segPins[ii], LOW); 
  }
  digitalWrite(12, LOW); //disable seven-segment MOSFET.  
}

void displayRawValue(byte value) { 
  for(int ii = 0; ii<7; ii++) {  //iterate through the 7 pins, masking the input value with each individual pin. This is raw abcdefg value.
    digitalWrite(segPins[ii], ((value&(1<<ii))>>ii)); 
  }  
}

void displayValue(byte value) { //now input 0-F gets mapped to abcdefg value, and displayed.
  displayRawValue(segVal[value]); 
}

//End 7-segment control calls.
byte getPins() {
  byte toReturn;
  for(byte ii = 0; ii<8; ii++) {
    toReturn |= (digitalRead(switchPins[ii])<<ii);
  }
  return toReturn;
}


void setup() {
  Serial1.begin(9600); //Xbee serial connection.
  enableSevenSeg(); //enable the 7-segment displays.
  displayValue(0);
  
  for(byte ii = 0; ii<8; ii++) {
    pinMode(switchPins[ii], INPUT); //all switch pins to input
    digitalWrite(switchPins[ii], HIGH); //internal pullup enable.
  }
  
  pinMode(0, INPUT); //RX input
  pinMode(1, OUTPUT); //TX output
}

void loop() {
unsigned int value = getPins();
 switch(value) {
  case 0: if(robotState != 0) { } robotState = 0; break; //no buttons
  case 1: if(robotState != 1) { Serial1.print("W"); displayValue(1); } robotState = 1; break; //button 0 = stop
  case 2: if(robotState != 2) { Serial1.print("F"); displayValue(2); } robotState = 2; break; //button 1 = forward
  case 4: Serial1.print("A"); displayValue(4);  while(getPins()==4); Serial1.print("F"); break; //button 2 = backward.
  case 8: Serial1.print("D"); displayValue(4);  while(getPins()==8); Serial1.print("F"); break; //button 3 = backward.
  case 16: if(robotState != 16) { Serial1.print("T"); displayValue(0xA); } robotState = 16; break; //button 4 = speed up
  case 32: if(robotState != 32) { Serial1.print("Y"); displayValue(0xB); } robotState = 32; break; //button 5 = slow down.
  case 64: Serial1.print("U"); displayValue(4);  while(getPins()==64); Serial1.print("I"); break; //button 6 = lever switch off.
  default: if(robotState != 0) { Serial1.print("F"); displayValue(0); } robotState = 0; break; //unknown combination
 } 
 delay(10);
}


