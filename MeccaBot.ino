#include <MeccaBrain.h>
#include <MusicalNotes.h>

//To try this example you'll need the following:
//0) Meccanoid G15 in its human form
//1) Any Arduino
//2) Motor shield or motor driver, to drive robot's wheels. I tried the one based on L293D and it was quite decent
//3) Tamiya Mini female connector with wires - to connect to the robots' battery
//4) Male connectors: CWF-2 to connect one motor and PLS-2 to connect another
//5) 3 resistors of 22 kOhm and 3 resistors of 1 kOhm - to make pullup resistors (according to the manual)
//6) Download the library: http://cdn.meccano.com/open-source/meccanoid-library.zip
//7) You can also check out the reference material, to understand, how do the Smart Modules work
//   http://cdn.meccano.com/open-source/Meccano_SmartModuleProtocols_2015.pdf

//Pins to connect Meccanoids' servos, where chain 1 is left arm, chain 2 is head and chain 3 is right arm
//pins can be any digital pins, not necessary PWM
const int chainPin1 = 3;
const int chainPin2 = 5;
const int chainPin3 = 6;
const int pirPin = 4;
const int speakerPin = 9;
const int ledPin = 13;

//pins to set speed on motor driver
const int leftSpeedPin = 10;
const int rightSpeedPin = 11;

//pins to set direction on motor driver
const int leftDirPin = 12;
const int rightDirPin = 13;

int index = 0;
bool presence = false;
bool timer = true;// -------------------------------------------------------------------------TIMER!!!!
int timerDelay = 10000;// -------------------------------------------------------------------------TIMER!!!!

MeccaBrain chain1(chainPin1); //left
MeccaBrain chain2(chainPin2); //head
MeccaBrain chain3(chainPin3); //right

void beep (int speakerPin, float noteFrequency, long noteDuration) {
  int x;
  // Convert the frequency to microseconds
  float microsecondsPerWave = 1000000 / noteFrequency;
  // Calculate how many HIGH/LOW cycles there are per millisecond
  float millisecondsPerCycle = 1000 / (microsecondsPerWave * 2);
  // Multiply noteDuration * number or cycles per millisecond
  float loopTime = noteDuration * millisecondsPerCycle;
  // Play the note for the calculated loopTime.
  for (x = 0; x < loopTime; x++)
  {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(microsecondsPerWave);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(microsecondsPerWave);
  }
}

void r2D2() {
  beep(speakerPin, note_A7, 100); //A
  beep(speakerPin, note_G7, 100); //G
  beep(speakerPin, note_E7, 100); //E
  beep(speakerPin, note_C7, 100); //C
  beep(speakerPin, note_D7, 100); //D
  beep(speakerPin, note_B7, 100); //B
  beep(speakerPin, note_F7, 100); //F
  beep(speakerPin, note_C8, 100); //C
  beep(speakerPin, note_A7, 100); //A
  beep(speakerPin, note_G7, 100); //G
  beep(speakerPin, note_E7, 100); //E
  beep(speakerPin, note_C7, 100); //C
  beep(speakerPin, note_D7, 100); //D
  beep(speakerPin, note_B7, 100); //B
  beep(speakerPin, note_F7, 100); //F
  beep(speakerPin, note_C8, 100); //C
}

//Commands for driving robots' motors
void turnLeft(int speed)
{
  analogWrite(leftSpeedPin, speed);
  analogWrite(rightSpeedPin, 0);
  digitalWrite(leftDirPin, LOW);
  digitalWrite(rightDirPin, HIGH);
}

void turnRight(int speed)
{
  analogWrite(leftSpeedPin, 0);
  analogWrite(rightSpeedPin, speed);
  digitalWrite(leftDirPin, HIGH);
  digitalWrite(rightDirPin, LOW);
}

void rotateLeft(int speed)
{
  analogWrite(leftSpeedPin, speed);
  analogWrite(rightSpeedPin, speed);
  digitalWrite(leftDirPin, LOW);
  digitalWrite(rightDirPin, HIGH);
}

void rotateRight(int speed)
{
  analogWrite(leftSpeedPin, speed);
  analogWrite(rightSpeedPin, speed);
  digitalWrite(leftDirPin, HIGH);
  digitalWrite(rightDirPin, LOW);
}

void goForward(int speed)
{
  analogWrite(leftSpeedPin, speed);
  analogWrite(rightSpeedPin, speed);
  digitalWrite(leftDirPin, HIGH);
  digitalWrite(rightDirPin, HIGH);
}

void goBackward(int speed)
{
  analogWrite(leftSpeedPin, speed);
  analogWrite(rightSpeedPin, speed);
  digitalWrite(leftDirPin, LOW);
  digitalWrite(rightDirPin, LOW);
}

void stay()
{
  analogWrite(leftSpeedPin, 0);
  analogWrite(rightSpeedPin, 0);
}

//Joints mapping:
//Chain 1 - Left Arm. 1.0 is Arm Pitch, 1.1 is Arm Roll, 1.2 is Elbow
//Chain 2 - Head. 2.0 is Head Yaw, 2.1 is Head Roll, 2.2 is LEDs
//Chain 3 - Right Arm. 3.0 is Arm Pitch, 3.1 is Arm Roll 3.2 is Elbow

const byte LEFT_ARM_PITCH = 0;
const byte LEFT_ARM_ROLL = 1;
const byte LEFT_ARM_ELBOW = 2;
const byte HEAD_YAW = 3;
const byte HEAD_ROLL = 4;
const byte RIGHT_ARM_PITCH = 5;
const byte RIGHT_ARM_ROLL = 6;
const byte RIGHT_ARM_ELBOW = 7;

//jointName is LEFT_ARM_ROLL etc
//pos is 0...255
void setJoint(byte jointName, byte pos)
{
  switch (jointName) {
    case LEFT_ARM_PITCH:
      chain1.setServoPosition(0, pos);
      chain1.communicate();
      break;
    case LEFT_ARM_ROLL:
      chain1.setServoPosition(1, pos);
      chain1.communicate();
      break;
    case LEFT_ARM_ELBOW:
      chain1.setServoPosition(2, pos);
      chain1.communicate();
      break;
    case HEAD_YAW:
      chain2.setServoPosition(0, pos);
      chain2.communicate();
      break;
    case HEAD_ROLL:
      chain2.setServoPosition(1, pos);
      chain2.communicate();
      break;
    case RIGHT_ARM_PITCH:
      chain3.setServoPosition(0, pos);
      chain3.communicate();
      break;
    case RIGHT_ARM_ROLL:
      chain3.setServoPosition(1, pos);
      chain3.communicate();
      break;
    case RIGHT_ARM_ELBOW:
      chain3.setServoPosition(2, pos);
      chain3.communicate();
      break;
  }
}

//Set the color of eye LEDS. red, green and blue are from 0 to 7 (0 - no color, 7 - max color).
//fadetime is from 0 to 7 and means the speed of color change (0 - immediate change, 7 - longest change)
//example: setColor(7,0,0,3) means change color to red with average speed
void setEyesColor(byte red, byte green, byte blue, byte fadetime)
{
  chain2.setLEDColor(red, green, blue, fadetime);
  chain2.communicate();
}

//Servo colors
const byte JOINT_BLACK = 0xF0;
const byte JOINT_RED = 0xF1;
const byte JOINT_GREEN = 0xF2;
const byte JOINT_BROWN = 0xF3;
const byte JOINT_BLUE = 0xF4;
const byte JOINT_VIOLET = 0xF5;
const byte JOINT_SEA = 0xF6;
const byte JOINT_WHITE = 0xF7;

//set the servo color
//for example, setJointColor(RIGHT_ARM_ELBOW, JOINT_VIOLET)
void setJointColor(byte jointName, byte color)
{
  switch (jointName) {
    case LEFT_ARM_PITCH:
      chain1.setServoColor(0, color);
      chain1.communicate();
      break;
    case LEFT_ARM_ROLL:
      chain1.setServoColor(1, color);
      chain1.communicate();
      break;
    case LEFT_ARM_ELBOW:
      chain1.setServoColor(2, color);
      chain1.communicate();
      break;
    case HEAD_YAW:
      chain2.setServoColor(0, color);
      chain2.communicate();
      break;
    case HEAD_ROLL:
      chain2.setServoColor(1, color);
      chain2.communicate();
      break;
    case RIGHT_ARM_PITCH:
      chain3.setServoColor(0, color);
      chain3.communicate();
      break;
    case RIGHT_ARM_ROLL:
      chain3.setServoColor(1, color);
      chain3.communicate();
      break;
    case RIGHT_ARM_ELBOW:
      chain3.setServoColor(2, color);
      chain3.communicate();
      break;
  }
}

void blinkEyes() {
  if (index >= 512) {
    index = 0;
  }

  byte red = (index >> 4) & 0x07;
  byte green = (index >> 2) & 0x07;
  byte blue = index & 0x07;
  setEyesColor(red, green, blue, 0);
  index++;
}

void waveHand(byte arm, int timeD) {
  setJoint(arm, 200);
  delay(timeD);

  setJoint(arm, 230);
  delay(timeD);
}

void handGesture(byte arm, int timeD) {
  setJoint(arm, 190);
  delay(timeD);

  setJoint(arm, 127);
  delay(timeD);
}

void setup() {
  pinMode(chainPin1, OUTPUT);
  pinMode(chainPin2, OUTPUT);
  pinMode(chainPin3, OUTPUT);
  pinMode(leftSpeedPin, OUTPUT);
  pinMode(rightSpeedPin, OUTPUT);
  pinMode(leftDirPin, OUTPUT);
  pinMode(rightDirPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(speakerPin, OUTPUT);
  Serial.begin(9600);
  Serial.println(note_A7);

  //"Discover" all the modules (make them blue-colored instead of green-colored)
  //for some unknown reason, I have to repeat it from time to time
  for (int i = 0; i < 50; i++){
    chain1.communicate();
    chain2.communicate();
    chain3.communicate();
  }
  //delay to be sure that all modules are ready
  //if some module is "not discovered" than it will remain green and later this module will behave strangely
  r2D2();
  delay(2500);
  r2D2();
}

void loop() {
  //  Serial.println("S");
  if(!timer){
    presence = digitalRead(pirPin);
  }else{
    delay(timerDelay);
    presence = true;
  }
  digitalWrite(ledPin, presence);

  //make every joint of the robot a different color just for fun
  //so that we cycle through all the possible colors and joints
  //  for(int i=0; i<7; i++)
  //  {
  //    setJointColor(i, JOINT_BLACK+i);
  //  }
  //
  //  //move forward, backward, left and right
  //  goForward(255);
  //  delay(3000);
  //  stay();
  //  delay(1000);
  //
  //  goBackward(255);
  //  delay(2000);
  //  stay();
  //  delay(1000);
  //
  //  rotateLeft(255);
  //  delay(2000);
  //  stay();
  //  delay(1000);
  //
  //  rotateRight(255);
  //  delay(2000);
  //  stay();
  //  delay(1000);

  if (presence) {
    chain1.communicate();
    chain2.communicate();
    chain3.communicate();
    
    r2D2();
    delay(100);
    
    for (int times = 0; times < 5; times++) {
      blinkEyes();
      delay(100);
      waveHand(LEFT_ARM_ROLL, 500);
    }

    setJoint(RIGHT_ARM_PITCH, 100);
    delay(100);

    for (int times = 0; times < 5; times++) {
      blinkEyes();
      delay(100);
      handGesture(RIGHT_ARM_ELBOW, 500);
    }

    setJoint(RIGHT_ARM_PITCH, 60);
    delay(100);

    r2D2();
    delay(100);

    chain1.communicate();
    chain2.communicate();
    chain3.communicate();
    if(timer){
      presence=false;
    }
  }
}
