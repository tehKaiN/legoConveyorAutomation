#ifndef _DISPENSER_H_
#define _DISPENSER_H_

#include <stdint.h>
#include "input.h"

class tDispenser {
public:
	tDispenser(uint8_t ubIrPin, uint8_t ubRelayPin);
	void resetBallCounter(void);
	uint8_t getBallCount(void);
	bool hasLastBallFallen(void);
	void setMoving(bool isMoving);
	bool isLongEnoughForMoveCheck(void);
	bool isMoving(void);
	bool isError(void);
	void clearError(void);
	void processLogic(void);
	void processControl(void);
	float getBpm(void);

private:
	const uint32_t m_ulBallFallDuration;
	const uint32_t m_ulMinTimeForMoveCheck;
	const uint32_t m_ulMaxTimeBetweenBalls;
	tInput m_IrBall;
	uint8_t m_ubRelayPin;
	uint8_t m_ubBallCountTotal;
	uint8_t m_ubBallCountSinceMotorStart;
	uint32_t m_ulLastBallTime;
	uint32_t m_ulMotorStartTime;
	bool m_isError;
	bool m_isMoveRequest;
};

#endif // _DISPENSER_H_
