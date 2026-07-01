/**
 * @file    ScanKeyPort.h
 * @author  axushilong@163.com
 * @version v0.0.2
 * @date    2025.12.06
 * @brief   ScanKey移植接口文件
 */
#ifndef __SCANKEYPORT_H__
#define __SCANKEYPORT_H__

#ifdef __cplusplus
extern "C"{
#endif


#include <stdint.h>

/* ↓ 下面是需要配置的参数.注:本代码所有的滴答,指的是多长时间来扫描一次按键值的按键扫描周期时间 */
#define DE_SKCount (1)/**< 要扫描的按键数 */

#define DE_SKDebounceTick      (3)   /**< 按键消抖需要的循环滴答次数(30ms,100ms) */
#define DE_SKIntervalTick      (10)  /**< 判断双击动作,按键第一次抬起的最长滴答次数(200ms,400ms) */
#define DE_SKLongStartTick     (150) /**< 按键长按开始所经历的滴答次数(2500ms,5000ms) */
#define DE_SKContinueStartTick (233) /**< 按键长按触发连击时所需的连击间隔滴答次数(4000ms,8000ms) */
#define DE_SKContinueTick      (16)  /**< 按键长按触发连击时所需的连击间隔滴答次数(250ms,500ms) */

/* 上面面是需要配置的参数 ↑ */

/**< 按键数位宽类型,按键数在1~8之间可以是uint8_t;在9~16之间可以是uint16_t;在17~32之间可以是uint32_t */
typedef uint8_t skwide_t;

/**
 * @brief   读取各按键的当前状态,返回激活的按键数据,按键按下表示激活,对应位置1
 * @param   None
 * @return  skwide_t 返回激活的按键数据,按键按下表示激活,对应位置1，位0置1表示按键0当前状态是按下的，位1置1表示按键1当前状态是按下的
 * @note    
 */
extern skwide_t skwSKPortReadKeyVal(void);/**< 返回激活的按键数据,按键按下表示激活,对应位置1 */

#ifdef __cplusplus
}
#endif

#endif
