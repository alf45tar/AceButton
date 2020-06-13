/*
MIT License

Copyright (c) 2020 Brian T. Park

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ACE_BUTTON_LADDER_BUTTON_CONFIG_H
#define ACE_BUTTON_LADDER_BUTTON_CONFIG_H

#include "ButtonConfig.h"

namespace ace_button {

/**
 * A ButtonConfig that handles an 4-to-2 binary encoder which converts 4 inputs
 * into 2 outputs. In practice, this means that 3 buttons can be handled with 2
 * pins, because the 0th button is used to represent "no button pressed". One
 * easy way to perform the 4-to-2 encoding is to use 2 diodes with 3 switches,
 * so that each switch translates into a 2-bit binary number:
 *
 *  * S0 = 00 = 0 [unavailable, see below]
 *  * S1 = 01 = 1
 *  * S2 = 10 = 2
 *  * S3 = 11 = 3
 *
 * Button S0 cannot be used because the code 00 is used to indicate that no
 * button was pressed.
 */
class LadderButtonConfig : public ButtonConfig {
  public:
    /**
     * Data table for button values
     *
     * The readButton() will use this table to determine which button has been pressed
     * and return the specified identifier.
     */
    typedef struct
    {
      uint16_t  threshold;    //  Average analog value of the button
      uint8_t   tolerance;    //  Valid +/- tolerance range around threshold for a button
      uint8_t   id;           //  Identifier for this button
    } AnalogButtons_t;

    /**
     * @param pinA the pin number representing bit0 of the binary encoder
     * @param defaultReleasedState state of the encoder bit when the button
     * is in the released state. For a pull-up wiring, the state of the pin is
     * HIGH when the button is released. This value is used to configure wiring
     * of the virtual button, so that it matches the wiring of the physical
     * buttons. The LS74148 encoder uses a pull-up wiring, so this should be set
     * HIGH. The default value is HIGH.
     */
    LadderButtonConfig(uint8_t pinA,
        AnalogButtons_t* ab, uint8_t abSize,
        uint8_t defaultReleasedState = HIGH):
      mPinA(pinA),
      mab(ab),
      mabSize(abSize),
      mPressedState(defaultReleasedState ^ 0x1) {}

    /**
     * Return state of the encoded 'pin' number, corresponding to the pull-down
     * states of the actual pins. LOW means that the corresponding encoded
     * virtual pin was pushed.
     */
    int readButton(uint8_t pin) override {
      int sa = analogRead(mPinA);
      uint8_t i = 0;
      while (i < mabSize && ((sa < mab[i].threshold - mab[i].tolerance) || (sa > mab[i].threshold + mab[i].tolerance))) i++;
      if (i == mabSize) return (mPressedState ^ 0x1);

      // Convert the actual pins states into a binary number which becomes
      // the encoded virtual pin numbers of the buttons.
      uint8_t virtualPin = i + 1;
      return (virtualPin == pin) ? mPressedState : (mPressedState ^ 0x1);
    }

  private:
    const uint8_t mPinA;
    const AnalogButtons_t*  mab;       // analog values table
    const uint8_t           mabSize;   // number of elements in analog table
    const uint8_t mPressedState;
};

}

#endif
