/*
 XBee Remote, demo 1
 Author: Harry Johnson
 This is the basic code, intended for combination with the XBee Remote Control sold by KippKitts 
 http://kippkitts.com
 http://github.com/hjohnson/Rover
 
 This code is released into the Public Domain, you're free to use it however you wish. (The hardware design, however, remains under CC BY/SA)
 
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
 8: Switch 6, joystick button.
 9: XBee Comm enable, active low.
 10: LED Latch Enable, not on the SPI bus.
 11: Latch DS, data pin
 12: Latch ST_CP Latchpin
 13: Latch SH_CP clockPin.
 
 Analog Pins:
 0: Vertical ADC input from joystick. 
 1: Horizontal ADC input from joystick.
 
 Seven Segment Assignments
 Q0: Decimal, LSB
 Q1: B
 Q2: A
 Q3: C
 Q4: D
 Q5: E
 Q6: F
 Q7: G, MSB
 
 Errata: 
 Arduino must have changed some of the timing requirements for bootloading with the Uno, because using the Uno firmware on this board seems to provoke bootloading issues on some computers. 
 As such, I'd suggest sticking to using the "Duemilanove (w/328P)" board option when using the full custom board.
 Also, the board runs at 3.3V, so it'd be a good idea to use a 3.3V FTDI cable. However, in theory the XBee is protected no matter what, due to the 3.3V powered 74HC125.
 */
byte segVal[16]; //to store the 16 patterns for the 7-segment display

const byte xbeeEnablePin = 9;
const byte sevenSegEnablePin = 10;
const byte dataPin = 11;
const byte latchPin = 12;
const byte clockPin = 13;
const byte verticalADC = 0; //vertical joystick input.
const byte horizontalADC = 1; //horizontal joystick input.

byte previousPinValue = 0xFF; //this case is impossible to get, even by pressing all of the buttons. 
byte pinValue = 0; //see getPins
byte changeCounter = 0; //to increment display.

//Utility methods. You shouldn't have to modify these.
void displayValue(byte value) { //use this function to display a value (0-0x0F) on the 7-segment display!
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

void enableXBee() { //enable communication with the Xbee through the buffer.
  pinMode(xbeeEnablePin, OUTPUT);
  digitalWrite(8, LOW); //active low enable XBee communications pin.
  delay(1);
}

void disableXBee() { //disconnects the Xbee from the remote using the buffer's output enable pins.
  pinMode(xbeeEnablePin, INPUT); //switch XBee enable pin to high-impedance, external pull-up resistor keeps it disabled.
  delay(1);
}

byte getPins() { //easy way to do switch statements based on pin states. Not required, though.
  byte result =0;
  for(int ii = 0; ii<=6; ii++) {
    result |= ((1-((byte)digitalRead(ii+2))) << ii); //if appropriate pin is LOW (active low switch), then activate appropriate bit.
  }
  return result;
}

//end of utility methods.

//Various setup functions. 7-segment, pin modes, XBee, etc.
void setup() {
  segSetup(); //setup the segment values, etc. 
  for(byte ii = 2; ii<=8; ii++) { //switches 0-6 inputs.
    pinMode(ii, INPUT); //input
    digitalWrite(ii, HIGH); //internal pullups enabled.
  }
  
  Serial.begin(9600); //XBee uses 9600 bits per second.
  enableXBee(); //enable the XBee pins.
  
  Serial.println("Remote Control Ready");
}

//Program here!
void loop() {
  previousPinValue = pinValue; //so that when it reads the new pin values, it can tell if there's been a change.
  pinValue = getPins(); //update pin values
  if(pinValue != previousPinValue) { //only send data on change of switches pressed, to prevent data overload.
    Serial.println(pinValue);
    if((pinValue & (1<<6))==1) { //if the joystick button was pressed, print current position
      Serial.print("Vertical: ");
      Serial.println(analogRead(verticalADC));
      Serial.print("Horizontal: ");
      Serial.println(analogRead(horizontalADC));
    }
    changeCounter++;
    if(changeCounter ==0x10) changeCounter = 0; //max we can display is 0x0F
    displayValue(changeCounter); //display the number of times the buttons have been pressed/released. 
  }
}








