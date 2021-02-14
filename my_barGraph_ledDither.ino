//#define LED_DITHER   // if You want to use led_dither library to make display more smooth

#ifdef LED_DITHER
//#define LED_DITHER_DEPTH 4 // bits of dithering . currently not implemented(hardcoded)
#endif LED_DITHER

//#define USE_TONE
#define USE_NEWTONE // you can use one OR the other. NewTone is 1390kb less 

#ifdef USE_TONE
#include <Tone.h>             // used for audible beeps
#endif 

#ifdef USE_NEWTONE
#include <NewTone.h>             // used for audible beeps
#endif 

#include <AsyncDelay.h>       // used to time out PWM outputs 
#include <SmoothThermistor.h> // used for translating thermistors without thermistor table

#ifdef  LED_DITHER
#include <led_dither.h>       
#endif  LED_DITHER

// global constants area

static const uint8_t   ledCount  = 10;     // the number of LEDs in the bar graph

static const uint8_t ledPins[ledCount] = {
  2, 4, 20, 21, 5, 6, 7, 8, 13, 12
//  2, 4, 20, 21, 5, 6, 7, 8, 12, 13 // for tqfp32 version routed in fritzing 
};   // an array of pin numbers to which LEDs are attached
static const uint8_t speaker_pin = 3;                // pin for speaker 
static const uint8_t fan1PWM_pin = 9;                // pin for fan1
static const uint16_t fan1PWM_delay_time = 8*1000 ;  // on time to keep fan running even after temp drops

static const uint8_t fan2PWM_pin = 10;     // pin for fan2
static const uint16_t fan2PWM_delay_time = 10*1000;  // on time to keep fan running even after temp drops

static const uint8_t eyecandy_delay_const = 70;      // eyecandy delay . was done before asyncdelay was introduced. 
                                              // i did let it like that to see how fast main loop is

// constructors : 
// -thermistors 
SmoothThermistor smoothThermistor_0(A0,              // the analog pin to read from
                                  ADC_SIZE_10_BIT, // the ADC size
                                  100000,           // the nominal resistance
                                  10000,           // the series resistance
                                  3950,            // the beta coefficient of the thermistor
                                  25,              // the temperature for nominal resistance
                                  10);             // the number of samples to take for each measurement
SmoothThermistor smoothThermistor_1(A1,              // the analog pin to read from
                                  ADC_SIZE_10_BIT, // the ADC size
                                  100000,           // the nominal resistance
                                  10000,           // the series resistance
                                  3950,            // the beta coefficient of the thermistor
                                  25,              // the temperature for nominal resistance
                                  10);             // the number of samples to take for each measurement
// sound/tone

#ifdef USE_TONE
Tone tone0 ;
#endif 

// Async Delay delays
AsyncDelay fan1PWM_delay;
AsyncDelay fan2PWM_delay; 
//
#ifdef  LED_DITHER
DitherLed led_0(ledPins[0]);
DitherLed led_1(ledPins[1]);
DitherLed led_2(ledPins[2]);
DitherLed led_3(ledPins[3]);
DitherLed led_4(ledPins[4]);
DitherLed led_5(ledPins[5]);
DitherLed led_6(ledPins[6]);
DitherLed led_7(ledPins[7]);
DitherLed led_8(ledPins[8]);
DitherLed led_9(ledPins[9]);

#endif  LED_DITHER

// -----------------------------------------
// global variables :
uint8_t  eyecandy_delay = eyecandy_delay_const; 

// -----------------------------------------
// functions : 

void bargraph_sweep(uint8_t persistence_delay) {
    for (uint8_t thisLed = 0 ; thisLed < ledCount; thisLed++) {
      digitalWrite(ledPins[thisLed],LOW); // ON

      delay(persistence_delay);
      digitalWrite(ledPins[thisLed],HIGH); // OFF      
    }
    for (uint8_t thisLed = ledCount ; thisLed-- > 0;) {
      digitalWrite(ledPins[thisLed],LOW); // ON
      delay(persistence_delay);
      digitalWrite(ledPins[thisLed],HIGH); // OFF     
    }
}

void clear_bargraph() {
    for (uint8_t thisLed = 0 ; thisLed < ledCount; thisLed++) {
      digitalWrite(ledPins[thisLed],HIGH); //
    }
}

void setup() {
  // loop over the pin array and set them all to output:
  for (uint8_t thisLed = 0; thisLed < ledCount; thisLed++) {
    pinMode(ledPins[thisLed], OUTPUT);
  }
#ifdef USE_TONE
  tone0.begin(speaker_pin);
  tone0.play(1000,500); 
#endif USE_TONE
#ifdef USE_NEWTONE
  NewTone(speaker_pin,1000,500); 
#endif USE_NEWTONE
  bargraph_sweep(30);
  pinMode(fan1PWM_pin, OUTPUT);
  pinMode(fan2PWM_pin, OUTPUT);
}



void loop() {

int8_t delta_T = (smoothThermistor_0.temperature()-smoothThermistor_1.temperature()) ;
   // delta of thermometer on top of radiator - thermometer on the bottom of radiator 
   // this gives us rough estimate on cooling capacity left 
   
  // map the result to a range from 0 to the number of LEDs:
  uint8_t ledLevel = map(delta_T, 0, 60, 0, ledCount);
  
  // loop over the LED array:
  for (uint8_t thisLed = 0; thisLed < ledCount; thisLed++) {
    // if the array element's index is less than ledLevel,
    // turn the pin for this element on:
    if (thisLed < ledLevel) {
      digitalWrite(ledPins[thisLed], LOW); // active LOW
    }
    // turn off all pins higher than the ledLevel:
    else {
      digitalWrite(ledPins[thisLed], HIGH); // active LOW, turn off = HIGH or highZ 
    }
  } 
// display is updated, now other bells and whistles time. 
  if (ledLevel < 5 ) { // if cooling capacity is low
    delay(ledLevel*ledLevel*10); // this is to make frequency of beeps increase as cooling capacity decreases . 
    if (smoothThermistor_0.temperature()>30) { // this is to prevent beeping when engine is heating up 
#ifdef USE_TONE
        if (!tone0.isPlaying()) { // not sure if it is needed. //fixme
          tone0.play(1500+(500-(ledLevel*100)),30);// play a tone, the higher frequency the less capacity left. 
          } 
#endif USE_TONE

#ifdef USE_NEWTONE
          NewTone(speaker_pin,1500+(500-(ledLevel*100)),30);// play a tone, the higher frequency the less capacity left. 
#endif USE_NEWTONE
        } //  if (smoothThermistor_0.temperature()>30) {
        
    clear_bargraph(); // clear bargraph to create blinking effect
    delay(ledLevel*5); // the less cooling capacity left, the faster blinking. 
  }

//idle sweep 
  if (ledLevel < 1 ) { // if display is off, like when car is cold, or when there is 0 cooling capacity left    
    if (eyecandy_delay == 0) { // when cycles are gone
      bargraph_sweep(80);    //sweep thru display
      eyecandy_delay = eyecandy_delay_const; // set up the cycle counter back. this is crude cycle counter. 
    }
    eyecandy_delay--; // if not, just count down. 
  }
//

   if (smoothThermistor_0.temperature() > 92) {  // if fluid temperature is above 92C, take extra steps

#ifdef USE_TONE
    if (!tone0.isPlaying()) { 
//      tone0.play(map(smoothThermistor_0.temperature()*10,920,1000,1046,2093),100); 
        tone0.play((smoothThermistor_0.temperature()*smoothThermistor_0.temperature())/8,100); // smaller     
    }
#endif USE_TONE
#ifdef USE_NEWTONE
        NewTone(speaker_pin,(smoothThermistor_0.temperature()*smoothThermistor_0.temperature())/8,100); // smaller     
#endif USE_NEWTONE    
   }

   if (smoothThermistor_0.temperature() > 100) {  // if fluid temperature is above 92C, take extra steps
#ifdef USE_TONE
    if (!tone0.isPlaying()) { 
//      tone0.play(map(smoothThermistor_0.temperature()*10,1000,1100,2093,4186),300); 
        tone0.play((smoothThermistor_0.temperature()*smoothThermistor_0.temperature())/8,300); // smaller
    }
#endif USE_TONE
#ifdef USE_NEWTONE
        NewTone(speaker_pin,(smoothThermistor_0.temperature()*smoothThermistor_0.temperature())/8,300); // smaller
#endif USE_NEWTONE    
   } //if (smoothThermistor_0.temperature() > 100) {

  if (smoothThermistor_0.temperature() >90) {    
uint16_t temperature_temp = map(smoothThermistor_0.temperature()*10,900,920,10,255); //smaller, does not overflow. 
//uint16_t temperature_temp = ((smoothThermistor_0.temperature()-90)*(smoothThermistor_0.temperature())-90)*64;
    analogWrite(fan1PWM_pin,constrain(temperature_temp,0,255)); // turn on fan1, servo, extra water pump etc. 
    fan1PWM_delay.start(fan1PWM_delay_time,AsyncDelay::MILLIS); // reset the async delay timer, so it will persist after temperature drops. 
  } //if (smoothThermistor_0.temperature() >90) { 

  if (smoothThermistor_0.temperature() >92) {
int16_t temperature_temp = map(smoothThermistor_0.temperature()*10,920,950,20,255);
    analogWrite(fan2PWM_pin,constrain(temperature_temp,0,255)); // turn on the fan2, servo or whatever else 
    fan2PWM_delay.start(fan2PWM_delay_time,AsyncDelay::MILLIS); // reset the async delay timer
  } //if (smoothThermistor_0.temperature() >92) {

  if (fan1PWM_delay.isExpired()){ // if async delay timer expried 
    analogWrite(fan1PWM_pin,0);        //disable fan after some time.
  }                                    //putting it here causes off/on glitch
                                      //in case fan should still be on, but i consider it a feature

  if (fan2PWM_delay.isExpired()){ // if async delay timer expired 
    analogWrite(fan2PWM_pin,0);        //disable fan after some time.
  }                                    //putting it here causes off/on glitch
                                      //in case fan should still be on, but i consider it a feature

} //void loop() {
