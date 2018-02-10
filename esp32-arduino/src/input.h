#ifndef _INPUT_H_
#define _INPUT_H_

#include <stdint.h>
#include <Arduino.h>

#define INPUT_DEFAULT_DEBOUNCE 5

enum class tPullType: uint8_t {
	NONE = INPUT,
	UP = INPUT_PULLUP,
	DOWN = INPUT_PULLDOWN
};

class tInput {
	public:
		tInput(
			uint8_t ubPin, tPullType ePull = tPullType::NONE, bool isNegated = false,
			uint8_t ubDebounceMs = INPUT_DEFAULT_DEBOUNCE
		);

		bool isActive(void);

		void process(void);

	private:
		const uint8_t m_ubPin;
		const uint8_t m_ubDebounceMs;
		const bool m_isNegated;
		bool m_isActive;
		bool m_isDebouncing;
		uint32_t m_ulDebounceEnd;
};

void inputProcessAll(void);

#endif // _INPUT_H_
