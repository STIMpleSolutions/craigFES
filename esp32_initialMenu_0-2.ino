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
 * Last updated: 2/5/2019
 */

#include <HardwareSerial.h>

#define freqLED 60 // this is the frequency, in Hz. This is high because I'm driving an LED for testing
#define ledChannel 0 // the channel we want to use 
#define resolution 8 // this is 8-bit resoltution (that is, PWM values 0-255)
#define ledPin 21 // the GPIO pin we want to send the signal through
//char cmd = '';


int freq;
int dutyCycle;
volatile int outputVoltagePWM = 0; // this is the PWM value passed to the DAC which will create a percieved output voltage
volatile float percievedVoltage;

// --- SETUP FUNCTION (Runs only once at boot) ---
void setup() {
  // put your setup code here, to run once:
  ledcSetup(ledChannel, 60, resolution); // set up the ledc parameters
  ledcAttachPin(ledPin, ledChannel); // attack the chosen GPIO to a chosen PWM channel

  //duty cycle parameters
  //ledc_set_duty()
  //ledc_update_duty()
  //---frequency parameters---
  //ledc_set_freq()
  //ledc_get_freq()

  Serial.begin(115200); // sets the BAUD rate

  freq = 60; // sets the inital frequency value (in Hz)
             // we need to figure out how to scale frequency based off the clock speed

  // set up the interrupt for signals
  attachInterrupt(digitalPinToInterrupt(ledPin), channel1_ISR, CHANGE);
}

// --- MAIN LOOP FUNCTION ---
void loop(){
  char cmd = '?';
  int wait = 1;
  int i = 0;

  if(Serial.available()) {
    cmd = Serial.read();  

    switch(cmd) {
      
      case '?':
        Serial.println("-------------------------------");
        Serial.println("?: Help menu");
        Serial.println("L: LED");
        Serial.println("F/f: Increase/Decrease Frequency");
        Serial.println("D/d: Increase/Decreaes Duty Cycle");
        Serial.println("A/a: Increase/Decrease Amplitude");
        Serial.println("t: test function (currenlty dac");
        Serial.println("-------------------------------");
      break;
      
      case 'L':
        // increase the LED brightness
        for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
          // changing the LED brightness with PWM
          ledcWrite(ledChannel, dutyCycle); //note we are specifying the PWM channel, NOT the GPIO pin here
          delay(15);
        }
        // decrease the LED brightness
        for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
          // changing the LED brightness with PWM
          ledcWrite(ledChannel, dutyCycle);   
          delay(15);
        }
       break;

       case 'F':
        Serial.print("The frequency was: ");
        Serial.println(freq);
        freq = freq + 10;
        ledcSetup(ledChannel, freq, resolution);
        Serial.print("The frequency is now: ");
        Serial.println(freq);
        Serial.println("-------------------------------");
      break;

      case 'f':
        Serial.print("The frequency was: ");
        Serial.println(freq);
        freq = freq - 10;
        ledcSetup(ledChannel, freq, resolution);
        Serial.print("The frequency is now: ");
        Serial.println(freq);
        Serial.println("-------------------------------");
      break;
     
      case 'D':
        dutyCycle = dutyCycle + 5;
        ledcWrite(ledChannel, dutyCycle);
        Serial.print("The duty cycle is now: ");
        Serial.println(dutyCycle);
        Serial.println("-------------------------------");
      break;

      case 'd':
        dutyCycle = dutyCycle - 5;
        ledcWrite(ledChannel, dutyCycle);
        Serial.print("The duty cycle is now: ");
        Serial.println(dutyCycle);
        Serial.println("-------------------------------");
      break;

      case 'A':
        outputVoltagePWM = outputVoltagePWM + 5;
        percievedVoltage = outputVoltagePWM * 0.0125;
        Serial.print("The amplitude is now: ");
        Serial.println(outputVoltagePWM);
        Serial.print("This is a voltage of: ");
        Serial.print(percievedVoltage);
        Serial.println(" V");
        Serial.println("-------------------------------");
      break;

      case 'a':
        outputVoltagePWM = outputVoltagePWM - 5;
        percievedVoltage = outputVoltagePWM * 0.0125;
        Serial.print("The amplitude is now: ");
        Serial.println(outputVoltagePWM);
        Serial.print("This is a voltage of: ");
        Serial.print(percievedVoltage);
        Serial.println(" V");
        Serial.println("-------------------------------");
      break;      

      case 't': //tests
        for(int i=0;i<256;i+=32){
          dacWrite(25,i);
          delay(1500);            
        } 
        dacWrite(25,i);
      break;
        
    }
   
  }
  
}

// --- CHANNEL 1 ISR ---
void channel1_ISR(){
  if(digitalRead(ledPin) == HIGH){
    dacWrite(25, outputVoltagePWM);
  }
  else if(digitalRead(ledPin) == LOW){
    dacWrite(25, 0);
  }
}
