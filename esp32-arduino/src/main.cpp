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
	s_IrConveyorStep(IR_CONVEYOR_STEP, tPullType::NONE, false),
	s_IrDispenserBall(IR_DISPENSER_NO_BALL, tPullType::NONE, true),
	s_IrBoxInPosition(IR_BOX_NOT_IN_PLACE, tPullType::NONE, true);

void setup(void) {
	Serial.begin(9600);
	Serial.write("HELO\r\n");

	ledSetup(LED_CONVEYOR);
	ledSetup(LED_DISPENSER);
	ledSetup(LED_MODE);
	ledSetup(LED_STATUS);
	ledSetup(LED_ERROR);

	relaySetup(RELAY_CONVEYOR);
	relaySetup(RELAY_DISPENSER);
}

enum class tControlMode: uint8_t {
	MANUAL = 0,
	AUTO = 1
};

static tControlMode s_eMode = tControlMode::MANUAL;
static bool s_isError = false;

static uint8_t s_ubBallCount = 0;

static bool s_isStatusDone = false;

void errorSet(void) {
	s_isError = true;
	s_isStatusDone = false;
}

void loop(void) {
	bool isRelayConveyorEnabled = false, isRelayDispenserEnabled = false;
	inputProcessAll();

	switch(s_eMode) {
		case tControlMode::MANUAL:
			if(s_ButtonConveyor.isActive() && !s_ButtonDispenser.isActive()) {
				isRelayConveyorEnabled = true;
			}
			else if(s_ButtonDispenser.isActive() && !s_ButtonConveyor.isActive()) {
				isRelayDispenserEnabled = true;
			}

			// Status LED - light up after box reaches dispenser
			// or all balls get dispensed
			if(isRelayConveyorEnabled) {
				if(s_IrBoxInPosition.hasRised()) {
					s_isStatusDone = true;
					s_ubBallCount = 0;
				}
			}
			else if(isRelayDispenserEnabled) {
				if(s_IrDispenserBall.hasRised()) {
					++s_ubBallCount;
				}
				if(s_ubBallCount == 5) {
					s_isStatusDone = true;
				}
				else if(s_ubBallCount > 5) {
					errorSet();
				}
			}
			else {
				s_isStatusDone = false;
			}
			break;
		case tControlMode::AUTO:
			break;
	}

	if(s_isError) {
		isRelayConveyorEnabled = false;
		isRelayDispenserEnabled = false;
	}

	relaySet(RELAY_CONVEYOR, isRelayConveyorEnabled);
	relaySet(RELAY_DISPENSER, isRelayDispenserEnabled);
	ledSet(LED_CONVEYOR, isRelayConveyorEnabled);
	ledSet(LED_DISPENSER, isRelayDispenserEnabled);
	ledSet(LED_MODE, s_eMode == tControlMode::AUTO);
	ledSet(LED_STATUS, s_isStatusDone);
	ledSet(LED_ERROR, s_isError);
}
