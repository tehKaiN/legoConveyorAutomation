#include <Arduino.h>
#include "input.h"
#include "conveyor.h"
#include "dispenser.h"

// ESP32:
// Pin 0 doesn't work well for btn.
// Pins 34-39 are input only and don't have pullup/pulldown capability.

#define LED_CONVEYOR 27
#define LED_DISPENSER 26
#define LED_MODE 25
#define LED_STATUS 33
#define LED_EMERGENCY 32

#define BTN_CONVEYOR 13
#define BTN_DISPENSER 15
#define BTN_MODE 2
#define BTN_STATUS 5
#define BTN_EMERGENCY 4

#define RELAY_CONVEYOR 12
#define RELAY_DISPENSER 14

#define IR_CONVEYOR_STEP 17
#define IR_DISPENSER_NO_BALL 18
#define IR_BOX_NOT_IN_PLACE 19
#define IR_EXTERNAL_VOLTAGE 35

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

tInput
	// Buttons
	s_ButtonConveyor(BTN_CONVEYOR, tPullType::DOWN),
	s_ButtonDispenser(BTN_DISPENSER, tPullType::DOWN),
	s_ButtonMode(BTN_MODE, tPullType::DOWN),
	s_ButtonStatus(BTN_STATUS, tPullType::DOWN),
	s_ButtonError(BTN_EMERGENCY, tPullType::DOWN),
	// External voltage
	s_ExternalVoltage(IR_EXTERNAL_VOLTAGE, tPullType::NONE);

tConveyor s_Conveyor(IR_CONVEYOR_STEP, IR_BOX_NOT_IN_PLACE, RELAY_CONVEYOR);
tDispenser s_Dispenser(IR_DISPENSER_NO_BALL, RELAY_DISPENSER);

const uint8_t s_ubBallCountGoal = 2;

void setup(void) {
	Serial.begin(57600);
	Serial.write("HELO\r\n");

	ledSetup(LED_CONVEYOR);
	ledSetup(LED_DISPENSER);
	ledSetup(LED_MODE);
	ledSetup(LED_STATUS);
	ledSetup(LED_EMERGENCY);
}

enum class tControlMode: uint8_t {
	MANUAL = 0,
	AUTO = 1
};

static tControlMode s_eMode = tControlMode::MANUAL;
static bool s_isError = false;
static bool s_isStatusDone = false;

enum class tAutoState: uint8_t {
	GOING_TO_DISPENSER,
	DISPENSING_BALLS,
	WAITING_FOR_FALL
};

tAutoState s_eAutoState = tAutoState::GOING_TO_DISPENSER;

void errorSet(void) {
	s_isError = true;
	s_isStatusDone = false;
	s_eMode = tControlMode::MANUAL;
}

void errorClear(void) {
	s_isError = false;
}

void loop(void) {
	bool isConveyorLedHigh = false, isDispenserLedHigh = false, isModeLedHigh = false;
	uint32_t ulStart = micros();
	bool isRelayConveyorRequest = false, isRelayDispenserRequest = false;
	inputProcessAll();
	s_Conveyor.processLogic();
	s_Dispenser.processLogic();

	switch(s_eMode) {
		case tControlMode::MANUAL:
			if(s_ButtonConveyor.isActive() && !s_ButtonDispenser.isActive()) {
				isRelayConveyorRequest = true;
				isConveyorLedHigh = true;
			}
			else if(s_ButtonDispenser.isActive() && !s_ButtonConveyor.isActive()) {
				isRelayDispenserRequest = true;
				isDispenserLedHigh = true;
			}

			if(
				!s_ButtonDispenser.isActive() && !s_ButtonConveyor.isActive() &&
				s_ButtonMode.hasRised()
			) {
				s_eMode = tControlMode::AUTO;
				s_eAutoState = tAutoState::GOING_TO_DISPENSER;
			}
			break;
		case tControlMode::AUTO:
			isModeLedHigh = (millis() / 1000) & 1;

			switch(s_eAutoState) {
				case tAutoState::GOING_TO_DISPENSER:
						if(!s_Conveyor.isBoxInPosition()) {
							isRelayConveyorRequest = true;
							isConveyorLedHigh = true;
						}
						else {
							s_eAutoState = tAutoState::DISPENSING_BALLS;
						}
					break;
				case tAutoState::DISPENSING_BALLS:
					if(s_Dispenser.getBallCount() >= s_ubBallCountGoal) {
						s_eAutoState = tAutoState::WAITING_FOR_FALL;
					}
					else {
						isRelayDispenserRequest = true;
						isDispenserLedHigh = true;
					}
					break;
				case tAutoState::WAITING_FOR_FALL:
					if(s_Dispenser.hasLastBallFallen()) {
						s_eAutoState = tAutoState::GOING_TO_DISPENSER;
					}
					break;
			}

			if(s_ButtonMode.hasRised()) {
				s_eMode = tControlMode::MANUAL;
			}
			break;
	}

	// Status LED - light up after box reaches dispenser
	// or all balls get dispensed
	if(isRelayConveyorRequest) {
		s_Dispenser.resetBallCounter();
		if(s_Conveyor.isBoxInPosition()) {
			s_isStatusDone = true;
		}
	}
	else if(isRelayDispenserRequest) {
		if(s_Dispenser.getBallCount() >= s_ubBallCountGoal) {
			s_isStatusDone = true;
		}
	}
	else {
		s_isStatusDone = false;
	}

	if(
		s_ButtonError.isActive() || !s_ExternalVoltage.isActive() ||
		s_Conveyor.isError() || s_Dispenser.isError()
	) {
		errorSet();
	}

	if(s_isError) {
		isConveyorLedHigh = false;
		isDispenserLedHigh = false;
		isRelayConveyorRequest = false;
		isRelayDispenserRequest = false;
		bool isErrorRelatedHigh = (millis() / 200) & 1;
		if(!s_ExternalVoltage.isActive()) {
			isDispenserLedHigh = isErrorRelatedHigh;
			isConveyorLedHigh = isErrorRelatedHigh;
		}
		else if(s_Conveyor.isError()) {
			isConveyorLedHigh = isErrorRelatedHigh;
		}
		else if(s_Dispenser.isError()) {
			isDispenserLedHigh = isErrorRelatedHigh;
		}

		if(
			s_ButtonStatus.hasRised() && !s_ButtonError.isActive() &&
			s_ExternalVoltage.isActive()
		) {
			// Everything's ok - clear error
			s_Conveyor.clearError();
			s_Dispenser.clearError();
			errorClear();
		}
	}

	s_Conveyor.setMoving(isRelayConveyorRequest);
	s_Conveyor.processControl();
	s_Dispenser.setMoving(isRelayDispenserRequest);
	s_Dispenser.processControl();
	ledSet(LED_CONVEYOR, isConveyorLedHigh);
	ledSet(LED_DISPENSER, isDispenserLedHigh);
	ledSet(LED_MODE, s_eMode == tControlMode::AUTO);
	ledSet(LED_STATUS, s_isStatusDone);
	ledSet(LED_EMERGENCY, s_isError);
	uint32_t ulEnd = micros();
	Serial.printf(
		"Loop took %lu us, RPM: %.2f, balls: %hhu, BPM: %.2f\r\n",
		ulEnd-ulStart, s_Conveyor.getRpm(),
		s_Dispenser.getBallCount(), s_Dispenser.getBpm()
	);
}
