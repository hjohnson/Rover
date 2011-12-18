//rovercard_speedadjust: the whole hog. Bumpers, Remote Control, Phototransistors
#include <Tone.h> //tone functions, allows output of square waves

const int INHIB_LFT = 4; //ON-OFF left, when LOW, motors inhibited
const int INHIB_RGHT = 5; //ON-OFF right, when LOW, motors inhibited
const int DIR_LFT = 6; //direction left, when HIGH, motor reversed
const int DIR_RGHT = 7; //direction right, when HIGH, motor reversed
const int LVR_LFT = 8; //bumper switch left, high when pressed
const int LVR_RGHT = 9; //bumper switch right, high when pressed
const int PHOT_LFT = 1; //phototransistor left, analog pin 1
const int PHOT_RGHT = 0; //phototransistor right, analog pin 0
const int V_SWTCH = 2; //vulnerability switch
const int CLK_LFT = 10; //clock output left
const int CLK_RGHT = 11; //clock output right

char tmpRead = ' ';  //temporary serial value 
int user_stopped = 0; //has the user stopped the rover using the remote?
unsigned int speedL = 3000;
unsigned int speedR = 3000;
Tone velocityL; //actually speed of left
Tone velocityR; //actually speed of right

const int thet_diff_LO =  -150; //
const int thet_diff_HI = 150; //
const int thet_summ_LO =  175; 
const int thet_summ_HI =  1000; //4 volts.

int summ = 0;
int diff = 0;
int photReadLFT = 0;
int photReadRGHT = 0;
int lvr_enabled = 1;

void setup() {
  Serial.begin(9600);
  for(int ii =4; ii<=11; ii++) {
    pinMode(ii, OUTPUT);   //set up all of the inhibs, clock, etc as output
  }
  pinMode(V_SWTCH, INPUT); //V switch input
  pinMode(LVR_LFT, INPUT);
  pinMode(LVR_RGHT, INPUT);
   digitalWrite(V_SWTCH, LOW); //enable internal pullup.
  digitalWrite(LVR_LFT, HIGH);
  digitalWrite(LVR_RGHT, HIGH);
  
  velocityL.begin(CLK_LFT); //set up frequency output pins. don't need to do this again
  velocityR.begin(CLK_RGHT); 
  velocityL.play(speedL);  //3KHz square wave 
  velocityR.play(speedR); //3Khz square wave. These are changeable live, just be careful.
  
  digitalWrite(INHIB_LFT, LOW); //inhibit both motors
  digitalWrite(INHIB_RGHT, LOW);
  user_stopped = 1;
  attachInterrupt(0, interruptroutine, LOW);  //when the V-switch is flicked to the side, call interruptroutine function

}

void loop() {
  if(Serial.available()>0) {
    tmpRead = Serial.read();
    switch(tmpRead) {
   
      case 'F': 
      digitalWrite(INHIB_LFT, LOW); 
      digitalWrite(INHIB_RGHT, LOW);
     user_stopped = 1; 
      break; //turns off both motors
   
    case 'W': 
      digitalWrite(INHIB_LFT, HIGH); 
      digitalWrite(INHIB_RGHT, HIGH); 
      digitalWrite(DIR_LFT, LOW); 
      digitalWrite(DIR_RGHT, LOW); 
      user_stopped = 0;
      break; //forward
    
    case 'S': 
      digitalWrite(INHIB_LFT, HIGH); 
      digitalWrite(INHIB_RGHT, HIGH); 
      digitalWrite(DIR_LFT, HIGH); 
      digitalWrite(DIR_RGHT, HIGH); 
      user_stopped = 0;
      break; //backward
    
    case 'A': 
       digitalWrite(INHIB_LFT, HIGH);
       digitalWrite(INHIB_RGHT, LOW); //disable the right motor 
       digitalWrite(DIR_LFT, HIGH); 
       digitalWrite(DIR_RGHT, HIGH); 
       user_stopped = 0;
       break;
       
       case 'D': 
       digitalWrite(INHIB_LFT, LOW); //disable the left motor 
       digitalWrite(INHIB_RGHT, HIGH);
       digitalWrite(DIR_LFT, HIGH); 
       digitalWrite(DIR_RGHT, HIGH); //back up.
       user_stopped = 0;
       break;
       
    case 'T': 
      speedL += 500;
      speedR += 500;
      Serial.print("Left: ");
      Serial.println(speedL);
      Serial.print("Right: ");
      Serial.println(speedR);
      velocityL.play(speedL);  
      velocityR.play(speedR);
    break;
    
     case 'Y': 
      speedL -= 500;
      speedR -= 500;
      Serial.print("Left: ");
      Serial.println(speedL);
      Serial.print("Right: ");
      Serial.println(speedR);
      velocityL.play(speedL);  
      velocityR.play(speedR);
    break;
    
      case 'U': lvr_enabled = 0; break;
      case 'I': lvr_enabled = 1; break;
    
    default: 
      break;
    } 
  }
  if((digitalRead(LVR_LFT)==HIGH) && (user_stopped ==0) && (lvr_enabled == 1)) {
   Serial.println("Left Lever"); 
      digitalWrite(DIR_LFT, HIGH); //back up 
      digitalWrite(DIR_RGHT, HIGH); 
      delay(750);
      digitalWrite(DIR_LFT, LOW);  //turn right
     delay(500);
     digitalWrite(DIR_RGHT, LOW); //go back to going forward.
    
  }
  
   if((digitalRead(LVR_RGHT)==HIGH) && (user_stopped ==0) && (lvr_enabled ==1)) {
   Serial.println("Right Lever"); 
      digitalWrite(DIR_LFT, HIGH); //back up
      digitalWrite(DIR_RGHT, HIGH); 
      delay(750);
      digitalWrite(DIR_RGHT, LOW); //turn left
     delay(500);
     digitalWrite(DIR_LFT, LOW); //back to going forward
  }
  
  if(user_stopped == 0) { //Phototransistor code
   photReadLFT = analogRead(PHOT_LFT); //read left phototransistor, 0-1023
   photReadRGHT= analogRead(PHOT_RGHT); //right phototransistor.
   summ = photReadLFT + photReadRGHT; //sum of two values
   diff = photReadLFT - photReadRGHT; //difference of two values
   Serial.print("Sum: ");
   Serial.println(summ);
   Serial.print("Diff: ");
   Serial.println(diff);
   if((summ < thet_summ_HI) && (summ > thet_summ_LO)) { //if the sum is too low, the rover is too far away to care, if the sum is too high, the robot is too close to care.
     
     if(diff < thet_diff_LO) { //if the robot is skewed too far to the left.
       Serial.println("turning right"); 
       digitalWrite(INHIB_RGHT, LOW); //turn off right motor, robot turns right.
       while(diff < thet_diff_LO) { //wait until centered. 
         photReadLFT = analogRead(PHOT_LFT); //updating constantly.
         photReadRGHT= analogRead(PHOT_RGHT);
         diff = photReadLFT - photReadRGHT;
       }
       digitalWrite(INHIB_RGHT, HIGH); //go back to going forward.
       Serial.println("Going Forward");
     }
     
      if(diff > thet_diff_HI) { //robot is skewed too far to the right 
       digitalWrite(INHIB_LFT, LOW);  //turn left.
       while(diff > thet_diff_HI) { //until centered
         photReadLFT = analogRead(PHOT_LFT);
         photReadRGHT= analogRead(PHOT_RGHT);
         diff = photReadLFT - photReadRGHT;
       } 
       digitalWrite(INHIB_LFT, HIGH); //go forward.
       Serial.println("Going Forward");
     } 
   }    
  }
}

void interruptroutine() { // V-switch activated!
  digitalWrite(4, LOW); //turn off left motor
  digitalWrite(5, LOW); //turn off right motor
  while(digitalRead(2) == LOW) { //loop until V-switch toggled to center
  } 
}



