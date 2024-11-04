#pragma once

/**
   Class to interact with each motor on the PCB
   The motor class also includes tachometer support which is used to get the RPM that the motor is running at

   Written by Wade Varesio and Courtney McIntosh 9/21/2018
 **/
class Motor {
  public:
    const byte FULL_OFF = 255; /**< The signal is inverted therefore FULL_OFF is 255*/
    const byte FULL_ON =  0; /**< The signal is inverted therefore FULL_OFF is 0*/
    double percentage = 0;

    //my own
    int leftTachometerRotations = 0;
    int rightTachometerRotations = 0;
    int tachometerRotationCountFor25 = 0;
    double discRotation = 0; //gives a decimal of where the disc has rotated from -1 to 0 to 1
    static double startingRotation = 0.0;

    /**
      Constructor to create a Motor object

      @param direction - pin number that controls the direction of the motor
      @param pwm - pin number that the motor PWM pin is on. (Hint: Pulse Width Modulation AKA PWM is how you dim LEDs and control the motor)
      @param tachPin - pin number that the tachometer is attached to on the Arduino
    */
    Motor (byte direction, byte pwm, byte tachPin) {
      directionPin = direction;
      pwmPin = pwm;
      tachometerPin = tachPin;

      /*
         Change the prescaler for timer 1 (used by the motor PWM) to increase the PWM frequency
         to 31250Hz so that it can not be heard  (1=31250Hz, 2=3906Hz, 3=488Hz, 4=122Hz, 5=30.5Hz)
      */
      TCCR1B = (TCCR1B & B11111000) | 1;
      setupMotor();
    }

    /**
       Sets up Motor to spin.

       Sets pinNumber and directionPin to be an OUTPUT, tachometer pin to be an input.
    */
    void setupMotor() {
      pinMode(directionPin, OUTPUT);
      pinMode(pwmPin, OUTPUT);
      analogWrite(pwmPin, FULL_OFF); //Write off to ensure motor is intitialized to be off

      pinMode(tachometerPin, INPUT_PULLUP);
    }

    /**
       Runs a motor at a given speed in perecent

       Makes the disc spin clockwise if percent is positive, and counterclockwise if it is negative.
       Calls the turnOnMotor function at a given percent.

       @param percent -  the percent at which you want to run the motor.
    */
    void runMotor(double percent) {
      if (percent < 0) {
        digitalWrite(directionPin, LOW);
      }
      else {
        digitalWrite(directionPin, HIGH);
      }

      analogWrite(pwmPin, turnPercentIntoPwm(percent));
      percentage = percent;
    }

    /**
       Takes a percentage in the form of a double and turns it into a PWM signal for motor control.

       A percentage value of 0 is off (returns a PWM value of 255), and 100 is fully on (returns a PWM value of 0).

       @param percent - the percentage value to be converted to a PWM value.
       @return returns the conversion of the percent in a PWM value
     **/
    double turnPercentIntoPwm(double percent) {
      percent = max(-100, min(100, percent)); //constrains rpm to be within -100 and 100
      return 255 - (255 * abs(percent) / 100);
    }

    /**
       Turn the motor completely off
     **/
    void turnOffMotor() {
      analogWrite(pwmPin, FULL_OFF);
    }

    /**
       Turn the motor completely on
     **/
    void turnOnMotor() {
      analogWrite(pwmPin, FULL_ON);
    }

    /**
       Increases percentage

       Adds .25 to the value of percentage.
       Calls the runMotor function at this new percentage.

       @param percent -  the amount you want the percentage to ncrease by.
    */
    void increasePercentage(double step) {
      if (percentage < 100) {
        percentage += step;
        runMotor(percentage);
      }
    }

    /**
       Decreases percentage

       Subtracts .25 from the value of percentage.
       Calls the runMotor function at this new percentage.

       @param percent -  the amount you want the percentage to ncrease by.
    */
    void decreasePercentage(double step) {
      if (percentage > -100) {
        percentage -= step;
        runMotor(percentage);
      }
    }
    /**
       Interrupt service routine for the Tachometer

       ISR for when the pin falls low, this is necessary to get a measurement and convert into the RPM of the motor.
     **/
    void interruptServiceRoutine() {
      newTime = micros();
      microsecondsBetweenLines = newTime - timeOfLastMeasurement;
      timeOfLastMeasurement = newTime;
      rotationsPerMinute = 24000000 / microsecondsBetweenLines;
      tachometerRotationCountFor25++;
      
      if(tachometerRotationCountFor25 == 25)
      {
        tachometerRotationCountFor25 = 0;
        //back disk / right motor
        if(directionPin == 14)
        {
          if(digitalRead(directionPin))
          {
            rightTachometerRotations++;
          }
          else
          {
            rightTachometerRotations--;
          }
        }
        else //front disk / left motor
        {
          if(digitalRead(directionPin))
          {
            leftTachometerRotations++;
          }
          else
          {
            leftTachometerRotations--;
          }
        }
        
        //startingRotation = calculateDiscRotations();
      }
    }

    /**
       @return the motors current rotations per minute
    */
    unsigned long getRotationsPerMinute() {
      return rotationsPerMinute;
    }

    /**
       @return the attached Tachometer pin
    */
    byte getTachAttachedPin() {
      return tachometerPin;
    }

    /**
       Get the motors current percentage

       @return percentage the motor is running at
     **/
    double getPercentage() {
      return percentage;
    }

    double getDirection() {
      return digitalRead(directionPin);

    }

    //my own:
    //int lastAmountOfRotations = 0;
    double currentPositionOfRightTachometer = 0;
    double currentPositionOfLeftTachometer = 0;
    
    double calculateDiscRotations()
    {
      if(directionPin == 14)
      {
        currentPositionOfRightTachometer = diskPositionInEighths(rightTachometerRotations/64.0, true); //64 is the gear ratio
        Serial.print("R: "); Serial.println(currentPositionOfRightTachometer);
        //Serial.println(" ");
        return currentPositionOfRightTachometer;
      }
      currentPositionOfLeftTachometer = diskPositionInEighths(leftTachometerRotations/64.0, true); //64 is the gear ratio
      Serial.print("L: "); Serial.println(currentPositionOfLeftTachometer);
      //Serial.println(" ");
      return currentPositionOfLeftTachometer;
    }

    // for the disk: 1 = front disk, 0 = back disk
    // pin 14 == back disk, pin 15 == front disk
    void setDiskToPosition(double wantedPosition)
    {
      double currentPosition = calculateDiscRotations();
      wantedPosition += currentPosition;
      while(currentPosition != wantedPosition)
      {
        Serial.print(currentPosition); Serial.print(" wanted: "); Serial.println(wantedPosition);
        //Serial.println(" ");
        if(wantedPosition > calculateDiscRotations())
        {
          runMotor(50);
        }
        else
        {
          runMotor(-50);
        }
        currentPosition = calculateDiscRotations();
      }
      //runMotor(0);
      turnOffMotor();
      //setCurrentPosition();
    }

//    void setCurrentPosition()
//    {
//      startingRotation = calculateDiscRotations();
//    }
    
    void setToStartPosition()
    {
      setDiskToPosition(0.0);
    }

    double diskPositionInEighths(double currentDiskPosition, bool withMainInt)
    {
      double eighthsFraction = 0;
      int mainInt = (int)currentDiskPosition;
      int decimals = (currentDiskPosition - mainInt) * 100;
      if(decimals <= 12)
      {
        eighthsFraction = 0.125;
      }
      else if (decimals <= 25)
      {
        eighthsFraction = 0.25;
      }
      else if (decimals <= 37)
      {
        eighthsFraction = 0.375;
      }
      else if (decimals <= 50)
      {
        eighthsFraction = 0.5;
      }
      else if (decimals <= 62)
      {
        eighthsFraction = 0.625;
      }
      else if (decimals <= 75)
      {
        eighthsFraction = 0.75;
      }
      else if (decimals <= 87)
      {
        eighthsFraction = 0.875;
      }
      else
      {
        eighthsFraction = 1.0;
      }
      if(currentDiskPosition < 0)
      {
        eighthsFraction *= -1;
      }
      if(withMainInt)
      {
        return mainInt + eighthsFraction;
      }
      
      return eighthsFraction;
    }

  private:
    byte pwmPin;
    byte directionPin, tachometerPin;
    unsigned long rotationsPerMinute;
    unsigned long newTime;
    unsigned long timeOfLastMeasurement;
    unsigned long microsecondsBetweenLines;
};
