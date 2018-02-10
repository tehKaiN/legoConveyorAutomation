#include <Arduino.h>
#include <input.h>

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
#define IR_DISPENSER_NO_BALL 18
#define IR_BOX_NOT_IN_PLACE 19

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

void relaySet(uint8_t ubPin, bool isClosed) {
	digitalWrite(ubPin, !isClosed);
}

void relaySetup(uint8_t ubPin) {
	pinMode(ubPin, OUTPUT);
	relaySet(ubPin, false);
}

// Buttons
tInput
	s_ButtonConveyor(BTN_CONVEYOR, tPullType::DOWN),
	s_ButtonDispenser(BTN_DISPENSER, tPullType::DOWN),
	s_ButtonMode(BTN_MODE, tPullType::DOWN),
	s_ButtonStatus(BTN_STATUS, tPullType::DOWN),
	s_ButtonError(BTN_ERROR, tPullType::DOWN);

// IRs
tInput
	s_IrConveyorStep(IR_CONVEYOR_STEP, tPullType::NONE, false, 1),
	s_IrDispenserBall(IR_DISPENSER_NO_BALL, tPullType::NONE, true, 1),
	s_IrBoxInPosition(IR_BOX_NOT_IN_PLACE, tPullType::NONE, true, 1);

void setup(void) {
	Serial.begin(9600);

	ledSetup(LED_CONVEYOR);
	ledSetup(LED_DISPENSER);
	ledSetup(LED_MODE);
	ledSetup(LED_STATUS);
	ledSetup(LED_ERROR);

	relaySetup(RELAY_CONVEYOR);
	relaySetup(RELAY_DISPENSER);
}

void loop(void) {
	inputProcessAll();

	ledSet(LED_CONVEYOR, s_IrConveyorStep.isActive());
	ledSet(LED_DISPENSER, s_IrDispenserBall.isActive());
	ledSet(LED_STATUS, s_IrBoxInPosition.isActive());

	relaySet(RELAY_CONVEYOR, s_ButtonConveyor.isActive());
	relaySet(RELAY_DISPENSER, s_ButtonDispenser.isActive());
}
