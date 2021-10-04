#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "Waveforms.h"

/***
This class provides waveforms for blinking and controlling light intensity.
The class uses the millisecond timer as the base so a period of 1000 is 1 sec.
calling the respective waveform methods returns the value of the waveform at
the current time slot.
*/

Waveforms::Waveforms(unsigned long per, int ampl, int offs) {
    //"""declare the period in ms, amp as a digital value, and offset"""
    period = per;
    amp = ampl;
    offset = offs;
    dutyCyc = per/2; // default 50%
    }

int
Waveforms::getPeriod() {
    //"""retrieve the period"""
    return period;
    }

int
Waveforms::getAmplitude() {
    //"""retrieve the amplitude"""
    return amp;
    }

int
Waveforms::getOffset() {
    //"""retrieve the offset"""
    return offset;
    }

int
Waveforms::getCycles() {
    // retrieve the approximate number of cycles
    return millis() % period;
}


void
Waveforms::setPeriod(unsigned long per) {
    //"""set the period"""
    period = per;
    }

void
Waveforms::setAmplitude(int ampl) {
    //"""set the amplitude"""
    amp = ampl;
    }

void
Waveforms::setOffset(int offs) {
    // """set the offset"""
    offset = offs;
    }



void
Waveforms::setDutyCycle(int duty) {
    // """set the duty cycle"""
    dutyCyc = period * 100 / duty;
    }

/** 
 * square wave with duty cycle represented as length in period
 * e.g. a call of squareWave(period/2) would be 50% duty cycle, period/4 would be 25%, etc.
 **/
int
Waveforms::squareWave(unsigned long time) {
    // """get the value of a square wave @ time with duty cycle expressed as portion of period"""
    if (time == 0) time = millis();
    if ((time % period) < dutyCyc)
        return amp + offset;
    else
        return offset;
    }

/**
 * sawtooth wave of preset period
 **/
int
Waveforms::sawToothWave(unsigned long time) {
    // """get the value of a saw tooth wave @ time ms"""
    if (time == 0) time = millis();
    unsigned long now = time % period;
    return map(now, 0, period, offset, offset + amp); // probably should be: map(now, 0, period, offset, offset+amp)
    }

/**
 * triangle wave of preset period
 **/
int
Waveforms::triangleWave(unsigned long time) {
    // """get the value of a triangle wave @ time ms"""
    if (time == 0) time = millis();
    unsigned long now = time % period;
    if (now < period / 2)
      return map(now, 0, period/2, offset, offset + amp); // probably should be: map(now, 0, period, offset, offset+amp)
    else
      return map(now, period/2, period, amp + offset, offset); // probably should be: map(now, 0, period, amp+offset, offset)
    }


/**
 * triangle wave of preset period
 **/
int
Waveforms::sineWave(unsigned long time) {
    // """get the value of a triangle wave @ time ms"""
    if (time == 0) time = millis();
    unsigned long now = time % period;
    unsigned char tau = map(now, 0, period, 0, 255);

    int ampl = Adafruit_NeoPixel::sine8(tau); // so we don't duplicate the table (may have to revisit)
    return map(ampl, 0, 255, offset, offset + amp); // probably should be: map(now, 0, 255, offset, offset+amp)
    }
