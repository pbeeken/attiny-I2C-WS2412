#include <Arduino.h>
#include <Wire.h>
#include <Streaming.h>
#include <Adafruit_NeoPixel.h>

// I2C address
#define I2C_SLAVE_ADDRESS 0x09 // the 7-bit address (remember to change this when adapting this example)

/**
 * Control Commands
 **/
uint8_t    IDLE          = 0x00;
uint8_t    COLOR_CHANGE  = 0x20; // Change Pixel 3 additional bytes of color (doesn't show the change)
uint8_t    SHOW          = 0x40; // Show pixel changes (this locks the color programming) 
uint8_t    CLEAR         = 0x80; // Wipe all the pixels and make them go dark.
uint8_t    TIME_DURATION = 0x60; // How long for pixel to stay on (ms) UNUSED except a for debugging
uint8_t    UNUSED01      = 0xE0; // 
uint8_t    UNUSED02      = 0xA0; // 
uint8_t    UNUSED03      = 0xC0; // 
uint8_t    CMD_MASK      = 0b11100000;  // mask out the top 3 bits

/**
 * Setup the libraries and signal rediness 
 **/
void setup() {
    Wire.begin();
    Wire.setWireTimeout(1000);
    Wire.setTimeout(1000);
    Serial.begin(115200);
}

void sendColor(int i, uint8_t red, uint8_t green, uint8_t blue) {
    Wire.beginTransmission(I2C_SLAVE_ADDRESS);
    char cmd[4];
    cmd[0] = COLOR_CHANGE | (0x1F & i);  // color change pixel 1
    cmd[1] = red; // RED
    cmd[2] = green; // GREEN
    cmd[3] = blue; // BLUE
    Wire.write(cmd, 4);
    Wire.endTransmission();
}

void showColors() {
    Wire.beginTransmission(I2C_SLAVE_ADDRESS);
    Wire.write(SHOW);
    Wire.endTransmission();
}

void clearColors() {
    Wire.beginTransmission(I2C_SLAVE_ADDRESS);
    Wire.write(CLEAR);
    Wire.endTransmission();
}

//#define DEBUG 1
#ifdef DEBUG
// If the signal debugging is done in peripheral then this is needed to slow the control down
#define WAIT_FOR_IT delay(15)
#else
#define WAIT_FOR_IT ;
#endif

/**
 * Run the process and handle the commands 
 **/
void loop() {
    if (Serial.available()>0) {
        char cc = Serial.read();
        if (cc=='R') {
            char cr[6]; // largely debugging
            // we get 6 bytes of data from the peripheral (the contents of the status buffer).
            Wire.requestFrom(I2C_SLAVE_ADDRESS, 6);
            Wire.readBytes(cr, 6);
            // Report
            Serial << cc << "-Response ["; 
            Serial << _HEX(0x0FF & cr[0]) <<"-"<< _HEX(0x0FF & cr[1]) <<"-"<< _HEX(0x0FF & cr[2]) <<"-"<< _HEX(0x0FF & cr[3]);
            Serial <<"#"<< _HEX(0x0FF & cr[4]) <<" c:"<< _HEX(0x0FF & cr[5]) << "]" << endl;
        } 

        else if (cc=='S') {
            showColors();
            Serial << cc << "-Transmit" << endl;

        } else if (cc=='C') {
            clearColors();
            Serial << cc << "-Transmit" << endl;

        } else if (cc=='1') {
            sendColor(1, 200, 100, 50);
            Serial << cc << "-Transmit" << endl;

        } else if (cc=='2') {
            sendColor(2, 100, 200, 50);
            Serial << cc << "-Transmit" << endl;

        } else if (cc=='3') {
            sendColor(3, random(0,255), random(0,255), random(0,255));
            Serial << cc << "-Transmit" << endl;

        } else if (cc=='4') {
            for(int i=0; i<60; i++) {
                sendColor(5, random(0,255), random(0,255), random(0,255));
                WAIT_FOR_IT; // seems to be needed to allow the system to work.  This isn't the protocol talking
                sendColor(6, random(0,255), random(0,255), random(0,255));
                WAIT_FOR_IT;
                sendColor(7, random(0,255), random(0,255), random(0,255));
                WAIT_FOR_IT;
                showColors();
                delay(50);
            }
            Serial << cc << "-Transmit" << endl;
            clearColors();

        }
    }
    delay(500);
}