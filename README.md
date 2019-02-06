# craigFES
Code files for senior design

See https://docs.google.com/document/d/1KoA2qeiHj3AecPcPePiVI05Ld7mAt6nKuXkNx31tHaM/edit?usp=sharing for information about setting up the Arduino IDE to work with the ESP32 Feather.

Files that start with "sample_" (without quotes) are not intended to be used for the final implementation. They were created during the design process to test for functionality after setting up the Arduino IDE to work with the ESP32 Feather. Hopefully this will save members time when setting up their Feather.

The esp32_initialMenu file has become the primary working file for the project. As of 2/5/2019, it is possible to configure the parameters of the generated square-wave signal using a development ('?') menu. This menu will be replaced by external controls for the end user.

In order to use the '?' menu, load the script in the Arduino IDE and upload it to the ESP32 Feather. Then, open the Serial Monitor and enter a question mark in the text field at the top. After hitting 'Enter', you will be presented with a menu that shows additional controls. 
