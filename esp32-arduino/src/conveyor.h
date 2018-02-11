#ifndef _CONVEYOR_H_
#define _CONVEYOR_H_

#include "input.h"
#include <cstdint>

class tConveyor {
	public:
		tConveyor(uint8_t ubIrStepPin, uint8_t ubIrBoxPin, uint8_t ubRelayPin);
		void setMoving(bool isMoving);
		bool isLongEnoughForMoveCheck(void);
		bool isMoving(void);
		bool isError(void);
		void clearError(void);
		void processLogic(void);
		void processControl(void);
		bool isBoxInPosition(void);
		float getRpm(void);

	private:
		const uint32_t m_ulMinTimeForMoveCheck;
		const uint32_t m_ulMaxTimeBetweenSteps;
		const uint8_t m_ubStepsPerRevolution;
		const uint32_t m_ulStepsAfterBoxCrossIr;
		uint32_t m_ulMoveStart;
		uint32_t m_ulLastStepTime;
		uint32_t m_ulSteps;
		uint32_t m_ulBoxOkStartSteps;
		tInput m_IrStep;
		tInput m_IrBoxInPlace;
		uint8_t m_ubRelayPin;
		bool m_isError;
		bool m_isMoveRequest;
		bool m_isLongEnoughForMoveCheck;
};

#endif // _CONVEYOR_H_
