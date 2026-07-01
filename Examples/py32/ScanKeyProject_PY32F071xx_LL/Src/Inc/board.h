/**
 * @file    board.h
 * @author  axushilong@163.com
 * @version v0.0.2
 * @date    2025.12.06
 * @brief   工程板硬件数据结构定义及上电初始化
 */
#ifndef __BOARD_H__
#define __BOARD_H__


#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <stdio.h>
#include "py32f071_ll_rcc.h"
#include "py32f071_ll_bus.h"
#include "py32f071_ll_utils.h"
#include "py32f071_ll_gpio.h"
#include "py32f071_ll_exti.h"
#include "py32f071_ll_cortex.h"
#include "py32f071_ll_pwr.h"
#include "core_cm0plus.h"

/* 定义3种开关机模式：关机低功耗模式、正常开机工作、充电模式 */
#define DE_PowerModeOff         0
#define DE_PowerModeOn          1
#define DE_PowerModeCharge      2

#define HARDWARE_VERSION               "V0.1.0"
#define SOFTWARE_VERSION               "V0.0.1"
#define DE_SystickFrq   1000

#define __Concat(a,b) a##b
#define _Concat(a,b) __Concat(a,b)
#define DE_Concat(a,b) _Concat(a,b)

#ifdef EnDebug
    // DebugDisable未定义时，PRINT执行实际打印操作
    #define PRINT(...) printf(__VA_ARGS__)
#else
#ifdef EnRTT
    // DebugDisable定义时，PRINT不执行任何操作
    #define PRINT(...) SEGGER_RTT_printf(0, __VA_ARGS__);
#else
    #define PRINT(...)
#endif

#endif

#define DE_SystickInit(tick) do{ \
    /* Configure the SysTick to have interrupt in Nms time base */ \
    SysTick->LOAD  = (uint32_t)(tick - 1UL); /* set reload register */ \
    SysTick->VAL   = 0;           /* Load the SysTick Counter Value */ \
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;/* Enable the Systick Timer */ \
    __IO uint32_t  tmp = SysTick->CTRL;  /* Clear the COUNTFLAG first */ \
    ((void)tmp); \
    __NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */ \
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; \
}while(0)

#define DE_SystickStart() do{ \
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; \
}while(0)

#define DE_SystickStop() do{ \
    __IO uint32_t  tmp = SysTick->CTRL;  /* Clear the COUNTFLAG first */ \
    ((void)tmp); \
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk; \
}while(0)

#define DE_EnterLpStopMode()  do{ \
    /*DE_SystickStop();*/ \
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR); \
    /* VCORE = 0.8V when enter stop mode */ \
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_0P8V); \
    /* Enable Low Power Run mode */ \
    LL_PWR_EnableLowPowerRunMode(); \
    /* Enter DeepSleep mode */ \
    LL_LPM_EnableDeepSleep(); \
    /* Request Wait For Event */ \
    __SEV(); \
    __WFE(); \
    __WFE(); \
    LL_LPM_EnableSleep(); \
    /*DE_SystickStart();*/ \
}while(0)

/*正常gpio复位是模拟输入模式，推挽输出，非常低速，无上下拉,AF0*/
#define DE_CfgAllIOReset() do{ \
    LL_IOP_GRP1_ForceReset(LL_IOP_GRP1_PERIPH_GPIOA); \
    LL_IOP_GRP1_ReleaseReset(LL_IOP_GRP1_PERIPH_GPIOA); \
    LL_IOP_GRP1_ForceReset(LL_IOP_GRP1_PERIPH_GPIOB); \
    LL_IOP_GRP1_ReleaseReset(LL_IOP_GRP1_PERIPH_GPIOB); \
    LL_IOP_GRP1_ForceReset(LL_IOP_GRP1_PERIPH_GPIOC); \
    LL_IOP_GRP1_ReleaseReset(LL_IOP_GRP1_PERIPH_GPIOC); \
    LL_IOP_GRP1_ForceReset(LL_IOP_GRP1_PERIPH_GPIOF); \
    LL_IOP_GRP1_ReleaseReset(LL_IOP_GRP1_PERIPH_GPIOF); \
}while(0)

#define DE_CfgPin(port,pin,mode,speed,upDown) do{ \
    LL_GPIO_SetPinMode(port, pin, mode); \
    LL_GPIO_SetPinSpeed(port, pin, speed); \
    LL_GPIO_SetPinPull(port, pin, upDown); \
}while(0)

#define DE_CfgPinInput(port,pin,speed,upDown) do{ \
    DE_CfgPin(port,pin,LL_GPIO_MODE_INPUT,speed,upDown); \
}while(0)

#define DE_CfgPinOutput(port,pin,speed,upDown) do{ \
    DE_CfgPin(port,pin,LL_GPIO_MODE_OUTPUT,speed,upDown); \
}while(0)

#define DE_CfgPinAlternate(port,pin,speed,upDown) do{ \
    DE_CfgPin(port,pin,LL_GPIO_MODE_ALTERNATE,speed,upDown); \
}while(0)

#define DE_CfgPinAnalog(port,pin,speed,upDown) do{ \
    DE_CfgPin(port,pin,LL_GPIO_MODE_ANALOG,speed,upDown); \
}while(0)

#define DE_CfgPinPPOut(port,pin,pinSpeed,upDown) do{ \
    DE_CfgPin(port,pin,LL_GPIO_MODE_OUTPUT,pinSpeed,upDown); \
    /* LL_GPIO_SetPinMode(port, pin, LL_GPIO_MODE_OUTPUT); */ \
    /* LL_GPIO_SetPinSpeed(port, pin, pinSpeed); */ \
    /* LL_GPIO_SetPinPull(port, pin, LL_GPIO_PULL_NO); */ \
    LL_GPIO_SetPinOutputType(port, pin, LL_GPIO_OUTPUT_PUSHPULL); \
}while(0)

#define DE_CfgPinODOut(port,pin,pinSpeed,upDown) do{ \
    LL_GPIO_ResetOutputPin(port, pin); \
    DE_CfgPin(port,pin,LL_GPIO_MODE_OUTPUT,pinSpeed,upDown); \
    /* LL_GPIO_SetPinMode(port, pin, LL_GPIO_MODE_OUTPUT); */ \
    /* LL_GPIO_SetPinSpeed(port, pin, pinSpeed); */ \
    /* LL_GPIO_SetPinPull(port, pin, LL_GPIO_PULL_NO); */ \
    LL_GPIO_SetPinOutputType(port, pin, LL_GPIO_OUTPUT_OPENDRAIN); \
    /* LL_GPIO_SetOutputPin(port, pin); */ \
}while(0)

#define DE_CfgPinRisingWFE(portIndex,pinNum,speed,upDown) do{ \
    /* LL_GPIO_SetPinMode(DE_Concat(GPIO,portIndex), DE_Concat(LL_GPIO_PIN_,pinNum), LL_GPIO_MODE_INPUT); */ \
    /* LL_GPIO_SetPinSpeed(DE_Concat(GPIO,portIndex), DE_Concat(LL_GPIO_PIN_,pinNum), LL_GPIO_SPEED_FREQ_LOW); */ \
    /* LL_GPIO_SetPinPull(DE_Concat(GPIO,portIndex), DE_Concat(LL_GPIO_PIN_,pinNum), upDown); */ \
    DE_CfgPinInput(DE_Concat(GPIO,portIndex), DE_Concat(LL_GPIO_PIN_,pinNum),speed,upDown); \
    LL_EXTI_SetEXTISource(DE_Concat(LL_EXTI_CONFIG_PORT,portIndex),DE_Concat(LL_EXTI_CONFIG_LINE,pinNum)); \
    LL_EXTI_DisableIT(DE_Concat(LL_EXTI_LINE_,pinNum)); \
    LL_EXTI_EnableEvent(DE_Concat(LL_EXTI_LINE_,pinNum)); \
    LL_EXTI_DisableFallingTrig(DE_Concat(LL_EXTI_LINE_,pinNum)); \
    LL_EXTI_EnableRisingTrig(DE_Concat(LL_EXTI_LINE_,pinNum)); \
}while(0)

#define DE_CfgPinFallingWFE(portIndex,pinNum,speed,upDown) do{ \
    DE_CfgPinInput(DE_Concat(GPIO,portIndex), DE_Concat(LL_GPIO_PIN_,pinNum),speed,upDown); \
    LL_EXTI_SetEXTISource(DE_Concat(LL_EXTI_CONFIG_PORT,portIndex),DE_Concat(LL_EXTI_CONFIG_LINE,pinNum)); \
    LL_EXTI_DisableIT(DE_Concat(LL_EXTI_LINE_,pinNum)); \
    LL_EXTI_EnableEvent(DE_Concat(LL_EXTI_LINE_,pinNum)); \
    LL_EXTI_DisableRisingTrig(DE_Concat(LL_EXTI_LINE_,pinNum)); \
    LL_EXTI_EnableFallingTrig(DE_Concat(LL_EXTI_LINE_,pinNum)); \
}while(0)

#define DE_CfgPinRisingFallingWFE(portIndex,pinNum,speed,upDown) do{ \
    DE_CfgPinInput(DE_Concat(GPIO,portIndex), DE_Concat(LL_GPIO_PIN_,pinNum),speed,upDown); \
    LL_EXTI_SetEXTISource(DE_Concat(LL_EXTI_CONFIG_PORT,portIndex),DE_Concat(LL_EXTI_CONFIG_LINE,pinNum)); \
    LL_EXTI_DisableIT(DE_Concat(LL_EXTI_LINE_,pinNum)); \
    LL_EXTI_EnableEvent(DE_Concat(LL_EXTI_LINE_,pinNum)); \
    LL_EXTI_EnableRisingTrig(DE_Concat(LL_EXTI_LINE_,pinNum)); \
    LL_EXTI_EnableFallingTrig(DE_Concat(LL_EXTI_LINE_,pinNum)); \
}while(0)

#define DE_CfgPinDisWFE(portIndex,pinNum,speed,upDown) do{ \
    DE_CfgPinInput(DE_Concat(GPIO,portIndex), DE_Concat(LL_GPIO_PIN_,pinNum),speed,upDown); \
    /* LL_EXTI_SetEXTISource(DE_Concat(LL_EXTI_CONFIG_PORT,portIndex),DE_Concat(LL_EXTI_CONFIG_LINE,pinNum)); */ \
    LL_EXTI_DisableIT(DE_Concat(LL_EXTI_LINE_,pinNum)); \
    LL_EXTI_DisableEvent(DE_Concat(LL_EXTI_LINE_,pinNum)); \
    LL_EXTI_DisableRisingTrig(DE_Concat(LL_EXTI_LINE_,pinNum)); \
    LL_EXTI_DisableFallingTrig(DE_Concat(LL_EXTI_LINE_,pinNum)); \
}while(0)

/* SWD调试烧录接口 */
#define DE_SWDPort  GPIOA
#define DE_SWDPin   LL_GPIO_PIN_13/*默认复位后是,复用功能模式AF0，推挽输出，上拉,非常高速*/
#define DE_SWCPin   LL_GPIO_PIN_14/*默认复位后是,复用功能模式AF0，推挽输出，下拉,非常低速*/

#if SWD==1
    /* 启用SWD调试烧录接口 */
    #define DE_SWD_Pin_Init() do {/*SWC速度改成：非常高速*/ \
        LL_GPIO_SetPinSpeed(DE_SWDPort, DE_SWCPin, LL_GPIO_SPEED_FREQ_VERY_HIGH); \
    } while(0)
#elif SWD==2
    /* SWD调试烧录接口复用位UART */
    #define DE_SWD_Pin_Init() do {/* SWD调试烧录接口配置为推挽输出低电平，以此来禁用SWD调试烧录接口 */ \
        DE_CfgPinAlternate(DE_SWDPort,DE_SWDPin,LL_GPIO_SPEED_FREQ_VERY_HIGH,LL_GPIO_PULL_UP); \
        LL_GPIO_SetPinOutputType(DE_SWDPort,DE_SWDPin, LL_GPIO_OUTPUT_PUSHPULL); \
        DE_CfgPinAlternate(DE_SWDPort,DE_SWCPin,LL_GPIO_SPEED_FREQ_VERY_HIGH,LL_GPIO_PULL_UP); \
        LL_GPIO_SetPinOutputType(DE_SWDPort, DE_SWCPin, LL_GPIO_OUTPUT_PUSHPULL); \
        LL_GPIO_SetAFPin_8_15(DE_SWDPort,DE_SWDPin, LL_GPIO_AF9_USART1); \
        LL_GPIO_SetAFPin_8_15(DE_SWDPort,DE_SWCPin, LL_GPIO_AF9_USART1); \
    } while(0)
#else
    /* 禁用SWD调试烧录接口 */
    #define DE_SWD_Pin_Init() do {/* SWD调试烧录接口配置为推挽输出低电平，以此来禁用SWD调试烧录接口 */ \
        /* LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA); */ \
        /* LL_GPIO_SetOutputPin(DE_SWDPort, DE_SWDPin | DE_SWCPin); */ \
        LL_GPIO_ResetOutputPin(DE_SWDPort, DE_SWDPin | DE_SWCPin); \
        DE_CfgPinPPOut(DE_SWDPort,DE_SWDPin,LL_GPIO_SPEED_FREQ_VERY_HIGH,LL_GPIO_PULL_NO); \
        DE_CfgPinPPOut(DE_SWDPort,DE_SWCPin,LL_GPIO_SPEED_FREQ_VERY_HIGH,LL_GPIO_PULL_NO); \
    } while(0)
#endif

/* 电池供电输出控制引脚 */
#define DE_EnBatteryOutputPortIndex A
#define DE_EnBatteryOutputNum       9
#define DE_EnBatteryOutputPort      DE_Concat(GPIO,DE_EnBatteryOutputPortIndex)
#define DE_EnBatteryOutputPin       DE_Concat(LL_GPIO_PIN_,DE_EnBatteryOutputNum)

#define DE_BatteryOupPinInit() do{ \
    LL_GPIO_ResetOutputPin(DE_EnBatteryOutputPort, DE_EnBatteryOutputPin); \
    DE_CfgPinPPOut(DE_EnBatteryOutputPort,DE_EnBatteryOutputPin,LL_GPIO_SPEED_FREQ_LOW,LL_GPIO_PULL_NO); \
}while(0)

#define DE_BatteryOutOn() do{ \
    LL_GPIO_SetOutputPin(DE_EnBatteryOutputPort, DE_EnBatteryOutputPin); \
}while(0)

#define DE_BatteryOutOFF() do{ \
    LL_GPIO_ResetOutputPin(DE_EnBatteryOutputPort, DE_EnBatteryOutputPin); \
}while(0)

/* ScanKey工程测试按键 powerOnOffKey、valueAddKey、valueSubtractKey、modeKey按键端口信息定义 */
#define DE_AddKeyPortIndex        A
#define DE_AddKeyNum              10
#define DE_AddKeyPort             DE_Concat(GPIO,DE_AddKeyPortIndex)
#define DE_AddKeyPin              DE_Concat(LL_GPIO_PIN_,DE_AddKeyNum)

#define DE_ModeKeyPortIndex        A
#define DE_ModeKeyNum              11
#define DE_ModeKeyPort             DE_Concat(GPIO,DE_ModeKeyPortIndex)
#define DE_ModeKeyPin              DE_Concat(LL_GPIO_PIN_,DE_ModeKeyNum)

#define DE_SubtractKeyPortIndex   F
#define DE_SubtractKeyNum         5
#define DE_SubtractKeyPort        DE_Concat(GPIO,DE_SubtractKeyPortIndex)
#define DE_SubtractKeyPin         DE_Concat(LL_GPIO_PIN_,DE_SubtractKeyNum)

#define DE_PowerKeyPortIndex      B
#define DE_PowerKeyNum            4
#define DE_PowerKeyPort           DE_Concat(GPIO,DE_PowerKeyPortIndex)
#define DE_PowerKeyPin            DE_Concat(LL_GPIO_PIN_,DE_PowerKeyNum)

#define DE_ExtiCfgLinePowerOnOffKeyPin DE_Concat(LL_EXTI_CONFIG_LINE,DE_PowerKeyNum)
#define DE_ExtiLinePowerOnOffKeyPin    DE_Concat(LL_EXTI_LINE_,DE_PowerKeyNum)

void vDelayNms(uint32_t uiSystick);
/**
 * @brief   上电电路板初始化，主要初始化各种时钟和core systick定时器
 * @param   None
 * @return  None
 * @note    
 */
void vBoardPowerOnInit(void);

#ifdef __cplusplus
}
#endif

#endif

/***************** (C) COPYRIGHT axushilong@163.com *****END OF FILE***********/
