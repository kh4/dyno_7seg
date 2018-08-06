#pragma once

// for roundf()
#define __USE_C99_MATH

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "stm32f10x_conf.h"
#include "core_cm3.h"
#include "printf.h"

// Chip Unique ID on F103
#define U_ID_0 (*(uint32_t*)0x1FFFF7E8)
#define U_ID_1 (*(uint32_t*)0x1FFFF7EC)
#define U_ID_2 (*(uint32_t*)0x1FFFF7F0)

#include "drv_system.h"         // timers, delays, etc
#include "drv_uart.h"
#include "drv_led_pc13.h"
#include "drv_adc.h"
#include "drv_7seg8.h"
