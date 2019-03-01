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
 * Last updated: 1/31/2019
 */

#include <HardwareSerial.h> //include the library necessary to use the '?' menu for development

#define freqLED 60 // this is the frequency, in Hz. This is high because I'm driving an LED for testing
#define ledChannel 0 // the pwm channel we want to use 
#define resolution 8 // this is 8-bit resoltution (that is, PWM values 0-255)
#define ledPin 21 // the GPIO pin we want to send the signal through
//char cmd = '';


volatile int freq1; //variable to set the desired frequency of reference PWM (in Hz)
volatile int freq2;
volatile int dutyCycle1; // variable to set the desired duty cycle (pulse width) of the reference PWM (0-255)
volatile int dutyCycle2;
volatile int outputVoltagePWM = 0; // this is the PWM value passed to the DAC which will create a percieved output voltage (0-255)
volatile int amp1;
volatile int amp2;
volatile float percievedVoltage; // this is a variable used to display the percieved output voltage from the DAC
volatile bool onOff = 0; // 0 is off, 1 is on
volatile int modeNumber = 0;

const int upButton = 12;
const int downButton = 27;
const int modeButton = 32;
const int startButton = 14;

const int dac1 = 25;
const int dac2 = 26;

// --- SETUP FUNCTION (Runs only once at boot) ---
void setup() {
  // put your setup code here, to run once:
  ledcSetup(ledChannel, 60, resolution); // set up the ledc parameters (channel, frequency, resolution(in bits) )
  ledcAttachPin(ledPin, ledChannel); // attack the chosen GPIO to a chosen PWM channel
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(modeButton, INPUT_PULLUP);
  pinMode(startButton, INPUT_PULLUP);

  //duty cycle parameters
  //ledc_set_duty()
  //ledc_update_duty()
  //---frequency parameters---
  //ledc_set_freq()
  //ledc_get_freq()

  Serial.begin(115200); // sets the BAUD rate

  freq1 = 60; // sets the inital frequency value (in Hz)
             // we need to figure out how to scale frequency based off the clock speed
  freq2 = 60;

  // set up the interrupt for signals
  attachInterrupt(digitalPinToInterrupt(ledPin), channel1_ISR, CHANGE); // creates an interrupt that is triggerd by the ledPin
  attachInterrupt(digitalPinToInterrupt(startButton), startStopStim, FALLING); //
  attachInterrupt(digitalPinToInterrupt(modeButton), changeMode, FALLING); 
}

// --- MAIN LOOP FUNCTION ---
void loop(){
  char cmd = '?'; // variable used to enter the development menu
  int wait = 1;
  int i = 0; // iteration variable used for loops

  switch(modeNumber){
    case 0: // channel 1 amp
      while((digitalRead(upButton) == HIGH) && (digitalRead(downButton) == HIGH));
      if(digitalRead(upButton) == false){
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
  
//  if(Serial.available()) {
//    cmd = Serial.read();  
//    
//    switch(cmd) { // beginning of the '?' menu options
//      
//      case '?': // will print a list of development controls
//        Serial.println("-------------------------------");
//        Serial.println("?: Help menu");
//        Serial.println("L: LED");
//        Serial.println("F/f: Increase/Decrease Frequency");
//        Serial.println("D/d: Increase/Decreaes Duty Cycle");
//        Serial.println("A/a: Increase/Decrease Amplitude");
//        Serial.println("t: test function (currenlty dac");
//        Serial.println("-------------------------------");
//      break;
//      
//      case 'L': // used for tesing basic PWM using an LED hooked up to a pin attached to a PWM channel
//        // increase the LED brightness
//        Serial.println("-------------------------");
//        Serial.println("Performing PWM sweep test");
//        Serial.println("-------------------------");
//        for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){
//          // changing the LED brightness with PWM
//          ledcWrite(ledChannel, dutyCycle); //note we are specifying the PWM channel, NOT the GPIO pin here
//          delay(1);
//        }
//        // decrease the LED brightness
//        for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
//          // changing the LED brightness with PWM
//          ledcWrite(ledChannel, dutyCycle);   
//          delay(1);
//        }
//        Serial.println("Sweep test completed");
//       break;
//
//       case 'F': // Raise the frequency of the reference PWM
//        Serial.print("The frequency was: ");
//        Serial.println(freq);
//        freq = freq + 10;
//        ledcSetup(ledChannel, freq, resolution);
//        Serial.print("The frequency is now: ");
//        Serial.println(freq);
//        Serial.println("-------------------------------");
//      break;
//
//      case 'f': // Lower the frequency of the reference PWM
//        Serial.print("The frequency was: ");
//        Serial.println(freq);
//        freq = freq - 10;
//        ledcSetup(ledChannel, freq, resolution);
//        Serial.print("The frequency is now: ");
//        Serial.println(freq);
//        Serial.println("-------------------------------");
//      break;
//     
//      case 'D': // increase the duty cycle of the reference PWM
//        dutyCycle = dutyCycle + 5;
//        ledcWrite(ledChannel, dutyCycle);
//        Serial.print("The duty cycle is now: ");
//        Serial.println(dutyCycle);
//        Serial.println("-------------------------------");
//      break;
//
//      case 'd': // decrease the duty cycle of the reference PWM
//        dutyCycle = dutyCycle - 5;
//        ledcWrite(ledChannel, dutyCycle);
//        Serial.print("The duty cycle is now: ");
//        Serial.println(dutyCycle);
//        Serial.println("-------------------------------");
//      break;
//
//      case 'A': // increase the amplitude of the DAC output
//        outputVoltagePWM = outputVoltagePWM + 5;
//        percievedVoltage = outputVoltagePWM * 0.0125;
//        dacWrite(25, outputVoltagePWM);
//        Serial.print("The amplitude is now: ");
//        Serial.println(outputVoltagePWM);
//        Serial.print("This is a voltage of: ");
//        Serial.print(percievedVoltage);
//        Serial.println(" V");
//        Serial.println("-------------------------------");
//      break;
//
//      case 'a': // decrease the amplitude of the DAC output
//        outputVoltagePWM = outputVoltagePWM - 5;
//        percievedVoltage = outputVoltagePWM * 0.0125;
//        dacWrite(25, outputVoltagePWM);
//        Serial.print("The amplitude is now: ");
//        Serial.println(outputVoltagePWM);
//        Serial.print("This is a voltage of: ");
//        Serial.print(percievedVoltage);
//        Serial.println(" V");
//        Serial.println("-------------------------------");
//      break;      
//
//      case 't': //tests
//        for(int i=0;i<256;i+=32){
//          dacWrite(25,i);
//          delay(1500);            
//        } 
//        dacWrite(25,i);
//      break;   
//    }  
//  } // end of if serial available
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
    ledcWrite(ledChannel, 0);
    onOff = false;
  }
  else {
    Serial.println("stim onned");
    ledcWrite(ledChannel, 200);
    dacWrite(25, outputVoltagePWM);
    onOff = true;
  }
}

void changeMode(){
  if(modeNumber < 5){
    modeNumber++;
  }
  else{
    modeNumber = 0;
  }
  Serial.print("Mode changed to ");
  Serial.println(modeNumber);
}
