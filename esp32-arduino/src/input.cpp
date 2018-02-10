#include "input.h"
#include <Arduino.h>
#include <vector>

static uint8_t s_ubInputNext = 0;
static tInput *s_pInputs[20];

tInput::tInput(
	uint8_t ubPin, tPullType ePull, bool isNegated, uint8_t ubDebounceMs
):
	m_ubPin(ubPin),
	m_ubDebounceMs(ubDebounceMs),
	m_isNegated(isNegated)
{
	pinMode(m_ubPin, static_cast<uint8_t>(ePull));
	s_pInputs[s_ubInputNext++] = this;
}

bool tInput::isActive(void)
{
	return m_isActive ^ m_isNegated;
}

void tInput::process(void)
{
	bool isNowActive = digitalRead(m_ubPin);
	if(m_isDebouncing) {
		if(millis() >= m_ubDebounceMs) {
			m_isDebouncing = false;
			m_isActive = isNowActive;
		}
	}
	else {
		if(isNowActive != m_isActive) {
			m_isDebouncing = true;
			m_ulDebounceEnd = millis() + m_ubDebounceMs;
		}
	}
}

void inputProcessAll(void) {
	for(uint8_t i = 0; i < s_ubInputNext; ++i) {
		s_pInputs[i]->process();
	}
	// for(auto input: s_vInputs) {
	// 	input->process();
	// }
}
