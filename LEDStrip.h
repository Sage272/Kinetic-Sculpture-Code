//#include <Arduino.h>

const byte gammaCorrectionTable[] PROGMEM = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};

/**
 * Class to control the backlights on the Kinetic Sculpture
 * 
 * Written by Wade Varesio and Courtney McIntosh 9/21/2018
 *
 */
class LEDStrip {
  public:

    /**
     * LEDStrip Constructor
     * 
     * Constructs an LEDStrip object which is how the backlighting is controlled on the Kinetic Sculpture
     *
     * @param red the pin that red LED is controlled through
     * @param green the pin that green LED is controlled through
     * @param blue the pin that blue LED is controlled throughS
     *
     */
    LEDStrip(byte red, byte green, byte blue) {
      RED_PIN = red;
      GREEN_PIN = green;
      BLUE_PIN = blue;

      setupLEDStrip();
    }

    /**
     * Smoothly transitions colors to new colors over a given duration
     *
     * @param red the new red value from 0-255
     * @param green the new green value from 0-255
     * @param blue the new blue value from 0-255
     * @param transitionDurationMS amount of time (in MS) for the LEDStrip to transition colors
     */
    void backlightStartTransition(byte red, byte green, byte blue, unsigned long transitionDurationMS) {
      unsigned long startTimeMS = millis();
      unsigned long finishTimeMS = startTimeMS + transitionDurationMS;
      nextRed = red;
      nextGreen = green;
      nextBlue = blue;

      transitionCompleteFlg = false;

      if (finishTimeMS < startTimeMS){
        finishTimeMS = startTimeMS;
      }

      while (transitionCompleteFlg != true){
        if (currentRed < nextRed){
          currentRed += 1;
        }
        else if (currentRed > nextRed){
          currentRed -= 1;
        }
        if (currentGreen < nextGreen){
          currentGreen += 1;
        }
        else if (currentGreen > nextGreen){
          currentGreen -= 1;
        }
        if (currentBlue < nextBlue){
          currentBlue += 1;
        }
        else if (currentBlue > nextBlue){
          currentBlue -= 1;
        }
        setColor(currentRed, currentGreen, currentBlue);
        delay(transitionDurationMS/255);
      }
    }

    /**
     * Change the color of the LEDStrip
     *
     * Will change the color on the LEDStrip without delay or a smooth transition
     * 
     * @param red the red color value you want to change to from 0-255
     * @param green the green color value you want to change to from 0-255
     * @param blue the blue color value you want to change to from 0-255
     */
    void setColor(byte red, byte green, byte blue) {
      currentRed = red;
      currentGreen = green;
      currentBlue = blue;
      analogWrite(RED_PIN, pgm_read_byte(&gammaCorrectionTable[red]));
      analogWrite(GREEN_PIN, pgm_read_byte(&gammaCorrectionTable[green]));
      analogWrite(BLUE_PIN, pgm_read_byte(&gammaCorrectionTable[blue]));

      if (currentRed == nextRed and currentBlue == nextBlue and currentGreen == nextGreen){
        transitionCompleteFlg = true;
      }
    }

  private:
    byte RED_PIN, GREEN_PIN, BLUE_PIN;
    byte nextRed, nextGreen, nextBlue;
    byte currentRed, currentGreen, currentBlue; //Saves current color for transitions
    bool transitionCompleteFlg;

    /**
     * Setup the LEDStrip
     *
     * Sets LEDStrip pins to OUTPUT and show no color on LEDStrip
     */
    void setupLEDStrip() {
      pinMode(RED_PIN, OUTPUT);
      pinMode(GREEN_PIN, OUTPUT);
      pinMode(BLUE_PIN, OUTPUT);
      setColor(0, 0, 0);
    }
};
