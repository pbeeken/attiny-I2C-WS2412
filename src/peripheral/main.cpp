#include <Arduino.h>
#include <TinyWireS.h>
#include <Adafruit_NeoPixel.h>

/**
 * Setups for I2C 
 **/
#define I2C_SLAVE_ADDRESS 0x09 // the 7-bit address (remember to change this when adapting this example)
// Set the default buffer size, from usiTwiSlave.h
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

// Declarations for ISRs
void requestEvent();
void receiveEvent(uint8_t howMany);

// Global variables across ISRs
volatile uint8_t i2c_regs[6];
volatile byte reg_position = 0;
const byte reg_size = sizeof(i2c_regs);

/**
 * Setups for NeoPixel
 **/
const int LED_PIN = PB3;
const int NUMPIXELS = 9;
const int BRIGHTNESS = 150;
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

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

volatile uint8_t status = IDLE;


/** Debugging signal for oscilloscope set to 5 ms per box **/
//#define DEBUG 1
#ifdef DEBUG
const int DEBUG_PIN = PB4;
void FLASHY(int times) {
    for(int i=0; i<times; i++) {
        tws_delay(1);
        digitalWrite(DEBUG_PIN, HIGH);
        tws_delay(1);
        digitalWrite(DEBUG_PIN, LOW);
    }
}
#else
#define FLASHY(a)  ;
#endif

/**
 * Setup the libraries and signal rediness 
 **/
void setup() {
#ifdef DEBUG
    pinMode(DEBUG_PIN, OUTPUT);
    digitalWrite(DEBUG_PIN, LOW);
#endif

    // initialize the neopixel
    pixel.begin();
    pixel.setBrightness(BRIGHTNESS);
    pixel.clear();
    pixel.show(); // Initialize all pixels to 'off'

    /**
     * Reminder: taking care of pull-ups is the masters job
     */
    // initialize the tinywire library
    TinyWireS.begin(I2C_SLAVE_ADDRESS);
    TinyWireS.onReceive(receiveEvent);
    TinyWireS.onRequest(requestEvent);

    // debugging marker
    FLASHY(6);
}


/**
 * Run the process and handle the commands 
 **/
void loop() {
    // This needs to be at the top and run for each loop:
    TinyWireS_stop_check();

    if (status == SHOW) {
        // 2.5 ms to run
        FLASHY(1);
        pixel.show();
        status = IDLE;
        FLASHY(2);
    }
    else if (status == COLOR_CHANGE) {
        // 2.5 ms to run
        FLASHY(1);
        int      pixel_index = i2c_regs[0] & 0x1F;
        uint32_t color = pixel.Color(i2c_regs[1], i2c_regs[2], i2c_regs[3]);
        pixel.setPixelColor(pixel_index, color);
        status = IDLE;
        FLASHY(3);
    }
    else if (status == CLEAR) {
        FLASHY(1);
        // 2.5 ms to run
        pixel.clear();
        pixel.show();
        status = IDLE;
        FLASHY(4);
    }
    else if (status == TIME_DURATION) {
        FLASHY(1);
        // 2.5 ms to run
        int      pixel_index = i2c_regs[0] & 0x1F;
        uint32_t color = pixel.Color(i2c_regs[1], i2c_regs[2], i2c_regs[3]);
        pixel.setPixelColor(pixel_index, color);
        pixel.show();
        status = IDLE;
        FLASHY(5);
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
    // TODO: This is largely done for debugging.  We can return some useful information
    // Logical way to do it.  We are returning the whole buffer for debugging
    // send the whole command buffer so we can debug the command
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

    // This works but doesnt use the howMany parameter.  # bytes assumed to be fixed.
    // int i = 0;
    // while (0 < TinyWireS.available()) { // loop through bytes (we expect 4)
        // i2c_regs[i++] = TinyWireS.receive();      // receive byte as a character
        // }

    // This uses the provided parameter
    for(int i=0; i<howMany; i++) {
        i2c_regs[i] = TinyWireS.receive();      // receive byte as a character
        }

    status = CMD_MASK & i2c_regs[0];  // set the current status based on the first byte.
    // For debugging purposes put the values into the status register.
    i2c_regs[4] = howMany;
    i2c_regs[5] = status;
}
