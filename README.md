# engine-cooler-capacity-bargraph
Simple atmega8 LED bargraph, to display remaining cooling capacity of car engine cooler

Coding excercise. 

It is designed to display cooling capacity (difference inbetween input and output temperature) of car engine cooler. 

Features :
* custom size and pin mapping of bargraph - standard arduino sketch :)
* If there is nothing on the bargraph for whatever reason, sweep goes thru , so You know it's powered on and all is OK.
* There is threshold temperature below which warnings are not triggered - to prevent annoyance when engine is warming up.
* Tone and NewTone library used for audio warnings
* When temperature difference is too low, warnings are displayed and beeped.
 The less cooling capacity left, the faster display blinks and beeps get higher pitched. 
* Above 92C , first extra PWM output is enabled , can be used for extra cooler fluid pump, a fan or for grille inlet servo. 
 Also extra beeps are beeped.
* Above 95C second extra PWM output is enabled. Can be used for extra cooler fluid pump or a fan.
 Extra beeps beeped again
* after temperature goes below 90C , PWM outputs remain active for a while so engine can cool down to safe levels. 
 TODO : if multiple overheats are detected, they could remain on for longer time for greater engine safety. 
 TODO : implement dither led somehow - with new int {} somehow perhaps? 
  I cannot figure out how to make it initalize all constructors based on list of LED pins . 
  Rationale of using DitherLed is that half bright leds provide more display range. 

That's it. Very simple code. 
Hardware:
* two NTC thermistors wired + to thermistor, ADC pin and then series resistor to ground. 
* beeper - piezo, small speaker
* things for PWM output - servos, mosfets for fans or pumps . 
 Wiring LED's to PWM outputs will also provide visual indication of overheating (92C then 95C). 
* LED bargraph or any number of LED's. More than four is good idea. I used 10 . 
* atmega8 or any other low cost uC . 
Sketch uses 5102 bytes (66%) of program storage space. Maximum is 7680 bytes.
Global variables use 78 bytes (7%) of dynamic memory, leaving 946 bytes for local variables. Maximum is 1024 bytes.
I guess one can optimize it even more , for me it was good enough. 

* voltage regulator can be very crude/cheap as ADC Vref is Vcc . Device works fine from 3.3V to 5V.
* good , long wires for thermistors. 

After thinking it all over it's probably much cheaper/better to use two ESP8266 - one put on the radiator, 
another inside the car, and read the temperature wireless - most labour intensive process of laying the cables. 
There are ESP32 with OLED display built in for pennies. 
This means much less soldering/assembly required, prettier display and so on, probably would fit into one of dead buttons 
on dashboard. 

OTOH if You have spare atmega8/arduino , breadboard, evening and a bit of old telephone wire ...

Remember to burn the fuses/bootloader first - I spent lot of time figuring out why my code does not work on new atmega :D
I used miniCore to get atmega8 into arduino IDE chip list. 
