This repository contains Firmware for the dipcoater used in the context of the WOM project.
A WOM is the Wavelegth-shifting Optical Module developed for IceCube-Gen2.
The firmware is specifically designed for the large dipcoater used to apply WLS paint.
The device is currently based at DESY (Zeuthen) and belongs to the Humboldt University.
The Hardware used is an Arduino Uno with a Motor Shield, connected to a stepper motor.
The stepper drives a thread rod and allows movement of a carriage along the rod, within a range of a little more than a meter.
For exact specifications please refer to the actual device.

The firmware currently in use "Curent_Microstepping" is written by Dustin Hebecker.
It is partially based on "old_BaseVersion" written by Dr. Akos Hoffmann and based on the Motor Shield Stepper Demo by Randy Sarafan.

It is recommended to use the "Curent_Microstepping" version, since it supports speed control and limited range movements.
Furthermore it uses micro stepping and thus avoids jumps that might occur when using the old version.
Yet the current version calculates the values for micro stepping on the fly. Any future user might want to create a look-up table for that.
This will allow for higher coating speeds in case they are needed.
The type of "steps" should be changed from unsigned int to unsigned long to avoid overflows.
The delay should call delayMicroseconds(us) not delay(ms) to allow for more accurate velocity control.


Instructions:
After loading the firmware the following options are available:
Push the Up button. -> the carriage will move up
Push the DOwn button -> the carriage will move down 
Both up and down buttons are dead man switches, when first initialized and stop operating when released.
This changes when the number of steps are initialized to values > 1. In this case the given number of steps will be made.
After initialisation the Stop button can be used to halt the carriage during operation.
After initialisation the Run button can be used to move the carriage up with a well defined speed.

For initialisation and normal operation the serial interface via USB is used:
Send any value (e.g. "h") except the ones defined below to receive a short manual.
Send "u" to move the carriage up. Operates like the button Up.
Send "d" to move the carriage down. Operates like the button Down.
Send "r" to move the carriage slowly up with a well defined speed using micro stepping. Operates like the button Run.
Send "s" to overwrite and previous movement commands and halt the device. Operates like the button Stop.
Send "S####" to initialise the number of steps to ####. The recommended value is 30000 to cover just about 3/4 of the devices height.
Send "D#" to initialise the delay between micro steps to # in milliseconds. Useful values are between 2 and 5.
Send "t" to return the current settings.

Safety:
Do not leave the step number initialized at values > 1 when the device is unattended.
Make sure the end switches are installed properly and are operational. 

