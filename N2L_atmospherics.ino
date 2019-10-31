// ask_transmitter.pde
// -*- mode: C++ -*-
// Simple example of how to use RadioHead to transmit messages
// with a simple ASK transmitter in a very simple way.
// Implements a simplex (one-way) transmitter with an TX-C1 module
// Tested on Arduino Mega, Duemilanova, Uno, Due, Teensy, ESP-12

#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver;
// RH_ASK driver(2000, 4, 5, 0); // ESP8266 or ESP32: do not use pin 11 or 2

int rasState = 0;

const int SWITCH_PIN = 8;

const byte SERIAL_BUFFER_LENGTH = 32;
char receivedChars[SERIAL_BUFFER_LENGTH];
boolean newData = false;

void setup()
{
    Serial.begin(9600);	  // Debugging only
    if (!driver.init()) {
         Serial.println("!!! RF Driver init failed !!!");
    }

    Serial.println("Atmospherics Arduino Ready");
    pinMode(SWITCH_PIN, INPUT_PULLUP);
}

void loop()
{

    receiveSerialMessage();

    if (newData == true) {
      processMessage();
    }
    sendRasState();

    delay(50);
}

void receiveSerialMessage() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= SERIAL_BUFFER_LENGTH) {
        ndx = SERIAL_BUFFER_LENGTH - 1;
      }
    } else {
      receivedChars[ndx] = '\0';
      ndx = 0;
      newData = true;
    }
  }
}

void processMessage() {
  char* command = strtok(receivedChars, ":");
  char* argument = strtok(NULL, "");

  if (strcmp(command, "ras")==0) {
    rasState = strcmp(argument, "1") == 0;
  }

  newData = false;
}

void sendRasState() {
  const char *msg = rasState ? "ras:1;" : "ras:0;";
  driver.send((uint8_t *)msg, strlen(msg));
  driver.waitPacketSent();
}
