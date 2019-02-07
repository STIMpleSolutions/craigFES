/*
 * Title: sample_blinLED
 * Purpose: This script was created to check basic functionality of the ESP32 using the Arduino IDE
 * Owner: STIMple Solutions Team
 * Managed by: Jay Drobnick
 * Last updated: 1/29/2019
 */

void setup() {
  // put your setup code here, to run once:
  pinMode(21, OUTPUT); //sets GPIO pin #21 to be an output
  digitalWrite(21, LOW); //sets the inital state of the output to LOW (0 V)
}

void loop() {
  // The code in this loop turns an LED off and on with a frequency of 1 Hz. The timing solution used here is not idea for future implementation,
  // but is useful for demonstration and proof of communication. 
  digitalWrite(21, HIGH); // Turn on the LED
  delay(1000); // Wait 1 second
  digitalWrite(21, LOW); // Turn off the LED
  delay(1000); // Wait 1 second
}
