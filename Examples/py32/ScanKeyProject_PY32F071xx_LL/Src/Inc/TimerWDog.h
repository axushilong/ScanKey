/**
 * @file    TimerWDog.h
 * @author  axushilong@163.com
 * @version v0.0.2
 * @date    2025.12.06
 * @brief   定时器模拟看门狗功能，实现在定时器中断中处理异常重新系统
 */
#ifndef __TIMERWDOG_H__
#define __TIMERWDOG_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "board.h"
#include "py32f071_ll_rcc.h"
#include "py32f071_ll_bus.h"
#include "py32f071_ll_gpio.h"

#define DE_WDogTimer TIM17

#define DE_WDogTimerInit(usNMs) do{ \
    LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_TIM17); \
    LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_TIM17); \
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM17); \
    DE_WDogTimer->CR1 = TIM_CR1_URS; \
    DE_WDogTimer->PSC = 24000 - 1; \
    DE_WDogTimer->ARR = usNMs - 1; \
    DE_WDogTimer->CNT = 0; \
    DE_WDogTimer->CR1 |= TIM_CR1_CEN; \
    DE_WDogTimer->EGR = TIM_EGR_UG; \
    DE_WDogTimer->SR  = 0; \
    DE_WDogTimer->DIER = TIM_DIER_UIE; \
    NVIC_EnableIRQ(TIM17_IRQn); \
    NVIC_SetPriority(TIM17_IRQn,0); \
} while (0)

#define DE_WDogTimerStart() do{ \
    DE_WDogTimer->CR1 |= TIM_CR1_CEN; \
} while (0)

#define DE_WDogTimerStop() do{ \
    DE_WDogTimer->CR1 &= ~TIM_CR1_CEN; \
    DE_WDogTimer->DIER = 0; \
    NVIC_DisableIRQ(TIM17_IRQn); \
    LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_TIM17); \
} while (0)

#define DE_WDogTimerFeeds() do{ \
    DE_WDogTimer->CNT = 0; \
} while (0)


void vTimerWDogInit(TIM_TypeDef * pstTimer,unsigned short usNMs);
void vTimerWDogStart(TIM_TypeDef * pstTimer);
void vTimerWDogStop(TIM_TypeDef * pstTimer);
void vTimerWDogReloadCounter(TIM_TypeDef * pstTimer);

#ifdef __cplusplus
}
#endif

#endif

/***************** (C) COPYRIGHT axushilong@163.com *****END OF FILE***********/
