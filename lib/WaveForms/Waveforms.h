#include <Arduino.h>

/**
This class provides waveforms for blinking and controlling light intensity.
The class uses the millisecond timer as the base so a period of 1000 is 1 sec.
calling the respective waveform methods returns the value of the waveform at
the current time slot.
**/
#ifndef Waveforms_h

class Waveforms {

    protected:
        // Provides for digital waveforms to be used in developing visual outputs on WS2812
        unsigned long period = 1000;    // ms
        unsigned long dutyCyc = 500;    // 50% of period
        int amp = 255;                  // max val
        int offset = 0;                 // offset
        int lastValue;        // for slope detection

    public:
        Waveforms(unsigned long per = 1000, int ampl = 255, int offs = 0);

        int  getPeriod();
        int  getAmplitude();
        int  getOffset();
        int  getCycles();

        void setPeriod(unsigned long period);
        void setAmplitude(int amp);
        void setOffset(int offset); 
        void setDutyCycle(int dutyCycle); // Only valid for square wave 0-100, call AFTER setting period
        bool isFalling();  // slope of waveform

        // square wave with duty cycle represented as length in period
        // e.g. a call of squareWave(period/2) would be 50% duty cycle, period/4 would be 25%, etc.
        int squareWave(unsigned long time = 0L);

        // sawtooth wave of preset period 
        int sawToothWave(unsigned long time = 0L);

        // sawtooth wave of preset period 
        int triangleWave(unsigned long time = 0L);

        // sinewave with lookup on time and amplitude
        int sineWave(unsigned long time = 0L);


};

#define Waveforms_h
#endif