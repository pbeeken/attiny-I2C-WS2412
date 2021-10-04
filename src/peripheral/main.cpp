#include <Arduino.h>
#include <TinyWireS.h>

#include "LightShow.h"

#define I2C_SLAVE_ADDRESS 0x09 // the 7-bit address (remember to change this when adapting this example)

// The default buffer size, Can't recall the scope of defines right now
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

// Declarations
void requestEvent();
void receiveEvent(uint8_t howMany);

LightShow ring(PB3, 150);

// Tracks the current register pointer position
volatile uint8_t i2c_regs[6];
volatile byte reg_position = 0;
const byte reg_size = sizeof(i2c_regs);

uint8_t    IDLE          = 0x00;
uint8_t    COLOR_CHANGE  = 0x20; // Change Pixel
uint8_t    LOCK_CHANGE   = 0x40; // Show Pixel Changes 
uint8_t    TIME_DURATION = 0x60; // How long for pixel to stay on (ms)
uint8_t    CLEAR         = 0x80; // Wipe all the pixels
uint8_t    UNUSED01      = 0xE0; // 
uint8_t    UNUSED02      = 0xA0; // 
uint8_t    UNUSED03      = 0xC0; // 
uint8_t    CMD_MASK      = 0b11100000;  // mask out the top 3 bits

volatile uint8_t status = IDLE;

const int DEBUG_PIN = PB4;
/** Debugging signal **/
void flashy(int times) {
    for(int i=0; i<times; i++) {
        tws_delay(2);
        digitalWrite(DEBUG_PIN, HIGH);
        tws_delay(2);
        digitalWrite(DEBUG_PIN, LOW);
    }
}

/**
 * Setup the libraries and signal rediness 
 **/
void setup() {
    pinMode(DEBUG_PIN, OUTPUT);
    digitalWrite(DEBUG_PIN, LOW);

    ring.begin();

    /**
     * Reminder: taking care of pull-ups is the masters job
     */
    TinyWireS.begin(I2C_SLAVE_ADDRESS);
    TinyWireS.onReceive(receiveEvent);
    TinyWireS.onRequest(requestEvent);

    flashy(6);

    ring.pixel.setPixelColor(7, ring.pixel.Color(100, 0, 100));
    ring.pixel.show();

}



void loop() {
    // put your main code here, to run repeatedly:
    TinyWireS_stop_check();
    // ring.update();


    if (status == LOCK_CHANGE) {
        flashy(3);
        ring.pixel.show();
        status = IDLE;
    }

    if (status == COLOR_CHANGE) {
        flashy(2);
        int      pixel_index = i2c_regs[0] & 0x1F;
        uint32_t color = ring.pixel.Color(i2c_regs[1], i2c_regs[2], i2c_regs[3]);
        ring.pixel.setPixelColor(pixel_index, color);
        status = IDLE;
    }
    
    if (status == CLEAR) {
        flashy(4);
        ring.pixel.clear();
        ring.pixel.show();
        status = IDLE;
    }

    if (status == TIME_DURATION) {
        flashy(5);
        int      pixel_index = i2c_regs[0] & 0x1F;
        uint32_t color = ring.pixel.Color(i2c_regs[1], i2c_regs[2], i2c_regs[3]);
        ring.pixel.setPixelColor(pixel_index, color);
        ring.pixel.show();
        status = IDLE;
    }

}

/**
 * The I2C transmit ISR
 * 
 * This is called for each read request we receive, never put more than one byte of data (with TinyWireS.send) 
 * to the send-buffer when using this callback. Rambo claims that all I2C return a single byte of information.
 * The way to stay in sync, therefore, is to return the address and a 0 to flag the start of multibyte data
 */
void requestEvent() {
    // Logical way to do it.  We are returning the whole buffer for debugging
    for(int8_t i=0; i < 6; i++) {
        TinyWireS.send(i2c_regs[i]);  // send the recieved byte
    }
}

/**
 * The I2C data received ISR
 *
 * This needs to complete before the next incoming transaction (start, data, restart/stop) on the bus does
 * so be quick, set flags for long running tasks to be called from the mainloop instead of running them directly,
 */
void receiveEvent(uint8_t howMany) {

    if (howMany < 1 || howMany > TWI_RX_BUFFER_SIZE) {
        // Sanity-check
        return;
    }

    // This works but doesnt use the parameter.  # bytes assumed to be fixed.
    // int i = 0;
    // while (0 < TinyWireS.available()) { // loop through bytes (we expect 4)
        // i2c_regs[i++] = TinyWireS.receive();      // receive byte as a character
        // }

    // This uses the provided parameter
    for(int i=0; i<howMany; i++) {
        i2c_regs[i] = TinyWireS.receive();      // receive byte as a character
        }

    i2c_regs[4] = howMany;
    status = CMD_MASK & i2c_regs[0];  // set the current status based on the first byte.
    i2c_regs[5] = status;
}
