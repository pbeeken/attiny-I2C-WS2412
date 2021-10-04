# I2C peripheral (slave) to control a small string of WS2412 RGB Pixels
The goal is to create an I2C controller using an attiny85 to control a small number (<32) of
WS2412 pixels from a controller. The notion is that the controller can be one of many
so the number of LEDs can be increased using additional I2C devices but the power and 
management remain modest.

The principal dependancies are Rambo's TinyWire and Adafruit's NeoPixel Library.  After 
much mucking about and playing with timing it seems to work reliably.  Now off to develop
a protocol for setting and manipulating the string of LEDs.
