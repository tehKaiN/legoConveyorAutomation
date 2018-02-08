#include <Arduino.h>

// ESP32:
// Pin 0 doesn't work well for btn.
// Pins 34-39 are input only and don't have pullup/pulldown capability.

#define LED_CONVEYOR 27
#define LED_DISPENSER 26
#define LED_MODE 25
#define LED_STATUS 33
#define LED_ERROR 32

#define BTN_CONVEYOR 13
#define BTN_DISPENSER 15
#define BTN_MODE 2
#define BTN_STATUS 5
#define BTN_ERROR 4

#define RELAY_CONVEYOR 12
#define RELAY_DISPENSER 14

#define IR_CONVEYOR_STEP 17
#define IR_BALL_DISPENSED 18
#define IR_BOX_IN_PLACE 19

void ledSet(uint8_t ubPin, bool isHigh) {
	// LEDs have inverted logic - they're on when set to 0
	digitalWrite(ubPin, !isHigh);
}

void ledSetup(uint8_t ubPin) {
	// Do a blink using LED to check if it's ok
	pinMode(ubPin, OUTPUT);
	ledSet(ubPin, 1);
	delay(50);
	ledSet(ubPin, 0);
}

void relaySetup(uint8_t ubPin) {
	pinMode(ubPin, OUTPUT);
}

void relaySet(uint8_t ubPin, bool isClosed) {
	digitalWrite(ubPin, isClosed);
}

void setup() {
    // put your setup code here, to run once:
		// pinMode(22, OUTPUT);
		// pinMode(17, OUTPUT);
		// pinMode(19, OUTPUT);
		// pinMode(32, INPUT);
		Serial.begin(9600);

		ledSetup(LED_CONVEYOR);
		ledSetup(LED_DISPENSER);
		ledSetup(LED_MODE);
		ledSetup(LED_STATUS);
		ledSetup(LED_ERROR);

		pinMode(BTN_CONVEYOR, INPUT_PULLDOWN);
		pinMode(BTN_DISPENSER, INPUT_PULLDOWN);
		pinMode(BTN_MODE, INPUT_PULLDOWN);
		pinMode(BTN_STATUS, INPUT_PULLDOWN);
		pinMode(BTN_ERROR, INPUT_PULLDOWN);

}

void loop() {
	// static bool isPrevHigh = true;
	// // put your main code here, to run repeatedly:

	// bool isHigh = digitalRead(32);

	// digitalWrite(22, isHigh);
	// delay(10);
	// if(!isHigh && isPrevHigh) {
	// 	Serial.write("zb\r\n");
	// }

	// isPrevHigh = isHigh;

	// delay(1000);
	// digitalWrite(17, 0);
	// digitalWrite(19, 1);

	// delay(1000);
	// digitalWrite(17, 1);
	// digitalWrite(19, 0);

	ledSet(LED_CONVEYOR, digitalRead(BTN_CONVEYOR));
	ledSet(LED_DISPENSER, digitalRead(BTN_DISPENSER));
	ledSet(LED_MODE, digitalRead(BTN_MODE));
	ledSet(LED_STATUS, digitalRead(BTN_STATUS));
	ledSet(LED_ERROR, digitalRead(BTN_ERROR));
	delay(100);
}
