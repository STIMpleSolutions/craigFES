/*
 * Title: sample_ledcPWM
 * Purpose: This script was created to check basic functionality of the ESP32 using the Arduino IDE
 *          Specifically, this script tests the function of the ledc PWM functionality of the ESP32
 *          
 * Background: The ESP32 has 16 LEDC PWM channels which are capable of outputting independent signals. 
 *             This seems like it could be useful for our multi-channel device, as long as PWM is a valid solution for implementation
 *             
 * Owner: STIMple Solutions Team
 * Managed by: Jay Drobnick and Annelyse Baker
 * Last updated: 3/11//2019
 */

// ---- TO DO -----
// ---- Delete entries when completed ----
// Create ISR for the start/stop button
// Add functionality to start/stop the stim on both channels independently as well as simultaneously


// ---- LIBRARIES ----
#include <HardwareSerial.h> //include the library necessary to use the '?' menu for development
#include <SPI.h> //include the library to use SPI functions
#include <Adafruit_GFX.h> //include the library to use the gfx driver for the display
#include <Adafruit_ILI9341.h> // include the display library for text, color, and size

// ---- DEFINES ----
// This section also serves as a pin connection reference
#define ledChannel 2 // pwm for the indicator led
#define resolution 8 // this is 8-bit resoltution (that is, PWM values 0-255)
#define ledPin 21 // the GPIO pin we want to send the signal through (this was for preliminary testing, will be removed)
#define safe1 36 // safety for channel 1
#define safe2 4 // safety for channel 2
#define upButton 12 // pin used to increase the value of chosen parameter
#define downButton 27 // pin used to decrease the value of chosen parameter
#define modeButton 32 // pin used to select the parameter to be adjusted
#define startButton 14 // pin used for the start/stop button
#define pwmchan1 0 // choose the "pwm channel" for "stim chan 1"
#define pwmchan2 1 // chose the "pwm channel" for "stim chan 2"
#define pwm1 34 // pulse control for channel 1
#define pwm2 39 // pulse control for channel 2
#define dac1 25 // amplitude control for channel 1
#define dac2 26 // amplitude control for channel 2
#define TFT_CS 15 // pin for the screen cs
#define TFT_DC 33 // pin for the screen dc

// ---- OBJECTS ----
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC); // defines screen object with the given pins

// ---- GLOBAL VARIABLES ----
volatile int freqLED = 60; // frequency for the led pwm
volatile int freq1 = 0; //variable to set the desired frequency of channel 1 pwm (Hz)
volatile int freq2 = 0; //variable to set the desired frequency of channel 2 pwm (Hz)
volatile int dutyLED = 125; // duty cycle of led pwm
volatile int dutyCycle1 = 0; // variable to set the desired duty cycle (pulse width) of the reference PWM (0-255)
volatile int dutyCycle2 = 0;
volatile int amp1 = 0; // variable to set the desired amplitude of the channel (takes pwm values 0-255 for 0-3.2 V)
volatile int amp2 = 0;
volatile float percievedVoltage; // this is a variable used to display the percieved output voltage from the DAC
volatile bool onOff = 0; // 0 is off, 1 is on
volatile int modeNumber = 6; // variable to determine which "mode" the device is operating in/acting on
volatile int pastMode = 6;
/* MODE NUMBERS REFERENCE COMMENT
 *  0 - channel 1 amplitude
 *  1 - channel 1 frequency
 *  2 - channel 1 duty cucyle
 *  3 - channel 2 amplitude
 *  4 - channel 2 frequency
 *  5 - channel 2 duty cycle
 *  6 - no selection
 */

// ---- SETUP FUNCTION ----
void setup() {
  // put your setup code here, to run once:
  ledcSetup(ledChannel, 60, resolution); // set up the ledc parameters (channel, frequency, resolution(in bits) )
  ledcAttachPin(ledPin, ledChannel); // attack the chosen GPIO to a chosen PWM channel
  ledcSetup(pwmchan1, freq1, resolution); // set up the pwm for channel 1
  ledcAttachPin(pwm1, pwmchan1);          // attach the channel 1 pin to the pwm object for chan 1
  ledcSetup(pwmchan2, freq2, resolution); // set up pwm for channel 2
  ledcAttachPin(pwm2, pwmchan2);          // attack the channel 2 pin to the pwm object for chan 2
  pinMode(upButton, INPUT_PULLUP);      // set upButton as input
  pinMode(downButton, INPUT_PULLUP);    // set downButton as input
  pinMode(modeButton, INPUT_PULLUP);    // set modeButton as input
  pinMode(startButton, INPUT_PULLUP);   // set startButton as input
  Serial.begin(115200); // sets the BAUD rate
  tft.begin();  // initializes the screen
  tft.fillScreen(ILI9341_BLACK); // fills the screen background with black (best battery life)
  tft.setRotation(3);            // orients the screen to be right-side up in the housing
  // set up the interrupt for signals
  attachInterrupt(digitalPinToInterrupt(ledPin), channel1_ISR, CHANGE); // creates an interrupt that is triggerd by the ledPin
  attachInterrupt(digitalPinToInterrupt(startButton), startStopStim, FALLING); //
  attachInterrupt(digitalPinToInterrupt(modeButton), changeMode, FALLING);

  // INSERT CODE FOR THE WELCOME SCREEN HERE
  // PUT A DELAY AFTER THE WELCOME SCREEN APPEARS SO THAT IT STAYS VISIBLE FOR A PERIOD OF TIME
  testLines(ILI9341_CYAN);
  tft.setCursor(320/2+20, (240/2));
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_ORANGE);
  tft.setTextSize(3);

  tft.print("Welcome");
  Serial.println("displaying welcome screen...");
  
  delay(6000); // show intro screen for 1 second
  
  Serial.println("displaying parameter screen for first time...");
  printScreen(); //prints the parameter screen upon full boot
} // end of setup function

// ---- MAIN LOOP FUNCTION ----
void loop(){
//  char cmd = '?'; // variable used to enter the development menu (obsolete)
  int wait = 1;
  int i = 0; // iteration variable used for loops

  if(pastMode != modeNumber){
    printScreen();
    pastMode = modeNumber;
  }
  
  switch(modeNumber){ // beginning of a switch which evaluates the modeNumber variable
    case 0: // channel 1 amp
      while((digitalRead(upButton) == HIGH) && (digitalRead(downButton) == HIGH));
      if(digitalRead(upButton) == false){ // the buttons are active low, which is why we want to look for a false here
        if(amp1 < 255){
          amp1++;
          Serial.println(amp1);
        }
      }
      if(digitalRead(downButton) == false){
        if(amp1 > 0){
          amp1--;
          Serial.println(amp1);
        }
      }
      printScreen();
    break;

    case 1: // channel 1 freq
      while((digitalRead(upButton) == HIGH) && (digitalRead(downButton) == HIGH));
      if(digitalRead(upButton) == false){
        if(freq1 < 255){
          freq1++;
          Serial.println(freq1);
        }
      }
      if(digitalRead(downButton) == false){
        if(freq1 > 0){
          freq1--;
          Serial.println(freq1);
        }
      }
    break;

    case 2: // channel 1 duty
      while((digitalRead(upButton) == HIGH) && (digitalRead(downButton) == HIGH));
      if(digitalRead(upButton) == false){
        if(dutyCycle1 < 255){
          dutyCycle1++;
          Serial.println(dutyCycle1);
        }
      }
      if(digitalRead(downButton) == false){
        if(dutyCycle1 > 0){
          dutyCycle1--;
          Serial.println(dutyCycle1);
        }
      }
    break;

    case 3: // channel 2 amp
      while((digitalRead(upButton) == HIGH) && (digitalRead(downButton) == HIGH));
      if(digitalRead(upButton) == false){
        if(amp2 < 255){
          amp2++;
          Serial.println(amp2);
        }
      }
      if(digitalRead(downButton) == false){
        if(amp2 > 0){
          amp2--;
          Serial.println(amp2);
        }
      }
    break;

    case 4: // channel 2 freq
      while((digitalRead(upButton) == HIGH) && (digitalRead(downButton) == HIGH));
      if(digitalRead(upButton) == false){
        if(freq2 < 255){
          freq2++;
          Serial.println(freq2);
        }
      }
      if(digitalRead(downButton) == false){
        if(freq2 > 0){
          freq2--;
          Serial.println(freq2);
        }
      }
    break;

    case 5: // channel 2 duty
      while((digitalRead(upButton) == HIGH) && (digitalRead(downButton) == HIGH));
      if(digitalRead(upButton) == false){
        if(dutyCycle2 < 255){
          dutyCycle2++;
          Serial.println(dutyCycle2);
        }
      }
      if(digitalRead(downButton) == false){
        if(dutyCycle2 > 0){
          dutyCycle2--;
          Serial.println(dutyCycle2);
        }
      }
    break;
  }
//  printScreen();
} // end of main loop function

// --- CHANNEL 1 ISR ---
// this ISR is used to "pass" frequency and duty cycle parameters on to the DAC output
void channel1_ISR(){
//  if(digitalRead(ledPin) == HIGH){
//    dacWrite(25, outputVoltagePWM);
//  }
//  else if(digitalRead(ledPin) == LOW){
//    dacWrite(25, 0);
//  }
} // end of channel 1 ISR

void startStopStim(){
  if(onOff){
    Serial.println("stim offed");
    ledcWrite(pwmchan1, 0);
    ledcWrite(pwmchan2, 0);
    onOff = false;
  }
  else {
    Serial.println("stim onned");
    ledcWrite(pwmchan1, pwm1); // writes the pwm channel with the value stored in pwm1 (channel 1 stim)
    dacWrite(dac1, amp1); // sends the amp1 value to dac1 (channel 1 stim)
    
    ledcWrite(pwmchan2, pwm2); // writes the pwm channel with the value stored in pwm2 (channel 2 stim)
    dacWrite(dac2, amp2); // sends the amp2 value to dac2 (channel 2 stim)
    onOff = true;
  }
}

void changeMode(){
  if(modeNumber < 6){
    modeNumber++;
  }
  else{
    modeNumber = 0;
  }
  Serial.print("Mode changed to ");
  Serial.println(modeNumber);
}

void printScreen(){
  // ---- channel 1 prints ----
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0,0);
  tft.setTextColor(ILI9341_ORANGE);
  tft.setTextSize(3);
  tft.println("Channel 1");
  
  if(modeNumber == 0){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("Amp:  ");
  tft.print(amp1);
  tft.println(" PWM");
  
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
  tft.print(dutyCycle1);
  tft.println(" %");

  // --- channel 2 prints ---
  tft.setTextColor(ILI9341_ORANGE);
  tft.println(" ");
  tft.println("Channel 2");
  
  if(modeNumber == 3){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("Amp:  ");
  tft.print(amp2);
  tft.println(" PWM");
  
  if(modeNumber == 4){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("Freq: ");
  tft.print(freq2);
  tft.println(" Hz");
  
  if(modeNumber == 5){
    tft.setTextColor(ILI9341_YELLOW);
  } else tft.setTextColor(ILI9341_WHITE);
  tft.print("Duty: ");
  tft.print(dutyCycle2);
  tft.println(" %");
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
