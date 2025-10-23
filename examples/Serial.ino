// Include the Flthy API Library
#include <flthyhpapi.h>

// Initialise the FlthyHP API
FlthyHP FlthyHP(&Serial,9600); // Serial (Stream Port, baud rate)
//FlthyHP FlthyHP(16,17,9600); // Serial (RX Pin, TX Pin, baud rate)

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Begin Setup
  FlthyHP.begin(); // Refresh Speed (Default:125 ms)
}

unsigned long Wait = millis();
void loop() {
  // put your main code here, to run repeatedly:

  // If 30 Seconds have passed
  if (millis() > Wait) {
    // Set HPs to Rainbow for 15 Seconds
    FlthyHP.Rainbow("A", 15);


    // Reset Timer    
    Wait = millis() + 30000;
  }
}