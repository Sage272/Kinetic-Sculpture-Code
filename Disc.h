#pragma once
#include "Motor.h"

/**
 * Disc Class
 *
 * Class that represents a disc (front or back disc) on the Kinetic Sculpture.
 * Written by Wade Varesio & Courtney McIntosh 9/21/2018
 **/
class Disc{
  public:
    Motor *motor; /**<The Disc's attached motor */

   /**
    *  Constructor for a Disc Object
    *
    *  Constructs a Disc Object which represents the attached motor (and tachometer). Disc basically abstracts Motor to make controlling the Kinetic Sculpture more intuitive.
    *
    *  @param attachedMotor - the motor object that runs the disc object
    */
    Disc(Motor* attachedMotor) {
      motor = attachedMotor;
      motor->setupMotor();
    }

    /**
     *  Updates power sent to motors
     *
     *  Checks if the current RPM of each motor is greater or less than the desired RPM.
     *  If it is less, the motor is sped up, and if it is greater, the motor slows down is slowed down.
     *  It is also ensured that the motor will spin in the right direction by sending a positive or negative percentage when needed.
     *  jump is by default set to .5
     *
     *  @param desiredRpm the desired RPM at which the motor whould spin
     *  @param jump the desired jump at which the percentage of power sent to the motor iterates
     */
    void updateRPM(float desiredRpm) { //maxDiscRpm I measured was ~32rpm
        delay(100);
        desiredRpm = max(-30, min(30, desiredRpm)); //constrains rpm to be within -30 and 30
        float tachRpm = motor->getRotationsPerMinute();
        double motorPercentage = motor->getPercentage();
        float desiredDiscRpm = desiredRpm;
        long desiredTachRpm = desiredDiscRpm * 1250; //converts DiscRpm to Tach rpm
        int tachBoundary = 50;
        long desireMin = desiredTachRpm - tachBoundary; //Creates a lower bound on rpm
        long desireMax = desiredTachRpm + tachBoundary; //Creates an upper bound on rpm
        //Almost works, goes wonky on negative (CCW) rpm

        while (tachRpm > desireMax || tachRpm < desireMin){ //Runs until Rpm is within 10% of desired value
          delay(100);
          tachRpm = motor->getRotationsPerMinute();
          motorPercentage = motor->getPercentage();
          //if (motorPercentage == 0){
          //  motor->runMotor(random(0,100) * (abs(desiredTachRpm)/desiredTachRpm));
          //}
          byte dir = motor->getDirection();
          if (dir == 0){
            tachRpm = -tachRpm;
          }
          if (desiredTachRpm > tachRpm){
            motor->increasePercentage(double((desiredTachRpm-tachRpm)/10000));
          }
          else if(desiredTachRpm < tachRpm){
            motor->decreasePercentage(double((tachRpm-desiredTachRpm)/10000));
          }
        }
    }
    /**
     * Get the rotations per minute
     *
     * @returns returns an unsigned long (a non-negative number that can include a fractional part)
     **/
    unsigned long getRotationsPerMinute(){
      return motor->getRotationsPerMinute();
    }

    void setISR(){
      motor->interruptServiceRoutine();
    }

    byte getAssociatedTachPin(){
      return motor->getTachAttachedPin();
    }

    /**
     * Forwards the call to turnOnMotor() to the appropriate Motor object using pointers
     *
     * @see Motor::turnOnMotor()
     **/
    void turnOnMotor(){
      motor->turnOnMotor();
    }

    /**
     * Forwards the call to turnOffMotor() to the appropriate Motor object using pointers
     *
     * @see Motor::turnOffMotor()
     **/
    void turnOffMotor(){
      motor->turnOffMotor();
    }

    /**
     * Forwards the call to runMotor() to the appropriate Motor object using pointers
     *
     * @param percent the percent at which you want the motor to run at
     * @see Motor::runMotor()
     **/
    void runDisc(double percent){
      motor->runMotor(percent);
    }

  private:
};
