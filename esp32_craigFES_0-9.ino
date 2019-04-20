/*
 * Title: CraigFES Stim Main Code            
 * Owner: STIMple Solutions Team
 * Managed by: Jay Drobnick and Annelyse Baker
 * Last updated: 4/14//2019
 */

// ---- LIBRARIES ----
#include <HardwareSerial.h> //include the library necessary to use the '?' menu for development
#include <SPI.h> //include the library to use SPI functions
#include <Adafruit_GFX.h> //include the library to use the gfx driver for the display
#include <Adafruit_ILI9341.h> // include the display library for text, color, and size

// ---- DEFINES: PIN ASSIGNMENTS ----
// This section also serves as a pin connection reference
#define ledPin 21 // the GPIO pin we want to send the signal through (this was for preliminary testing, will be removed) (pin assignment)
#define safe1 36 // safety for channel 1 (pin assignment)
#define safe2 39 // safety for channel 2 (pin assignment)
#define upButton 13 // pin used to increase the value of chosen parameter (pin assignment)
#define downButton 27 // pin used to decrease the value of chosen parameter (pin assignment)
#define modeButton 32 // pin used to select the parameter to be adjusted (pin assignment)
#define startButton 14 // pin used for the start/stop button (pin assignment)
#define pulse1 12 // pulse control for channel 1 (pin assignment)
#define pulse2 4 // pulse control for channel 2 (pin assignment)
#define dac1 25 // amplitude control for channel 1 (pin assignment)
#define dac2 26 // amplitude control for channel 2 (pin assignment)
#define TFT_CS 15 // pin for the screen cs (pin assignment)
#define TFT_DC 33 // pin for the screen dc (pin assignment)

// ---- DEFINES: OTHER ----
#define ledChannel 12 // pwm for the indicator led
#define pwmchan1 0 // choose the "pwm channel" for "stim chan 1"
#define pwmchan2 6 // chose the "pwm channel" for "stim chan 2"
#define resolution 8 // this is 8-bit resoltution (that is, PWM values 0-255)

enum signalStates {
  OFF,
  RISINGRAMP,
  ON,
  FALLINGRAMP
};

// ---- OBJECTS ----
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC); // defines screen object with the given pins

hw_timer_t * timer = NULL;

// ---- GLOBAL VARIABLES ----
volatile int freqLED = 60; // frequency for the led pwm
volatile int freq1 = 60; //variable to set the desired frequency of channel 1 pwm (Hz)
volatile int freq2 = 60; //variable to set the desired frequency of channel 2 pwm (Hz)
volatile int dutyLED = 125; // duty cycle of led pwm
volatile int duty1 = 255; // variable to set the desired duty cycle (pulse width) of the reference PWM (0-255)
volatile int duty2 = 255;
volatile int amp1 = 1; // variable to set the desired amplitude of the channel (takes pwm values 0-255 for 0-3.2 V)
volatile int amp2 = 1;
volatile int rampTime1 = 1; volatile int rampTime2 = 1;
volatile int offTime1 = 1;  volatile int offTime2 = 1; 
volatile int onTime1 = 1;   volatile int onTime2 = 1;
volatile float percievedVoltage; // this is a variable used to display the percieved output voltage from the DAC
volatile bool onOff = 0; // 0 is off, 1 is on
volatile int modeNumber = 12; // variable to determine which "mode" the device is operating in/acting on
volatile int pastMode = 6;
volatile bool modeSwitch = false;
int amp1old = 0; int amp2old = 0; 
int freq1old, freq2old, duty1old, duty2old, ramp1old, ramp2old, on1old, on2old, off1old, off2old; 

void IRAM_ATTR onTimer(){
  static enum signalStates state1 = OFF;
  static int numberOfTime1 = 0;
  static int ampLevel1 = 0;
  static int incrementTime1;
  incrementTime1 = rampTime1*10/amp1;
  static enum signalStates state2 = OFF;
  static int numberOfTime2 = 0;
  static int ampLevel2 = 0;
  static int incrementTime2;
  incrementTime2 = rampTime2*10/amp2;

  if(onOff == true) {
    switch(state1){ // beginning of a switch which evaluates the signalStates variable
    case OFF: // channel 1 off state
      ledcWrite(pwmchan1, 0);
      dacWrite(dac1, 0);
      if( numberOfTime1++ >= (offTime1*1000)) {
        state1 = RISINGRAMP;
        numberOfTime1 = 0;
      }
      break;
    case RISINGRAMP: // channel 1 rising state
      if( numberOfTime1++ >= incrementTime1 && ampLevel1 != amp1) {
        ampLevel1++;
        dacWrite(dac1, ampLevel1);
        ledcWrite(pwmchan1, duty1);
        numberOfTime1 = 0;
      } else if( ampLevel1 == amp1) {
        state1 = ON;
        numberOfTime1 = 0;
      }
      break;
    case ON: // channel 1 on state
      if( numberOfTime1++ >= (onTime1*1000)) {
        state1 = FALLINGRAMP;
        numberOfTime1 = 0;
      }
      break;
    case FALLINGRAMP: // channel 1 rising state
      if( numberOfTime1++ >= incrementTime1 && ampLevel1 != 0) {
        ampLevel1--;
        dacWrite(dac1, ampLevel1);
        numberOfTime1 = 0;
      } else if( ampLevel1 == 0) {
        state1 = OFF;
        numberOfTime1 = 0;
      }
      break;
    }

  switch(state2){ // beginning of a switch which evaluates the signalStates variable
    case OFF: // channel 2 off state
      ledcWrite(pwmchan2, 0);
      dacWrite(dac2, 0);
      if( numberOfTime2++ >= (offTime2*1000)) {
        state2 = RISINGRAMP;
        numberOfTime2 = 0;
      }
      break;
    case RISINGRAMP: // channel 2 rising state
      if( numberOfTime2++ >= incrementTime2 && ampLevel2 != amp2) {
        ampLevel2++;
        dacWrite(dac2, ampLevel2);
        ledcWrite(pwmchan1, duty2);
        numberOfTime2 = 0;
      } else if( ampLevel2 == amp2) {
        state2 = ON;
        numberOfTime2 = 0;
      }
      break;
    case ON: // channel 2 on state
      if( numberOfTime2++ >= (onTime2*1000)) {
        state2 = FALLINGRAMP;
        numberOfTime2 = 0;
      }
      break;
    case FALLINGRAMP: // channel 2 rising state
      if( numberOfTime2++ >= incrementTime2 && ampLevel2 != 0) {
        ampLevel2--;
        dacWrite(dac2, ampLevel2);
        numberOfTime2 = 0;
      } else if( ampLevel2 == 0) {
        state2 = OFF;
        numberOfTime2 = 0;
      }
      break;
    }
  }
}
 

// ---- SETUP FUNCTION ----
void setup() {
  // put your setup code here, to run once:
  ledcSetup(ledChannel, 60, resolution); // set up the ledc parameters (channel, frequency, resolution(in bits) )
  ledcAttachPin(ledPin, ledChannel); // attack the chosen GPIO to a chosen PWM channel
  ledcSetup(pwmchan1, freq1, resolution); // set up the pwm for channel 1  
  ledcAttachPin(pulse1, pwmchan1);          // attach the channel 1 pin to the pwm object for chan 1
  ledcSetup(pwmchan2, freq2, resolution); // set up pwm for channel 2
  ledcAttachPin(pulse2, pwmchan2);          // attack the channel 2 pin to the pwm object for chan 2
  
  pinMode(upButton, INPUT_PULLUP);      // set upButton as input
  pinMode(downButton, INPUT_PULLUP);    // set downButton as input
  pinMode(modeButton, INPUT_PULLUP);    // set modeButton as input
  pinMode(startButton, INPUT_PULLUP);   // set startButton as input
  pinMode(safe1, INPUT);
  pinMode(safe2, INPUT);
  pinMode(pulse1, OUTPUT);
  pinMode(pulse2, OUTPUT);
  pinMode(dac1, OUTPUT);
  pinMode(dac2, OUTPUT);
  
  Serial.begin(115200); // sets the BAUD rate
  /* Use 1st timer of 4 */
  /* 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up */
  timer = timerBegin(0, 80, true);
  /* Attach onTimer function to our timer */
  timerAttachInterrupt(timer, &onTimer, true);
  /* Set alarm to call onTimer function every second 1 tick is 1us
  => 1 millisecond is 1000us */
  /* Repeat the alarm (third parameter) */
  timerAlarmWrite(timer, 1000, true);
  /* Start an alarm */
  timerAlarmEnable(timer);
  Serial.println("start timer");

  tft.begin();  // initializes the screen
  tft.fillScreen(ILI9341_BLACK); // fills the screen background with black (best battery life)
  tft.setRotation(3);            // orients the screen to be right-side up in the housing
  // set up the interrupt for signals
  attachInterrupt(digitalPinToInterrupt(startButton), startStopStim, FALLING); //
  attachInterrupt(digitalPinToInterrupt(modeButton), changeMode, FALLING);

  // INSERT CODE FOR THE WELCOME SCREEN HERE
  // PUT A DELAY AFTER THE WELCOME SCREEN APPEARS SO THAT IT STAYS VISIBLE FOR A PERIOD OF TIME
  testLines(ILI9341_CYAN);
  //tft.setCursor(320/2+20, (240/2));
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_ORANGE);
  tft.setTextSize(3);
  tft.println("CraigFES");
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_CYAN);
  tft.println("STIMple Solutions");
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  Serial.println("displaying welcome screen...");
  
  delay(2000); // show intro screen for 2 second

  onOff = false;
  modeNumber = 12;
  
  Serial.println("displaying parameter screen for first time...");
  printScreen(); //prints the parameter screen upon full boot
} // end of setup function

// ---- MAIN LOOP FUNCTION ----
void loop(){
  
  int wait = 1;
  int i = 0; // iteration variable used for loops

  //jaystestfunction();

  if(modeSwitch == true){
    printScreen();
    modeSwitch = false;
  }

  switch(modeNumber){ // beginning of a switch which evaluates the modeNumber variable
    case 0: // channel 1 amp
      if(digitalRead(upButton) == false && amp1 < 255){ // the buttons are active low, which is why we want to look for a false here
          if(amp1 == 1) {
            amp1+= 4;
          } else {
            amp1 += 5;
          }
          Serial.println(amp1);
      }
      if(digitalRead(downButton) == false && amp1 > 1){
          if(amp1 == 5) {
            amp1-= 4;
          } else {
            amp1 -= 5;
          }
          Serial.println(amp1);
      }
      if (amp1old != amp1){
        printScreen();
        Serial.println("printing because it thinks amp1 has changed");
        Serial.print(amp1);
        Serial.print("    ");
        Serial.println(amp1old);
      }
      amp1old = amp1;
    break;

    case 1: // channel 1 freq
      if(digitalRead(upButton) == false && freq1 < 255){
          freq1++;
          Serial.println(freq1);
      }
      if(digitalRead(downButton) == false && freq1 > 0){
          freq1--;
          Serial.println(freq1);
      }
      if (freq1old != freq1){
        printScreen();
        Serial.println("printing because it thinks freq1 has changed");
        Serial.print(freq1);
        Serial.print("    ");
        Serial.println(freq1old);
      }
      freq1old = freq1;
    break;

    case 2: // channel 1 duty
      if(digitalRead(upButton) == false && duty1 < 255){
          duty1++;
          Serial.println(duty1);
      }
      if(digitalRead(downButton) == false && duty1 > 0){
          duty1--;
          Serial.println(duty1);
      }
      if (duty1old != duty1){
        printScreen();
        Serial.println("printing because it thinks duty1 has changed");
        Serial.print(duty1);
        Serial.print("    ");
        Serial.println(duty1old);
      }
      duty1old = duty1;
    break;

    case 3: // channel 1 ramp Time
      if(digitalRead(upButton) == false && rampTime1 < 10){
          rampTime1++;
          Serial.println(rampTime1);
      }
      if(digitalRead(downButton) == false && rampTime1 > 1){
          rampTime1--;
          Serial.println(rampTime1);
      }
      if (ramp1old != rampTime1){
        printScreen();
        Serial.println("printing because it thinks ramp1 has changed");
        Serial.print(rampTime1);
        Serial.print("    ");
        Serial.println(ramp1old);
      }
      ramp1old = rampTime1;
    break;

    case 4: // channel 1 on Time
      if(digitalRead(upButton) == false && onTime1 < 10){
          onTime1++;
          Serial.println(onTime1);
      }
      if(digitalRead(downButton) == false && onTime1 > 1){
          onTime1--;
          Serial.println(onTime1);
      }
      if (on1old != onTime1){
        printScreen();
        Serial.println("printing because it thinks on1 has changed");
        Serial.print(onTime1);
        Serial.print("    ");
        Serial.println(on1old);
      }
      on1old = onTime1;
    break;
    
    case 5: // channel 1 off Time
      if(digitalRead(upButton) == false && offTime1 < 10){
          offTime1++;
          Serial.println(offTime1);
      }
      if(digitalRead(downButton) == false && offTime1 > 1){
          offTime1--;
          Serial.println(offTime1);
      }
      if (off1old != offTime1){
        printScreen();
        Serial.println("printing because it thinks off1 has changed");
        Serial.print(offTime1);
        Serial.print("    ");
        Serial.println(off1old);
      }
      off1old = offTime1;
    break;

    case 6: // channel 2 amp
      if(digitalRead(upButton) == false && amp2 < 255){
          if(amp2 == 1) {
            amp2 += 4;
          } else {
            amp2 += 5;
          }
          Serial.println(amp2);
      }
      if(digitalRead(downButton) == false && amp2 > 1){
          if(amp2 == 5) {
            amp2 -= 4;
          } else {
            amp2 -= 5;
          }
          Serial.println(amp2);
      }
      if (amp2old != amp2){
        printScreen();
        Serial.println("printing because it thinks amp1 has changed");
        Serial.print(amp2);
        Serial.print("    ");
        Serial.println(amp2old);
      }
      amp2old = amp2;
    break;

    case 7: // channel 2 freq
      if(digitalRead(upButton) == false && freq2 < 255){
          freq2++;
          Serial.println(freq2);

      }
      if(digitalRead(downButton) == false && freq2 > 0){
          freq2--;
          Serial.println(freq2);
      }
      if (freq2old != freq2){
        printScreen();
        Serial.println("printing because it thinks freq2 has changed");
        Serial.print(freq2);
        Serial.print("    ");
        Serial.println(freq2old);
      }
      freq2old = freq2;
    break;

    case 8: // channel 2 duty
      if(digitalRead(upButton) == false && duty2 < 255){
          duty2++;
          Serial.println(duty2);
      }
      if(digitalRead(downButton) == false && duty2 > 0){
          duty2--;
          Serial.println(duty2);
      }
      if (duty2old != duty2){
        printScreen();
        Serial.println("printing because it thinks duty2 has changed");
        Serial.print(duty2);
        Serial.print("    ");
        Serial.println(duty2old);
      }
      duty2old = duty2;
    break;

    case 9: // channel 2 ramp Time
      if(digitalRead(upButton) == false && rampTime2 < 10){
          rampTime2++;
          Serial.println(rampTime2);
      }
      if(digitalRead(downButton) == false && rampTime2 > 1){
          rampTime2--;
          Serial.println(rampTime2);
      }
      if (ramp2old != rampTime2){
        printScreen();
        Serial.println("printing because it thinks ramp2 has changed");
        Serial.print(rampTime2);
        Serial.print("    ");
        Serial.println(ramp2old);
      }
      ramp2old = rampTime2;
    break;

    case 10: // channel 2 on Time
      if(digitalRead(upButton) == false && onTime2 < 10){
          onTime2++;
          Serial.println(onTime2);
      }
      if(digitalRead(downButton) == false && onTime2 > 1){
          onTime2--;
          Serial.println(onTime2);
      }
      if (on2old != onTime2){
        printScreen();
        Serial.println("printing because it thinks on2 has changed");
        Serial.print(onTime2);
        Serial.print("    ");
        Serial.println(on2old);
      }
      on2old = onTime2;
    break;
    
    case 11: // channel 2 off Time
      if(digitalRead(upButton) == false && offTime2 < 10){
          offTime2++;
          Serial.println(offTime2);
      }
      if(digitalRead(downButton) == false && offTime2 > 1){
          offTime2--;
          Serial.println(offTime2);
      }
      if (off2old != offTime2){
        printScreen();
        Serial.println("printing because it thinks off2 has changed");
        Serial.print(offTime2);
        Serial.print("    ");
        Serial.println(off2old);
      }
      off2old = offTime2;
    break;
  } // end of switch
} // end of main loop function

// This will interrupt when the on/off button is pressed
void startStopStim(){
  if(onOff){
    Serial.println("stim offed");
    ledcWrite(pwmchan1, 0);
    ledcWrite(pwmchan2, 0);
    dacWrite(dac1, 0);
    dacWrite(dac2, 0);
    onOff = false;
  }
  else {
    Serial.println("stim onned");
    Serial.println(duty1);
    Serial.println(duty2);
    ledcSetup(pwmchan1, freq1, resolution);
    ledcWrite(pwmchan1, duty1); // writes the pwm channel with the value stored in duty1 (channel 1 stim
    ledcSetup(pwmchan2, freq2, resolution);
    ledcWrite(pwmchan2, duty2);
    onOff = true;
    modeNumber = 12;
  }
  printScreen();
}

// This will interrupt when the mode button is pressed
void changeMode(){
  if(onOff == false) {
    if(modeNumber < 12) modeNumber++;
    else modeNumber = 0;
    modeSwitch = true;
    Serial.print("Mode changed to ");
    Serial.println(modeNumber);
  }
  if(onOff == true){
    modeNumber = 11;
  }
  printScreen();
}

void printScreen(){
  // ---- channel 1 prints ----
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0,0);
  tft.setTextColor(ILI9341_ORANGE);
  tft.setTextSize(2);
  tft.println("Channel 1");
  
  if(modeNumber == 0){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("Power:  ");
  tft.println(amp1);
  
  if(modeNumber == 1){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("Freq: ");
  tft.print(freq1);
  tft.println(" Hz");
  
  if(modeNumber == 2){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("Duty: ");
  tft.print(duty1);
  tft.println(" %");

  if(modeNumber == 3){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("Ramp: ");
  tft.print(rampTime1);
  tft.println(" sec");

  if(modeNumber == 4){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("On: ");
  tft.print(onTime1);
  tft.println(" sec");

    if(modeNumber == 5){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("Off: ");
  tft.print(offTime1);
  tft.println(" sec");

  // --- channel 2 prints ---
  tft.setTextColor(ILI9341_ORANGE);
  tft.println(" ");
  tft.println("Channel 2");
  
  if(modeNumber == 6){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("Power:  ");
  tft.println(amp2);
  
  if(modeNumber == 7){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("Freq: ");
  tft.print(freq2);
  tft.println(" Hz");
  
  if(modeNumber == 8){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("Duty: ");
  tft.print(duty2);
  tft.println(" %");

  if(modeNumber == 9){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("Ramp: ");
  tft.print(rampTime2);
  tft.println(" sec");

  if(modeNumber == 10){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("On: ");
  tft.print(onTime2);
  tft.println(" sec");

  if(modeNumber == 11){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("Off: ");
  tft.print(offTime2);
  tft.print(" sec");

  if(onOff == false) {
    tft.setTextColor(ILI9341_RED);
    tft.println("      Stim: Off");
  }
  if(onOff == true) {
    tft.setTextColor(ILI9341_GREEN);
    tft.println("      Stim: On");
  }
  
}

unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();

  tft.fillScreen(ILI9341_BLACK);
  yield();
  
  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing

  yield();
  tft.fillScreen(ILI9341_BLACK);
  yield();

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  yield();
  tft.fillScreen(ILI9341_BLACK);
  yield();

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  yield();
  tft.fillScreen(ILI9341_BLACK);
  yield();

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);

  yield();
  return micros() - start;
}

void jaystestfunction(){
// ----- DEVELOPMENT MENU (COMMENT OUT WHEN NOT NEEDED) ------
// ----- This menu will not be part of the final code --------
char cmd;
  if(Serial.available()) {
    cmd = Serial.read();

    switch(cmd) {

      case '?':
        Serial.println("-------------------------------");
        Serial.println("?: Help menu");
        Serial.println("s: start button");
        Serial.println("m: mode button");
        Serial.println("u: up button");
        Serial.println("d: down button");
        Serial.println("-------------------------------");
      break;

      case 's': // start/stop button simulation
        startStopStim();
      break;

      case 'm': // mode button simulatoin
        changeMode();
      break;

      case 'u': // up button simulation
        if      (modeNumber == 0) amp1 = amp1 + 10;
        else if (modeNumber == 1) freq1 = freq1 + 10;
        else if (modeNumber == 2) duty1 = duty1 + 10;
        else if (modeNumber == 3) amp2 = amp2 + 10;
        else if (modeNumber == 4) freq2 = freq2 + 10;
        else if (modeNumber == 5) duty2 = duty2 + 10;
      break;

      case 'd': // down button simulation
        if      (modeNumber == 0) amp1--;
        else if (modeNumber == 1) freq1--;
        else if (modeNumber == 2) duty1--;
        else if (modeNumber == 3) amp2--;
        else if (modeNumber == 4) freq2--;
        else if (modeNumber == 5) duty2--;
      break;
      
    }
  }
}
