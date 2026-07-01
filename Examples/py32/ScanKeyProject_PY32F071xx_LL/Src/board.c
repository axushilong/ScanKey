/**
 * @file    board.c
 * @author  axushilong@163.com
 * @version v0.0.2
 * @date    2025.12.06
 * @brief   工程板硬件数据结构定义及上电初始化
 */

#include "board.h"



static volatile uint32_t uiTick;
uint32_t uiTickError = 0;

uint32_t uiGetTick(void){
    volatile uint32_t uiTmpTick = uiTick;
    while(uiTick != uiTmpTick){
        uiTmpTick = uiTick;
        uiTickError ++;
    }
    return uiTmpTick;
}

void vDelayNms(uint32_t uiSystick){
    uint32_t uiSystickVal = SysTick->VAL;
    uint32_t uiEndTick = uiGetTick() + uiSystick;
    while(uiGetTick() < uiEndTick){;}
    if (uiGetTick() == uiEndTick){
        while(SysTick->VAL > uiSystickVal){;}/* systick 为向下计数定时器 */
    }
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void){
    uiTick++;
}

uint16_t usSystickFreq = 100;

extern void SystemCoreClockUpdate(void);
/**
  * @brief  System clock configuration function.
  * @param  None
  * @retval None
  */
static void APP_SystemClockConfig(void){
    /* Enable LSI */
    LL_RCC_LSI_Enable();
    while(LL_RCC_LSI_IsReady() != 1){;}

    /* Enable HSI */
    LL_RCC_HSI_Enable();
    LL_RCC_HSI_SetCalibFreq(LL_RCC_HSICALIBRATION_24MHz);
    while(LL_RCC_HSI_IsReady() != 1){;}

    // LL_SetFlashLatency(24000000U);

    /* Set APB prescaler: PCLK = HCLK */
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

    /* Set AHB prescaler: HCLK = SYSCLK */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

    /* Select HSISYS as system clock source */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSISYS);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSISYS){;}

    // LL_Init1msTick(24000000U);//24000000
    DE_SystickInit(24000000U/DE_SystickFrq);

    /* Update the SystemCoreClock global variable(which can be updated also through SystemCoreClockUpdate function) */
    // LL_SetSystemCoreClock(24000000);//24000000
    SystemCoreClockUpdate();
}

/**
  * @brief  Configure PA08 for MCO alternate function
  * @param  None
  * @retval None
  */
static void APP_MCO_GPIOConfig(void){
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_8, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_8, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_8, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_8, LL_GPIO_PULL_NO);
    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_8, LL_GPIO_AF0_MCO);
    LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_PCLK,LL_RCC_MCO1_DIV_128);
}

void vBoardPowerOnInit(void){
    /* System clock configuration */
    APP_SystemClockConfig();

    /* Enable SYSCFG and PWR clock */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* Configure PA08 for MCO alternate function */
    APP_MCO_GPIOConfig();

    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOF);
    //配置开机按键为上拉输入模式，并等待按键释放
    DE_CfgPinInput(DE_PowerKeyPort,DE_PowerKeyPin,LL_GPIO_SPEED_FREQ_LOW,LL_GPIO_PULL_UP);
    while(0 == LL_GPIO_IsInputPinSet(DE_PowerKeyPort,DE_PowerKeyPin)){;}//等待按键释放，不然后续按键识别会有bug
    vDelayNms(2000/(1000/DE_SystickFrq));
    //SWD 烧录接口配置，用于禁用SWD 烧录接口和降低功耗
    DE_SWD_Pin_Init();
    #ifdef EnDebug
        // LL_USART_InitTypeDef stDebugUsart;
        // LL_USART_StructInit(&stDebugUsart);
        // stDebugUsart.BaudRate = De_DebugUsartBaudrate;
        // LL_USART_Init(De_DebugUsart,&stDebugUsart);
        vUsartInit();
        vGetMcuInfo();
    #endif
    // SEGGER_RTT_Init();
    // SEGGER_RTT_printf(0,"SEGGER_RTT_Init ok\r\n");
    PRINT("Hardware Version: %s\r\n",HARDWARE_VERSION);
    PRINT("Software Version: %s\r\n",SOFTWARE_VERSION);
    // cm_backtrace_init("board.c", HARDWARE_VERSION, SOFTWARE_VERSION);

}

/***************** (C) COPYRIGHT axushilong@163.com *****END OF FILE***********/
