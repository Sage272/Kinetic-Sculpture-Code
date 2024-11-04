#include "Motor.h"
#include "Ultrasonic.h"
#include "LEDStrip.h"
#include "Disc.h"

const int LED_PIN = 13; /**< Pin that the red LED on the left corner of the PCB that is attached to */

const int BIT_1_SWITCH_PIN = 63; /**< Pin that bit1 switch is attached to */
const int BIT_2_SWITCH_PIN = 64; /**< Pin that bit2 switch is attached to */
const int BIT_3_SWITCH_PIN = 65; /**< Pin that bit3 switch is attached to */

/**
   Motor Variable initialization
*/
const int RIGHT_MOTOR_DIRECTION_PIN = 14; /**< Pin that controls the direction of the right motor */
const int RIGHT_MOTOR_PWM_PIN = 12; /**< Pin that sends PWM power to the right motor */
const int RIGHT_MOTOR_TACHOMETER = 18; /**< Pin that the tachometer for the right motor is attached to */
Motor rightMotor(RIGHT_MOTOR_DIRECTION_PIN, RIGHT_MOTOR_PWM_PIN, RIGHT_MOTOR_TACHOMETER); /**< Motor that represents the right motor on the PCB (controls back disc) */

const int LEFT_MOTOR_DIRECTION_PIN = 15; /**< Pin that controls the direction of the left motor */
const int LEFT_MOTOR_PWM_PIN = 11; /**< Pin that sends PWM power to the left motor */
const int LEFT_MOTOR_TACHOMETER = 19; /**< Pin that the tachometer for the left motor is attached to */
Motor leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_PWM_PIN, LEFT_MOTOR_TACHOMETER); /**< Motor that represents the left motor on the PCB (controls front disc) */

/**
   Disc declarations, each includes a motor and tachometer for accurate control
*/
Disc backDisc(&rightMotor); /**< Back disc object to control the back disc */
Disc frontDisc(&leftMotor); /**< Front disc object to control the front disc */

/**
   LEDStrip declarations
*/
const int BACKLIGHT_RED_PIN = 6; /**< Attached pin on Arduino that controls red on LEDStrip */
const int BACKLIGHT_GREEN_PIN = 8; /**< Attached pin on Arduino that controls green on LEDStrip */
const int BACKLIGHT_BLUE_PIN = 7; /**< Attached pin on Arduino that controls blue on LEDStrip */
LEDStrip leds(BACKLIGHT_RED_PIN, BACKLIGHT_GREEN_PIN, BACKLIGHT_BLUE_PIN); /**< LEDStrip that controls the backlighting */

/**
   Ultrasonic declaration
*/
const int US_TRIGGER = 4; /**< Trigger pin that the ultrasonic sensor is attached to */
const int US_ECHO = 3; /**< Echo pin that the ultrasonic sensor is attached to */
Ultrasonic ultrasonicSensor(US_TRIGGER, US_ECHO); /**< Ultrasonic object to interact with the ultrasonic sensor */

////////
//My own:
int blue[] = {0, 82, 171};
int green[] = {10, 199, 142};
int purple[] = {160, 95, 245};
int pink[] = {171, 3, 98};
int yellow[] = {245, 192, 86};

int lightGreen[] = {150, 214, 165};
///////

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BIT_1_SWITCH_PIN, INPUT_PULLUP);
  pinMode(BIT_2_SWITCH_PIN, INPUT_PULLUP);
  pinMode(BIT_3_SWITCH_PIN, INPUT_PULLUP);

  //These two lines make it so every time the tachometer senses a change, the rpm is updated.
  attachInterrupt(digitalPinToInterrupt(frontDisc.motor->getTachAttachedPin()), rightTachometerInterruptServiceRoutine, RISING);
  attachInterrupt(digitalPinToInterrupt(backDisc.motor->getTachAttachedPin()), leftTachometerInterruptServiceRoutine, RISING);

  leds.backlightStartTransition(255, 255, 255, 25);
  //set base position back to 0.0;
  //leftMotor.setToStartPosition();
  //rightMotor.setToStartPosition();
}

void rightTachometerInterruptServiceRoutine() {
  frontDisc.setISR();
}

void leftTachometerInterruptServiceRoutine() {
  backDisc.setISR();
}

void loop() {
  digitalWrite(LED_PIN, 0);
  int currentState = getSwitchState();

  switch (currentState)
  {
    case 0:
      mode0();
      break;
    case 1:
      mode1();
      break;
    case 2:
      mode2();
      break;
    case 3:
      mode3();
      break;
    case 4:
      mode4();
      break;
    case 5:
      mode5();
      break;
    case 6:
      mode6();
      break;
    case 7:
      mode7();
      break;
  }
  
}

/**
   You should have a working implemenation of this function from Unit 4.
*/
int getSwitchState()
{
  return digitalRead(BIT_3_SWITCH_PIN) * 4 + digitalRead(BIT_2_SWITCH_PIN) * 2 + digitalRead(BIT_1_SWITCH_PIN);
}

/*
  Soothing Nightlight Mode. LED Backlight Only.
*/
void mode0() {

  //leds.backlightStartTransition(255, 255, 255, 25);
  leftMotor.turnOffMotor();
  rightMotor.turnOffMotor();
  
  int delayAmt = 10000;
  //fade purple -> blue -> green -> blue -> start over
  transitionToColorWithArray(purple, delayAmt * 2);
  transitionToColorWithArray(blue, delayAmt);
  transitionToColorWithArray(green, delayAmt * 2);
  transitionToColorWithArray(blue, delayAmt);
}

/*
  Mellow Mode. Slow RPM's and calm fading LEDs.
*/
void mode1() {
  int delayAmt = 10000;
  leftMotor.runMotor(70);
  rightMotor.runMotor(65);
  transitionToColorWithArray(lightGreen, delayAmt * 2);
  transitionToColorWithArray(pink, delayAmt * 2);
  transitionToColorWithArray(lightGreen, delayAmt * 2);
}

/*
  Exciting Mode. Faster speeds and color transitions.
*/
void mode2() {
  int delayMin = 2500;
  int delayMax = 5000;
  int fastSpeed = 100;
  //speedily rotate them in opposite directions
  //quick color changes matched to speed
  transitionToColorWithArray(pink, 2);
  rightMotor.runMotor(fastSpeed);
  leftMotor.runMotor(-fastSpeed);
  delay(random(delayMin,delayMax));
  //peer review implementation
  leftMotor.runMotor(0);
  transitionToColorWithArray(yellow, 2);
  delay(random(delayMin/5,delayMax/5));
  //
  transitionToColorWithArray(lightGreen, 2);
  rightMotor.runMotor(-fastSpeed);
  leftMotor.runMotor(fastSpeed);
  delay(random(delayMin,delayMax));
  //peer review implementation
  rightMotor.runMotor(0);
  transitionToColorWithArray(yellow, 2);
  delay(random(delayMin/5,delayMax/5));
  //
}

/*
  Motion Detecting Mode. Uses the ultrasonic sensor in a dynamic way.
*/
void mode3() {
  //get progressively faster and more intense as distance decreases
  //start spinning oppistes slowly, and fade color accordingly
  int delayAmt = 1000;
  int distance = findAvgDistance();
  if (distance < 15)
  {
    leds.backlightStartTransition(45, 255, 3, delayAmt);
  }
  else if (distance < 30)
  {
    leds.backlightStartTransition(69, 255, 32, delayAmt);
  }
  else if (distance < 45)
  {
    leds.backlightStartTransition(94, 255, 62, delayAmt);
  }
  else if (distance < 60)
  {
    leds.backlightStartTransition(126, 255, 101, delayAmt);
  }
  else if (distance < 75)
  {
    leds.backlightStartTransition(146, 255, 125, delayAmt);
  }
  else if (distance < 90)
  {
    leds.backlightStartTransition(181, 255, 167, delayAmt);
  }
  else if (distance < 105)
  {
    leds.backlightStartTransition(214, 255, 207, delayAmt);
  }
  else
  {
    leds.backlightStartTransition(255, 255, 255, delayAmt);
  }
}

/*
  CYOS: Create Your Own Show!
*/
void mode4() {
  int motorSpeed = 100;
  int delayAmt = 6;
  
  leftMotor.runMotor(motorSpeed);
  rightMotor.runMotor(-motorSpeed);
  
  leds.setColor(255, 0, 0);
  delay(delayAmt);
  leds.setColor(0, 255, 0);
  delay(delayAmt);
  leds.setColor(0, 0, 255);
  delay(delayAmt);
}

/*
  CYOS: Create Your Own Show!
*/
void mode5() {
  //positions
  int delayMin = 500;
  int delayMax = 2000;
  double minPosition = 0;
  double maxPosition = 10;
  //minPosition, maxPosition
  leftMotor.setDiskToPosition(leftMotor.diskPositionInEighths(random(minPosition, maxPosition)/10.0, false));
  delay(random(delayMin, delayMax));
  rightMotor.setDiskToPosition(rightMotor.diskPositionInEighths(random(minPosition, maxPosition)/10.0, false));
  delay(random(delayMin, delayMax));
}

/*
  Ultrasonic Sensor Diagnostic
  Activate Red LED's between 0-30cm range, Green between 30-60cm, Blue between 60-100cm, and White above 100cm.
*/
void mode6()
{
  int delayAmt = 100;
  int distance = findAvgDistance();
  if (distance < 30)
  {
    leds.backlightStartTransition(255, 0, 0, delayAmt);
  }
  else if (distance < 60)
  {
    leds.backlightStartTransition(0, 255, 0, delayAmt);
  }
  else if (distance < 100)
  {
    leds.backlightStartTransition(0, 0, 255, delayAmt);
  }
  else
  {
    leds.backlightStartTransition(255, 255, 255, delayAmt);
  }
}

/**
   Motor testing
*/
void mode7()
{
  int maxAmt = 90;
  int minAmt = 0;
  int delayAmt = 30;
  for (int i = minAmt; i <= maxAmt; i++)
  {
    leftMotor.runMotor(i);
    rightMotor.runMotor(-i);
    delay(delayAmt);
  }
  for (int i = maxAmt; i >= minAmt; i--)
  {
    leftMotor.runMotor(i);
    rightMotor.runMotor(-i);
    delay(delayAmt);
  }
  delay(1000);
  for (int i = minAmt; i >= -maxAmt; i--)
  {
    leftMotor.runMotor(i);
    rightMotor.runMotor(-i);
    delay(delayAmt);
  }
  for (int i = -maxAmt; i <= minAmt; i++)
  {
    leftMotor.runMotor(i);
    rightMotor.runMotor(-i);
    delay(delayAmt);
  }
  delay(1000);
}

//my own
void transitionToColorWithArray(int color[], int delayAmt)
{
  leds.backlightStartTransition(color[0], color[1], color[2], delayAmt);
}

int findAvgDistance()
{
  int outlierCount = 0;
  int avgDistance = 0;
  int prevDistance;
  for(int i = 0; i < 10; i++)
  {
    int newDistance = ultrasonicSensor.getDistanceInCM();
    if(i == 0 || newDistance < prevDistance * 2)
    {
      prevDistance = newDistance;
      avgDistance += newDistance;
      continue;
    }
    outlierCount++;
  }
  return avgDistance/(10 - outlierCount);
}
