#include "board.h"

// cycles per microsecond
static volatile uint32_t usTicks = 0;
// current uptime for 1kHz systick timer. will rollover after 49 days. hopefully we won't care.
static volatile uint32_t sysTickUptime = 0;

static void cycleCounterInit(void)
{
    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);
    usTicks = clocks.SYSCLK_Frequency / 1000000;
}

void (*hooks[4])() = {0,0,0,0};

// SysTick
void SysTick_Handler(void)
{
    sysTickUptime++;
    for (int i = 0; i < 4; i++) {
      if (hooks[i]) hooks[i]();
    }
}

void systickAddHook(void (*f)()) {
  for (int i = 0; i < 4; i++)
    if (!hooks[i]) {
      hooks[i] = f;
      break;
    }
}

// Return system uptime in microseconds (rollover in 70minutes)
uint32_t micros(void)
{
    register uint32_t ms, cycle_cnt;
    do {
        ms = sysTickUptime;
        cycle_cnt = SysTick->VAL;
    } while (ms != sysTickUptime);
    return (ms * 1000) + (72000 - cycle_cnt) / 72;
}

// Return system uptime in milliseconds (rollover in 49 days)
uint32_t millis(void)
{
    return sysTickUptime;
}

void systemInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // This is needed because some shit inside Keil startup fucks with SystemCoreClock, setting it back to 72MHz even on HSI.
    SystemCoreClockUpdate();

    // Turn on clocks for stuff we use
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4 | RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM1 | RCC_APB2Periph_ADC1 | RCC_APB2Periph_USART1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_ClearFlag();

    // Make all GPIO in by default to save power and reduce noise
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Turn off JTAG port as we only plan to use SWD
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    // Init cycle counter
    cycleCounterInit();

    // SysTick
    SysTick_Config(SystemCoreClock / 1000);
}

void delayMicroseconds(uint32_t us)
{
    uint32_t now = micros();
    while (micros() - now < us);
}

void delay(uint32_t ms)
{
    while (ms--)
        delayMicroseconds(1000);
}

#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000)

void systemReset(bool toBootloader)
{
    if (toBootloader) {
        // 1FFFF000 -> 20000200 -> SP
        // 1FFFF004 -> 1FFFF021 -> PC
        *((uint32_t *)0x20004FF0) = 0xDEADBEEF; // 20KB STM32F103
    }

    // Generate system reset
    SCB->AIRCR = AIRCR_VECTKEY_MASK | (uint32_t)0x04;
}
