#include "input.h"
#include <Arduino.h>
#include <vector>

static uint8_t s_ubInputNext = 0;
static tInput *s_pInputs[20];

tInput::tInput(
	uint8_t ubPin, tPullType ePull, bool isNegated, uint32_t ulDebounceUs
):
	m_ubPin(ubPin),
	m_ulDebounceUs(ulDebounceUs),
	m_isNegated(isNegated),
	m_isActive(false),
	m_wasActive(false)
{
	pinMode(m_ubPin, static_cast<uint8_t>(ePull));
	s_pInputs[s_ubInputNext++] = this;
}

bool tInput::isActive(void)
{
	return m_isActive ^ m_isNegated;
}

bool tInput::hasFallen(void)
{
	if(!m_isNegated) {
		return !m_isActive && m_wasActive;
	}
	else {
		return m_isActive && !m_wasActive;
	}
}

bool tInput::hasRised(void)
{
	if(!m_isNegated) {
		return m_isActive && !m_wasActive;
	}
	else {
		return !m_isActive && m_wasActive;
	}
}

void tInput::process(void)
{
	// Putting it here will ensure that edge detection occurs exactly one time
	m_wasActive = m_isActive;

	// Read current state and debounce
	bool isNowActive = digitalRead(m_ubPin);
	if(m_isDebouncing) {
		if(isNowActive == m_isActive) {
			// Still bouncing - reset timer
			m_ulDebounceEnd = micros() + m_ulDebounceUs;
		}
		else if(micros() >= m_ulDebounceEnd) {
			m_isDebouncing = false;
			m_isActive = isNowActive;
		}
	}
	else {
		if(isNowActive != m_isActive) {
			m_isDebouncing = true;
			m_ulDebounceEnd = micros() + m_ulDebounceUs;
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
