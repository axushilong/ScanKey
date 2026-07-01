/**
 * @file    ScanKeyPort.c
 * @author  axushilong@163.com
 * @version v0.0.1
 * @date    2022.05.16
 * @brief   ScanKey组件需要调用的更底层的接口，实现读取各按键的当前状态
 */
#include "ScanKeyPort.h"
//#include "CH58x_gpio.h"
#include "CH58x_common.h"






/**
 * @brief   读取各按键的当前状态,返回激活的按键数据,按键按下表示激活,对应位置1
 * @param   None
 * @return  skwide_t 返回激活的按键数据,按键按下表示激活,对应位置1，位0置1表示按键0当前状态是按下的，位1置1表示按键1当前状态是按下的
 * @note    
 */
skwide_t skwSKPortReadKeyVal(void){/**< 读取各按键的当前状态,返回激活的按键数据,按键按下表示激活,对应位置1 */
    skwide_t skKeyData = 0;
    if (GPIOB_ReadPortPin(GPIO_Pin_3)) {/* 按键按下 */
        skKeyData |= 1;
    } else {
        skKeyData &= ~1;
    }
    return skKeyData;
}




