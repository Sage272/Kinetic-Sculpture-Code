#pragma once

/**
 * Ultrasonic object class
 *
 * Class to easily interact with the ultrasonic sensor written by Wade Varesio 9/14/2018
 */
class Ultrasonic {
  public:

    /**
     * Contructs an Ultrasonic object
     *
     * @param trigger the pin that the ultrasonic trigger pin is associated with.
     * @param echo the pin that the ultrasonic echo pin is associated with.
     */
    Ultrasonic(byte trigger, byte echo) {
      triggerPin = trigger;
      pinMode(triggerPin, OUTPUT);

      echoPin = echo;
      pinMode(echoPin, INPUT);

      IN_PROGRESS = false;
    }

    /**
     * Starts a measurement on the ultrasonic sensor.
     *
     * Start a measurement by clearing the trigger pin, then setting to high and reading from the echoPin
     */
    void startMeasurement() {
      IN_PROGRESS = true;

      // Clear triggerPin
      digitalWrite(triggerPin, LOW);
      delayMicroseconds(2);

      // Sets the triggerPin on HIGH state for 10 micro seconds
      digitalWrite(triggerPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(triggerPin, LOW);

      // Reads the echoPin, returns the sound wave travel time in microseconds (US)
      duration = pulseIn(echoPin, HIGH);

      calculateDistance();
    }

    /**
     * Return distance from the ultrasonic sensor in CM
     */
    int getDistanceInCM(){
      startMeasurement();
      return distance;
    }

    /**
     * Return the distance in inches
     */
    float getDistanceInInches(){
      startMeasurement();
      return distance * 2.54;
    }

    bool inProgress(){
      return IN_PROGRESS;
    }

  private:
    const float speedOfSoundInCMPerUS = 0.034;
    byte triggerPin;
    byte echoPin;
    long duration;
    int distance;
    bool IN_PROGRESS;

    /**
     * Calculate distance from ultrasonic in CM
     *
     */
    void calculateDistance() {
      distance = duration * speedOfSoundInCMPerUS / 2;
    }
};
