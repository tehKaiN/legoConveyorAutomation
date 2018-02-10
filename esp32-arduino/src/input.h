#ifndef _INPUT_H_
#define _INPUT_H_

#include <stdint.h>
#include <Arduino.h>

#define INPUT_DEFAULT_DEBOUNCE_US 100

enum class tPullType: uint8_t {
	NONE = INPUT,
	UP = INPUT_PULLUP,
	DOWN = INPUT_PULLDOWN
};

class tInput {
	public:
		tInput(
			uint8_t ubPin, tPullType ePull = tPullType::NONE, bool isNegated = false,
			uint32_t ulDebounceUs = INPUT_DEFAULT_DEBOUNCE_US
		);

		bool isActive(void);

		bool hasFallen(void);

		bool hasRised(void);

		void process(void);

	private:
		const uint8_t m_ubPin;
		const uint32_t m_ulDebounceUs;
		const bool m_isNegated;
		bool m_isActive;
		bool m_wasActive;
		bool m_isDebouncing;
		uint32_t m_ulDebounceEnd;
};

void inputProcessAll(void);

#endif // _INPUT_H_
