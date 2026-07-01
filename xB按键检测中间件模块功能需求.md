设计一份按键检测中间件模块，需求如下：

1.默认所有单个按键要支持单击、长按、Hold和多击功能。
2.支持组合按键，所有按键编排到一内存空间中的对应的一个位，比如有8个按键就编排到一个字节中，字节中的位0到7分别对应按键0到7。
3.支持按键去抖动，去抖动时间可配置。
4.支持按键长按时间可配置。
5.支持按键Hold时间可配置。
6.支持按键双击时间间隔可配置。
7.支持按键单击、长按、Hold和多击事件回调函数，统一回调函数处理，带参数分别为键值、事件和多击次数。

头文件定义可参考如下：

```c
/**
  * @file    xB.h
  * @author  axushilong@163.com
  * @version v0.0.1
  * @date    2026.06.29
  * @brief   xB 按键模块，所有按键默认支持单击、长按、Hold和多击，只实现默认功能。
  */
#ifndef __XB_H__
#define __XB_H__



#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <stdbool.h>

#define DE_NoPress (0)

#define DE_OnlyClickForComb

typedef uint16_t uxWide_t;/*按键编排到的内存，根据实际需求决定是8位、16位还是32位*/

#define DE_XBEnentIdel   (0)
#define DE_XBEventClick  (1)
#define DE_XBEventLong   (2)
#define DE_XBEventHold   (3)
#define DE_XBEventUp     (4)
#define DE_XBEventError  (0xff)

typedef uxWide_t (*puxFuncReadTypes)(void);
typedef void (*pvFuncIdleHandlerTypes)(uxWide_t uxXBVal); // 按键空闲处理回调，参数 uxXBVal 表示从哪个按键进入空闲的
typedef void (*pvFuncEventHandlerTypes)(uxWide_t uxXBVal, uint8_t ucEvent, uint8_t ucClickCnt);


typedef struct {
    uint8_t ucFilterTime;        // 消抖阈值   (默认3,  30ms@10ms)
    uint8_t usLongTime;          // 长按阈值   (默认75, 750ms)
    uint8_t ucHoldTime;          // Hold间隔   (默认15, 150ms)
    uint8_t ucClickDelayTime;    // 多击延迟   (默认20, 200ms)
    uint8_t ucClickCntMax;       // 最多支持多少连击 (默认8)

    puxFuncReadTypes puxReadVal;    // 读原始按键值回调 (必须)
    pvFuncIdleHandlerTypes pvIdleHandler;   // 按键空闲处理回调   (可选)
    pvFuncEventHandlerTypes pvEventHandler; // 定时器注册回调   (必须)
} stXBCfgTypes;

typedef struct {
    const stXBCfgTypes *pstXBCfg; // 配置 (只读引用)

    uint8_t ucFilterCnt;         // 消抖计数器
    uint8_t ucReleasCnt;         // 消抖后释放抬起计数，释放抬起后延迟计数器，多击、空闲判定
    uint16_t usPressCnt;         // 消抖后按下计数，按下持续计数器
    uint8_t ucClickCnt;          // 连击次数 (1~8)

    uxWide_t uxFilterValue;      // 消抖中稳定的按键值
    uxWide_t uxLastValue;        // 上次确认的按键值
    uxWide_t uxNotifyValue;      // 多击时，待上报的按键值
} stXBTypes;

void vXB(stXBTypes * pstXB);

/*
使用示例：
1.创建静态按键实例并初始化
2.配置定时扫描xB程序
3.应用事件处理

static stXBCfgTypes stXBCfg = {
    .ucFilterTime = 3,
    .usLongTime = 75,
    .ucHoldTime = 15,
    .ucClickDelayTime = 20,
    .ucClickCntMax = 8,

    .puxReadVal = puxReadVal,
    .pvIdleHandler = pvIdleHandler,
    .pvEventHandler = pvEventHandler
};

stXBTypes stXB = {
    .pstXBCfg = &stXBCfg,
    .ucFilterCnt = 0,
    .ucReleasCnt = 0,
    .usPressCnt = 0,
    .ucClickCnt = 0,

    .uxFilterValue = 0,
    .uxLastValue = 0,
    .uxNotifyValue = 0
};

// 周期扫描xB程序
vXB(&stXB);

*/

*/

#ifdef __cplusplus
}
#endif

#endif

/*** (C) COPYRIGHT solonggroup.com ** axushilong@ 1 6 3 . com ***END OF FILE */

```

按键事件处理回调函数类型可参考如下，主要是考虑到统一按键事件处理：
typedef void (*pvFuncEventHandlerTypes)(uxWide_t uxXBVal, uint8_t ucEvent, uint8_t ucClickCnt);

参数说明：
uxXBVal：按键值，表示当前按键状态，按键0到7分别对应位0到7，按下对应的位为1，释放对应的位为0。
ucEvent：按键事件，只有ucClickCnt==1时，多击时，次参数无参考意义，取值范围如下：
- DE_XBEnentIdel：按键空闲
- DE_XBEventClick：单击
- DE_XBEventLong：长按
- DE_XBEventHold：Hold
- DE_XBEventUp：按键抬起
- DE_XBEventError：错误事件
ucClickCnt：多击次数，取值范围1到8，表示当前按键连续按下的次数。

按键的识别扫描程序，如下提供了部分思路框架：

```c
/**
  * @file    xB.c
  * @brief   通用按键扫描中间件 - 零硬件依赖实现
  * @author  axushilong@163.com
  * @version v0.0.1
  * @date    2026.06.29
  *
  * 核心设计:
  *   - pfnReadRaw读原始值 → 消抖 → 事件检测
  *   - 按键值 = 位掩码: 单键=单bit, 组合=多bit, 天然兼容
  *   - 所有硬件操作通过回调, 本文件不#include任何平台头文件
  */

#include "xB.h"



void vXB(stXBTypes * pstXB){
    uint32_t uiEvent;
    stXBCfgTypes * pstXBCfg = &pstXB->pstXBCfg;
    uxWide_t uxCurVal = pstXBCfg->puxReadVal; // 读取原始值
    /* 1.消抖 */
    if(uxCurVal != pstXB->uxFilterValue){
        pstXB->ucFilterCnt = 0;
        pstXB->uxFilterValue = uxCurVal;
        return;
    }
    if(pstXB->ucFilterCnt < pstXBCfg->ucFilterTime){
        pstXB->ucFilterCnt++;
        return;
    }
    /* 2.事件检测 */
    if(uxCurVal != pstXB->uxLastValue){ // 值变化
        if/*松开*/(uxCurVal == DE_NoPress){ // 变成无按键按下，即松开
            pstXB->ucReleasCnt = 1;
            /* 待补充的代码
            if(){ //如果*,则确认是*事件
                //对应处理

                uiEvent = ; // 更新事件为
                goto _LabelXBEventNotify;// 事件回调处理
            }
            */

        }else/*按下*/{ //变成其他键按下{1.从松开变到按下；2.从一个按键按下变到另一个按键按下}
            pstXB->usPressCnt = 1;
            pstXB->ucClickCnt++;
            /* 待补充的代码
            if(){ //如果*,则确认是*事件
                //对应处理

                uiEvent = ; // 更新事件为
                goto _LabelXBEventNotify;// 事件回调处理
            }
            goto _LabelXBEnd;
            */
        }
    }
    //保持松开uxCurVal == pstXB->uxLastValue == DE_NoPress
    if(uxCurVal == DE_NoPress){
        /* 待补充的代码
        pstXB->ucReleasCnt++;
        if(){ //如果*,则确认是*事件
            //对应处理

            uiEvent = ; // 更新事件为
            goto _LabelXBEventNotify;// 事件回调处理
        }
        goto _LabelXBEnd;
        */
    }
    //保持按下uxCurVal == pstXB->uxLastValue
    /* 待补充的代码
    pstXB->usPressCnt++;
    if(){ //如果*,则确认是*事件
        //对应处理

        uiEvent = ; // 更新事件为
        goto _LabelXBEventNotify;// 事件回调处理
    }
    goto _LabelXBEnd;
    */
_LabelXBEventNotify:
    //事件回调前处理
    pstXB->pstXBCfg->pvEventHandler(, uiEvent, ); // 事件回调处理
    //事件回调后处理
_LabelXBEnd:
    pstXB->uxLastValue = uxCurVal;
}



```
