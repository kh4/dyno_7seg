#include "board.h"

static uint8_t ledstate;

void ledSet(uint8_t state)
{
  if ((state == 1) || ((state == 2) && (!ledstate))) {
    ledstate=1;
  } else if ((state == 2) && (ledstate)) {
    ledstate=0;
  }
  GPIO_WriteBit(GPIOC, GPIO_Pin_13, !ledstate);
}

bool ledGet()
{
  return ledstate;
}
void ledInit()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    ledSet(LED_OFF);
}


