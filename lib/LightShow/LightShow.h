#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "Waveforms.h"

const int NUMPIXELS = 9;



// Specific for 24 pixel NeoPixelRing
class LightShow {

    public:
        LightShow( int ctlPIN, int brightness ) {
            LED_PIN = ctlPIN;
            pixel = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
            BRIGHTNESS = brightness;
        }

        void begin() {
            // pixel control
            pixel.begin();
            pixel.setBrightness(BRIGHTNESS);
            pixel.clear();
            pixel.show(); // Initialize all pixels to 'off'
            timerA = Waveforms(4500, 20, 120);
            timerB = Waveforms(5000, 0, 200);
            dispatch = 0; // Do nothing
        }

        void update() {
            if (dispatch & 0x01)
                oceanWaves(); // run ocean waves
            if (dispatch & 0x02)
                warningRed();
        }


        // NONBLOCKING colors trying to emulate refection off ocean surface.
        void oceanWaves(boolean init=false) {
            // static uint32_t duration;
            static uint32_t next;
            static uint32_t led;

            if (init) { // Iniitialize the timers and return.
                //oceanTimer = Waveforms(4500, 20, 120);
                led = 0;
                timerA.setAmplitude(120);
                timerA.setPeriod(5137);  //4500
                timerA.setOffset(20);
                dispatch |= 0x01;
                return;
            }

            if (next>millis()) return;
            next = millis()+1+random(20);

            int v = timerA.sineWave();

            led = (led+1)%pixel.numPixels();
            if (0 == (led%6)) return;  // don't do anything with 4 of the pixels
            pixel.setPixelColor(led, pixel.Color((v-random(10))/4, (v-random(20))/2, (v-random(20))/2));

            led = (led+1)%pixel.numPixels();
            if (0 == (led%6)) return;  // don't do anything with 4 of the pixels
            pixel.setPixelColor(led, pixel.Color(0, 0, random(20)+180-v));

            pixel.show();
        }

        // This is a wierd one. It is started by a 'true' option
        // and runs for one cycle and turns itself off.
        void warningRed(boolean init=false) {
            static uint32_t next;
            static int16_t count = -1;

            if (init) { // Iniitialize the timers and return
                timerB.setPeriod(3000);
                timerB.setOffset(50);
                timerB.setAmplitude(200);
                count = 0;
                dispatch |= 0x02;
                return;
            }

            // if (count==0 && timerB.triangleWave()>5) 
            //     return; // we are fresh don't start yet.

            if (next>millis()) return;
            next = millis()+10;
            //Serial << ".";

            count++;
            int red = timerB.triangleWave();
            pixel.setPixelColor( 0, pixel.Color(red, 0, 0));
            pixel.setPixelColor( 6, pixel.Color(red, 0, 0));
            pixel.setPixelColor(12, pixel.Color(red, 0, 0));
            pixel.setPixelColor(18, pixel.Color(red, 0, 0));

            if (count>550) { // one cycle is complete.
                count = -1;  // flag as done and turn off colors
                pixel.setPixelColor( 0, pixel.Color(0, 0, 0));
                pixel.setPixelColor( 6, pixel.Color(0, 0, 0));
                pixel.setPixelColor(12, pixel.Color(0, 0, 0));
                pixel.setPixelColor(18, pixel.Color(0, 0, 0));
                dispatch ^= 0x02;
            }

        }

        // BLOCKING color algorithm for displaying a rainbow pattern
        void rainbow(uint8_t wait) {
            uint16_t i, j;

            for(j=0; j<256; j++) {
                for(i=0; i<pixel.numPixels(); i++) {
                pixel.setPixelColor(i, Wheel((i+j) & 255));
                }
                pixel.show();
                delay(wait);
            }
        }

        // BLOCKING Fill the dots one after the other with a color
        void colorWipe(uint32_t c, uint8_t wait) {
            for(uint16_t i=0; i < pixel.numPixels(); i++) {
                pixel.setPixelColor(i, c);
                pixel.show();
                delay(wait);
            }
        }

        // BLOCKING Slightly different, this makes the rainbow equally distributed throughout
        void rainbowCycle(uint8_t wait) {
            uint16_t i, j;

            for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
                for(i=0; i< pixel.numPixels(); i++) {
                pixel.setPixelColor(i, Wheel(((i * 256 / pixel.numPixels()) + j) & 255));
                }
                pixel.show();
                delay(wait);
            }
        }

        // BLOCKING Theatre-style crawling lights.
        void theaterChase(uint32_t c, uint8_t wait) {
            for (int j=0; j<10; j++) {  //do 10 cycles of chasing
                for (int q=0; q < 3; q++) {
                for (uint16_t i=0; i < pixel.numPixels(); i=i+3) {
                    pixel.setPixelColor(i+q, c);    //turn every third pixel on
                }
                pixel.show();

                delay(wait);

                for (uint16_t i=0; i < pixel.numPixels(); i=i+3) {
                    pixel.setPixelColor(i+q, 0L);        //turn every third pixel off
                }
                }
            }
        }

        // BLOCKING Theatre-style crawling lights with rainbow effect
        void theaterChaseRainbow(uint8_t wait) {
            for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
                for (int q=0; q < 3; q++) {
                    for (uint16_t i=0; i < pixel.numPixels(); i=i+3) {
                        pixel.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
                    }
                    pixel.show();

                    delay(wait);

                    for (uint16_t i=0; i < pixel.numPixels(); i=i+3) {
                        pixel.setPixelColor(i+q, 0L);        //turn every third pixel off
                    }
                }
            }
        }

        // BLOCKING Input a value 0 to 255 to get a color value.
        // The colours are a transition r - g - b - back to r.
        uint32_t Wheel(byte WheelPos) {
            WheelPos = 255 - WheelPos;

            if(WheelPos < 85) {
                return pixel.Color(255 - WheelPos * 3, 0, WheelPos * 3);
            }

            if(WheelPos < 170) {
                WheelPos -= 85;
                return pixel.Color(0, WheelPos * 3, 255 - WheelPos * 3);
            }
            WheelPos -= 170;
            return pixel.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
        }


        void AllOff() {
            for (uint16_t i=0; i < pixel.numPixels(); i++) {
                pixel.setPixelColor(i, 0L);
            }
        }

    public:
        int LED_PIN;
        int BRIGHTNESS;
        Adafruit_NeoPixel pixel;
        Waveforms timerA;
        Waveforms timerB;
        uint16_t dispatch;

};