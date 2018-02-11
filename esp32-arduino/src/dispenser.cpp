#include "dispenser.h"
#include "relay.h"
#include <Arduino.h>

tDispenser::tDispenser(uint8_t ubIrPin, uint8_t ubRelayPin):
	m_ulBallFallDuration(1000),
	m_ulMinTimeForMoveCheck(2000),
	m_ulMaxTimeBetweenBalls(2000),
	m_ubRelayPin(ubRelayPin),
	m_IrBall(ubIrPin, tPullType::NONE, true)
{
	relaySetup(ubRelayPin);
}

void tDispenser::resetBallCounter(void)
{
	m_ubBallCountTotal = 0;
	m_ubBallCountSinceMotorStart = 0;
}

uint8_t tDispenser::getBallCount(void)
{
	return m_ubBallCountTotal;
}

bool tDispenser::hasLastBallFallen(void)
{
	return millis() >= m_ulLastBallTime + m_ulBallFallDuration;
}

void tDispenser::setMoving(bool isMoving)
{
	m_isMoveRequest = isMoving && !m_isError;
}

bool tDispenser::isLongEnoughForMoveCheck(void)
{
	return (m_ulMotorStartTime != 0) &&
		(millis() >= m_ulMotorStartTime + m_ulMinTimeForMoveCheck);
}

bool tDispenser::isMoving(void)
{
	return m_isMoveRequest && !m_isError;
}

bool tDispenser::isError(void)
{
	return m_isError;
}

void tDispenser::clearError(void)
{
	m_isError = false;
	m_ulMotorStartTime = 0;
}

void tDispenser::processLogic(void)
{
	uint32_t ulNow = millis();
	if(
		isMoving() && isLongEnoughForMoveCheck() &&
		(ulNow > m_ulLastBallTime + m_ulMaxTimeBetweenBalls)
	) {
		m_isError = true;
	}

	if(m_IrBall.hasRised()) {
		++m_ubBallCountTotal;
		++m_ubBallCountSinceMotorStart;
		m_ulLastBallTime = ulNow;
	}
}

void tDispenser::processControl(void)
{
	if(m_isMoveRequest && m_ulMotorStartTime == 0) {
		m_ubBallCountSinceMotorStart = 0;
		m_ulMotorStartTime = millis();
	}
	else if(!m_isMoveRequest) {
		m_ulMotorStartTime = 0;
	}
	relaySet(m_ubRelayPin, m_isMoveRequest);
}

float tDispenser::getBpm(void)
{
	if(m_ulMotorStartTime == 0) {
		return 0;
	}
	uint32_t ulDelta = m_ulLastBallTime - m_ulMotorStartTime;
	return ((m_ubBallCountSinceMotorStart * 60) * 1000.0f) / ulDelta;
}
