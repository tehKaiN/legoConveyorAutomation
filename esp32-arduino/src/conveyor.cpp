#include "conveyor.h"
#include "relay.h"

tConveyor::tConveyor(
	uint8_t ubIrStepPin, uint8_t ubIrBoxPin, uint8_t ubRelayPin
):
	m_ulMinTimeForMoveCheck(1000),
	m_ulMaxTimeBetweenSteps(1000),
	m_ubStepsPerRevolution(12),
	m_ulStepsAfterBoxCrossIr(3),
	m_IrStep(ubIrStepPin, tPullType::NONE, false),
	m_IrBoxInPlace(ubIrBoxPin, tPullType::NONE, true),
	m_ubRelayPin(ubRelayPin)
{
	relaySetup(ubRelayPin);
}

void tConveyor::setMoving(bool isMoveRequest)
{
	if(m_isError) {
		m_isMoveRequest = 0;
	}
	else {
		m_isMoveRequest = isMoveRequest;
	}
}

bool tConveyor::isMoving(void)
{
	return m_isMoveRequest && !m_isError;
}

bool tConveyor::isLongEnoughForMoveCheck(void)
{
	return m_isLongEnoughForMoveCheck;
}

bool tConveyor::isError(void)
{
	return m_isError;
}

void tConveyor::clearError(void)
{
	m_isError = false;
}

void tConveyor::processLogic(void) {
	uint32_t ulNow = millis();
	if(m_isMoveRequest) {
		if(m_ulMoveStart == 0) {
			m_ulMoveStart = ulNow;
			m_ulLastStepTime = ulNow;
		}
		if(ulNow >= m_ulMoveStart + m_ulMinTimeForMoveCheck) {
			m_isLongEnoughForMoveCheck = true;
		}
		if(m_IrStep.hasRised() || m_IrStep.hasFallen()) {
			++m_ulSteps;
			m_ulLastStepTime = ulNow;
		}
		if(m_IrBoxInPlace.hasRised()) {
			m_ulBoxOkStartSteps = m_ulSteps + m_ulStepsAfterBoxCrossIr;
		}
		if(
			isLongEnoughForMoveCheck() &&
			ulNow - m_ulLastStepTime > m_ulMaxTimeBetweenSteps
		) {
			m_isError = true;
		}
	}
	else {
		m_ulSteps = 0;
		m_ulLastStepTime = 0;
		m_ulMoveStart = 0;
		m_ulBoxOkStartSteps = -1;
	}
}

void tConveyor::processControl(void) {
	if(m_isError) {
		relaySet(m_ubRelayPin, 0);
	}
	else {
		relaySet(m_ubRelayPin, m_isMoveRequest);
	}
}

bool tConveyor::isBoxInPosition(void) {
	return m_ulSteps >= m_ulBoxOkStartSteps;
}

float tConveyor::getRpm(void) {
	if(m_ulMoveStart == 0) {
		return 0.0f;
	}
	uint32_t ulDelta = millis() - m_ulMoveStart;

	// 3-4 steps per second - 180-240 steps per minute - 30-40 RPM
	return (((m_ulSteps*60)/m_ubStepsPerRevolution)*1000.0f)/ulDelta;
}
