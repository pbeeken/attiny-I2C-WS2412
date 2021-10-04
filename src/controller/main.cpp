#include <Arduino.h>
#include <Wire.h>
#include <Streaming.h>

#define I2C_SLAVE_ADDRESS 0x09 // the 7-bit address (remember to change this when adapting this example)

uint8_t    IDLE          = 0x00;
uint8_t    COLOR_CHANGE  = 0x20; // Change Pixel
uint8_t    LOCK_CHANGE   = 0x40; // Show Pixel Changes 
uint8_t    TIME_DURATION = 0x60; // How long for pixel to stay on (ms)
uint8_t    CLEAR         = 0x80; // Wipe all the pixels
uint8_t    UNUSED01      = 0xE0; // 
uint8_t    UNUSED02      = 0xA0; // 
uint8_t    UNUSED03      = 0xC0; // 
uint8_t    CMD_MASK      = 0b11100000;  // mask out the top 3 bits


void setup() {
    Wire.begin();
    Wire.setWireTimeout(1000);
    Wire.setTimeout(1000);
    Serial.begin(115200);
}


void loop() {
    if (Serial.available()>0) {
        char cc = Serial.read();
        if (cc=='R') {
            // we get 4 bytes of data and 2 of 00 00 to indicate the end of the stream.
            char cr[6];
            Wire.requestFrom(I2C_SLAVE_ADDRESS, 6);
            Wire.readBytes(cr, 6);
            Serial << cc << "-Response ["; 
            Serial << _HEX(0x0FF & cr[0]) <<"-"<< _HEX(0x0FF & cr[1]) <<"-"<< _HEX(0x0FF & cr[2]) <<"-"<< _HEX(0x0FF & cr[3]);
            Serial <<"#"<< _HEX(0x0FF & cr[4]) <<" c:"<< _HEX(0x0FF & cr[5]) << "]" << endl;
        }
        if (cc=='S') {
            Wire.beginTransmission(I2C_SLAVE_ADDRESS);
            char cmd[4];
            cmd[0] = LOCK_CHANGE;  // Lock the changesE
            Wire.write(cmd, 1);
            Wire.endTransmission();
            Serial << cc << "-Transmit" << endl;
        }        
        if (cc=='C') {
            Wire.beginTransmission(I2C_SLAVE_ADDRESS);
            char cmd[4];
            cmd[0] = CLEAR;  // Lock the changes
            Wire.write(cmd, 1);
            Wire.endTransmission();
            Serial << cc << "-Transmit" << endl;
        }        
        else if (cc=='1') {
            Wire.beginTransmission(I2C_SLAVE_ADDRESS);
            char cmd[4];
            cmd[0] = COLOR_CHANGE | 0x01;  // color change pixel 1
            cmd[1] = 200; // RED
            cmd[2] = 100; // GREEN
            cmd[3] =  50; // BLUE
            Wire.write(cmd, 4);
            Wire.endTransmission();
            Serial << cc << "-Transmit" << endl;
        }
        else if (cc=='2') {
            Wire.beginTransmission(I2C_SLAVE_ADDRESS);
            char cmd[4];
            cmd[0] = COLOR_CHANGE | 0x02;  // color change pixel 1
            cmd[1] = 100; // RED
            cmd[2] = 200; // GREEN
            cmd[3] =  50; // BLUE
            Wire.write(cmd, 4);
            Wire.endTransmission();
            Serial << cc << "-Transmit" << endl;
        }
        else if (cc=='3') {
            Wire.beginTransmission(I2C_SLAVE_ADDRESS);
            char cmd[4];
            cmd[0] = COLOR_CHANGE | 0x03;  // color change pixel 1
            cmd[1] = 100; // RED
            cmd[2] = 100; // GREEN
            cmd[3] = 200; // BLUE
            Wire.write(cmd, 4);
            Wire.endTransmission();
            Serial << cc << "-Transmit" << endl;
        }
        else if (cc=='T') {
            Wire.beginTransmission(I2C_SLAVE_ADDRESS);
            char cmd[4];
            cmd[0] = TIME_DURATION | 0x08;  // color change pixel 8
            cmd[1] = 150; // RED
            cmd[2] = 250; // GREEN
            cmd[3] = 120; // BLUE
            Wire.write(cmd, 4);
            Wire.endTransmission();
            Serial << cc << "-Transmit" << endl;
        }
    }
    delay(500);
}