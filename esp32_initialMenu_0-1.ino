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

#include <HardwareSerial.h>

#define freqLED 5000 // this is the frequency, in Hz. This is high because I'm driving an LED for testing
#define ledChannel 0 // the channel we want to use 
#define resolution 8 // this is 8-bit resoltution (that is, PWM values 0-255)
#define ledPin 21 // the GPIO pin we want to send the signal through
//char cmd = '';
int freq;

void setup() {
  // put your setup code here, to run once:
  ledcSetup(ledChannel, freqLED, resolution); // set up the ledc parameters
  ledcAttachPin(ledPin, ledChannel); // attack the chosen GPIO to a chosen PWM channel

  Serial.begin(115200); // sets the BAUD rate

  freq = 60; // sets the inital frequency value (in Hz)
             // we need to figure out how to scale frequency based off the clock speed
  
}

void loop(){
  char cmd = '?';
  int wait = 1;

  if(Serial.available()) {
    cmd = Serial.read();  

    switch(cmd) {
      
      case '?':
        Serial.println("-------------------------------");
        Serial.println("?: Help menu");
        Serial.println("L: LED");
        Serial.println("F/f: Increase/Decrease Frequency");
        Serial.println("A: Amplitude");
        Serial.println("P: Pulse Width");
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
        freq++;
        Serial.print("The frequency is now: ");
        Serial.println(freq);
        Serial.println("-------------------------------");
      break;

      case 'f':
        Serial.print("The frequency was: ");
        Serial.println(freq);
        freq--;
        Serial.print("The frequency is now: ");
        Serial.println(freq);
        Serial.println("-------------------------------");
      break;

      case 'A':
        Serial.println("This will change the Amplitude");

      break;

      case 'P':
        Serial.println("This will change the Pulse Width");
      break;
        
    }
   
  }
  
}
