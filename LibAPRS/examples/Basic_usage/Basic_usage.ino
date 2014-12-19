// You must define what reference voltage the ADC
// of your device is running at. If you bought a
// MicroModem from unsigned.io, it will be running
// at 3.3v if the "hw rev" is greater than 2.0.
// This is the most common. If you build your own
// modem, you should know this value yourself :)
#define ADC_REFERENCE REF_3V3
// OR
//#define ADC_REFERENCE REF_5V

// You can also define whether your modem will be
// running with an open squelch radio:
#define OPEN_SQUELCH false

// Include LibAPRS
#include <LibAPRS.h>

// You always need to include this function. It will
// get called by the library every time a packet is
// received, so you can process incoming packets.
void aprs_msg_callback(struct AX25Msg *msg) {
  Serial.print("Received APRS packet. Data: ");
  for (int i = 0; i < msg->len; i++) { Serial.write(msg->info[i]); }
  Serial.println("");
}

void setup() {
  // Set up serial port
  Serial.begin(9600);
  
  // Initialise APRS library - This starts the modem
  APRS_init();
  
  // You must at a minimum configure your callsign and SSID
  APRS_setCallsign("NOCALL", 1);
  
  // You don't need to set the destination identifier, but
  // if you want to, this is how you do it:
  APRS_setDestination("APZMDM", 0);
  
  // Path parameters are set to sensible values by
  // default, but this is how you can configure them:
  APRS_setPath1("WIDE1", 1);
  APRS_setPath2("WIDE2", 2);
  
  // You can define preamble and tail like this:
  APRS_setPreamble(350);
  APRS_setTail(50);
  
  // You can use the normal or alternate symbol table:
  APRS_useAlternateSymbolTable(false);
  
  // And set what symbol you want to use:
  APRS_setSymbol('n');
  
  // We can print out all the settings
  APRS_printSettings();
}

void locationUpdateExample() {
  // Let's first set our latitude and longtitude.
  // These should be in NMEA format!
  APRS_setLat("5530.80N");
  APRS_setLon("01143.89E");
  
  // We can optionally set power/height/gain/directivity
  // information. These functions accept ranges
  // from 0 to 10, directivity 0 to 9.
  // See this site for a calculator:
  // http://www.aprsfl.net/phgr.php
  // LibAPRS will only add PHG info if all four variables
  // are defined!
  APRS_setPower(2);
  APRS_setHeight(4);
  APRS_setGain(7);
  APRS_setDirectivity(0);
  
  // We'll define a comment string
  char *comment = "LibAPRS location update";
    
  // And send the update
  APRS_sendLoc(comment, sizeof(comment));
  
}

void messageExample() {
  // We first need to set the message recipient
  APRS_setMessageDestination("AA3BBB", 0);
  
  // And define a string to send
  char *message = "Hi there! This is a message";
  APRS_sendMsg(message, sizeof(message));
  
}

void loop() {
  delay(2000);
  locationUpdateExample();
  
  delay(2000);
  messageExample();

  // Just for fun we print out the settings
  APRS_printSettings();

}