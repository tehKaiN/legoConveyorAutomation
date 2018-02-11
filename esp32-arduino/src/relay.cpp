#include "relay.h"
#include "Arduino.h"

void relaySet(uint8_t ubPin, bool isClosed) {
	digitalWrite(ubPin, !isClosed);
}

void relaySetup(uint8_t ubPin) {
	pinMode(ubPin, OUTPUT);
	relaySet(ubPin, false);
}
