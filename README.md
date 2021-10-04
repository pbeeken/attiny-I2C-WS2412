# attiny-I2C-WS2412

## I2C peripheral (slave) to control a small string of WS2412 RGB Pixels

The goal is to create an I2C controller using an attiny85 to control a small number (<32) of
WS2412 pixels from a controller. The notion is that the controller can be one of many
so the number of LEDs can be increased using additional I2C devices but the power and 
management remain modest.

The principal dependancies are Rambo's TinyWire and Adafruit's NeoPixel Library.  After 
much mucking about and playing with timing it seems to work reliably.  Now off to develop
a protocol for setting and manipulating the string of LEDs.

It went much smoother that I thought, I eliminated the lightShow library which I brought over
from a different project and streamlined the NeoPixel calls. The NP library has some critical timing
routines which might have played hob with the ISRs that manage the I2C communication but I have found
very little interference.  The typical call to deliver a command to the NeoPixels is about 2.4ms all in.
Trying to deliver commands faster than that can run into some issues but nothing serious.

My original plan was to pass in information to do some basic pixel manipulation so the controller 
didn't have to manage the light show of the pixels but the overhead isn't that bad.

Why not connect the NeoPixels to the controller?  The ultimate plan is to bury the electronics
in my Robosapien (yeah, one of the originals) and only have 4 wires come out to the controller which might be an Arduino or 
a Raspberry Pi.  One I2C controller for the robot movement and another for the extra lights I plan to install and...? who knows.

Two wires (and power) to rule them all!

