/*
 * Title: sample_ledcPWM
 * Purpose: This script was created to check basic functionality of the ESP32 using the Arduino IDE
 *          Specifically, this script tests the function of the ledc PWM functionality of the ESP32
 *          
 * Background: The ESP32 has 16 LEDC PWM channels which are capable of outputting independent signals. 
 *             This seems like it could be useful for our multi-channel device, as long as PWM is a valid solution for implementation
 *             
 * Owner: STIMple Solutions Team
 * Managed by: Jay Drobnick
 * Last updated: 1/30/2019
 */

#define freq 5000 // this is the frequency, in Hz. This is high because I'm driving an LED for testing
#define ledChannel 0 // the channel we want to use 
#define resolution 8 // this is 8-bit resoltution (that is, PWM values 0-255)
#define ledPin 21 // the GPIO pin we want to send the signal through

void setup() {
  // put your setup code here, to run once:
  ledcSetup(ledChannel, freq, resolution); // set up the ledc parameters
  ledcAttachPin(ledPin, ledChannel); // attack the chosen GPIO to a chosen PWM channel
}

void loop(){
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
}
