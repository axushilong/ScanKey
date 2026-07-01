/**
 * @file    TimerWDog.c
 * @author  axushilong@163.com
 * @version v0.0.2
 * @date    2025.12.06
 * @brief   定时器模拟看门狗功能，实现在定时器中断中处理异常重新系统
 */
#include "TimerWDog.h"
#include "core_cm0plus.h"

void vTimerWDogInit(TIM_TypeDef * pstTimer,unsigned short usNMs){
        LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_TIM17);
        LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_TIM17);
        LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM17);
    pstTimer->CR1 = TIM_CR1_URS;
    pstTimer->PSC = 24000 - 1;// 24M /24000 = 1ms
    pstTimer->ARR = usNMs - 1;
    pstTimer->CNT = 0;
    pstTimer->CR1 |= TIM_CR1_CEN;
    pstTimer->EGR = TIM_EGR_UG;
    pstTimer->DIER = TIM_DIER_UIE;
    pstTimer->SR  = 0;
        NVIC_EnableIRQ(TIM17_IRQn);
        NVIC_SetPriority(TIM17_IRQn,0);
}

void vTimerWDogStart(TIM_TypeDef * pstTimer){
    pstTimer->CR1 |= TIM_CR1_CEN;
}

void vTimerWDogStop(TIM_TypeDef * pstTimer){
    pstTimer->CR1 &= ~TIM_CR1_CEN;
    pstTimer->DIER = 0;
}

void vTimerWDogReloadCounter(TIM_TypeDef * pstTimer){
    pstTimer->CNT = 0;
}

void TIM17_IRQHandler(void){
    unsigned int uiSR = TIM17->SR;
    TIM17->SR = 0;
    if(uiSR & TIM_SR_UIF){
        PRINT("TIM17_IRQHandler\r\n");
        NVIC_SystemReset();
    }
}
void HardFault_Handler(void){
    PRINT("HardFault_Handler\r\n");
    NVIC_SystemReset();
}

/***************** (C) COPYRIGHT axushilong@163.com *****END OF FILE***********/
