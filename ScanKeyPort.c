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
 * @param   [in] pSK 要读取的按键实例的数组，pSK[0]表示按键0，pSK[1]表示按键1，具体多少按键参考宏DE_SKCount
 * @return  skwide_t 返回激活的按键数据,按键按下表示激活,对应位置1，为0置1表示按键0当前状态是按下的，位1置1表示按键1当前状态是按下的
 * @note    
 */
skwide_t kwSKPortReadKeyVal(stScanKeyTypes * pstSK){/**< 读取各按键的当前状态,返回激活的按键数据,按键按下表示激活,对应位置1 */
    skwide_t skKeyData;
    if (GPIOB_ReadPortPin(GPIO_Pin_3)) {
        if (pstSK[0].ucSKActiveLevel) {
            skKeyData = 1;
        } else {
            skKeyData = 0;
        }
    } else {
        if (pstSK[0].ucSKActiveLevel) {
            skKeyData = 0;
        } else {
            skKeyData = 1;
        }
    }
//    if((pSK[0].ucSKActiveLevel) && (GPIOB_ReadPortPin(GPIO_Pin_3))){
//        skKeyData = 1;
//    }else{
//        skKeyData = 0;
//    }
    // if(pSK[1].ucSKActiveLevel == tuya_pin_read(DE_Key2)){
    //     skKeyData |= 2;
    // }else{}
	 return skKeyData;
//	return (pSK[0].ucSKActiveLevel == Gpio_GetInputIO(DE_KeyPort , DE_KeyPin)) ? 1 : 0 ;
}




