//rovercard_student: No remote control code, only bumpers.

#include <Tone.h> //tone functions, allows output of square waves

int INHIB_LFT = 4; //ON-OFF left, when LOW, motors inhibited
int INHIB_RGHT = 5; //ON-OFF right, when LOW, motors inhibited
int DIR_LFT = 6; //direction left, when LOW, motor reversed
int DIR_RGHT = 7; //direction right, when LOW, motor reversed
int LVR_LFT = 8; //bumper switch left, high when pressed
int LVR_RGHT = 9; //bumper switch right, high when pressed
int PHOT_LFT = 1; //phototransistor left, analog pin 1
int PHOT_RGHT = 0; //phototransistor right, analog pin 0
int V_SWTCH = 2; //vulnerability switch
int CLK_LFT = 10; //clock output left
int CLK_RGHT = 11; //clock output right

char tmpRead = ' ';  //temporary serial value 
int user_stopped = 0; //has the user stopped the rover using the remote?
Tone velocityL; //actually speed of left
Tone velocityR; //actually speed of right

void setup() {
  Serial.begin(9600);
  for(int ii =4; ii<=11; ii++) {
    pinMode(ii, OUTPUT);   //set up all of the inhibs, clock, etc as output
  }
  pinMode(V_SWTCH, INPUT); //V switch input
  pinMode(LVR_LFT, INPUT);
  pinMode(LVR_RGHT, INPUT);
  digitalWrite(V_SWTCH, LOW); 
  digitalWrite(LVR_LFT, HIGH); //enable internal pullup.
  digitalWrite(LVR_RGHT, HIGH);

  velocityL.begin(CLK_LFT); //set up frequency output pins. don't need to do this again
  velocityR.begin(CLK_RGHT); 
  velocityL.play(2000);  //2KHz square wave 
  velocityR.play(2000); //2Khz square wave. These are changeable live, just be careful.

  digitalWrite(INHIB_LFT, LOW); //inhibit both motors
  digitalWrite(INHIB_RGHT, LOW);
  user_stopped = 1;
  attachInterrupt(0, interruptroutine, LOW);  //when the V-switch is flicked to the side, call interruptroutine function
}

void loop() {
  if((digitalRead(LVR_LFT)==HIGH)) {
    Serial.println("Left Lever");  
    digitalWrite(DIR_LFT, HIGH);  //back up
    digitalWrite(DIR_RGHT, HIGH); 
    delay(750); 
    digitalWrite(DIR_LFT, LOW); //turn right
    delay(500);
    digitalWrite(DIR_RGHT, LOW); //go back to going forward
  }

  if((digitalRead(LVR_RGHT)==HIGH)) {
    Serial.println("Right Lever"); 
    digitalWrite(DIR_LFT, HIGH); //back up
    digitalWrite(DIR_RGHT, HIGH); 
    delay(750);
    digitalWrite(DIR_RGHT, LOW); //turn left
    delay(500);
    digitalWrite(DIR_LFT, LOW); //go back to going forward.
  }

}
//Vulnerability switch interrupt handler. YOU MAY NOT TOUCH THIS.
void interruptroutine() {
  digitalWrite(4, LOW); //turn off left motor
  digitalWrite(5, LOW); //turn off right motor
  while(digitalRead(2) == LOW) { //loop until V-switch toggled to center
  } 
}




