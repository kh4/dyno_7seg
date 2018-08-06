#pragma once

#define LED_ON 1
#define LED_OFF 0
#define LED_TOGGLE 2

// LED on PC13
void ledInit();
void ledSet(uint8_t state);
bool ledGet();
