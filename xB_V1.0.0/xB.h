/**
  * @file    xB.h
  * @author  axushilong@163.com
  * @version v1.0.0
  * @date    2026.07.01
  * @brief   xB 按键模块，所有按键默认支持单击、长按、Hold和多击,组合按键开启宏DE_OnlyClickForComb，则所有组合按键只支持单击。
  *          非组合单个按键有只支持单击功能和编码多击功能可以配置。
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

typedef uint16_t uxWide_t;

// enum {
//     eXBEnentIdel   = 0;
//     eXBEventClick  = 1,/*  */
//     eXBEventLong   = 2,
//     eXBEventHold   = 3,
//     eXBEventUp     = 4,
//     eXBEventClick2 = 5,
//     eXBEventClick3 = 6,
//     eXBEventClick4 = 7,
//     eXBEventClick5 = 8,
//     eXBEventClick6 = 9,
//     eXBEventClick7 = 10,
//     eXBEventClick8 = 11,
//     eXBEventError  = 0xff
// };

#define DE_XBEnentIdel   (0)
#define DE_XBEventClick  (1)
#define DE_XBEventLong   (2)
#define DE_XBEventHold   (3)
#define DE_XBEventUp     (4)
#define DE_XBEventClick2 (5)
#define DE_XBEventClick3 (6)
#define DE_XBEventClick4 (7)
#define DE_XBEventClick5 (8)
#define DE_XBEventClick6 (9)
#define DE_XBEventClick7 (10)
#define DE_XBEventClick8 (11)
#define DE_XBEventError  (0xff)

typedef uxWide_t (*puxFuncReadTypes)(void);
typedef void (*pvFuncIdleHandlerTypes)(uxWide_t uxXBVal); // 按键空闲处理回调，参数 uxXBVal 表示从哪个按键进入空闲的
typedef void (*pvFuncEventHandlerTypes)(uxWide_t uxXBVal, uint8_t ucEvent, uint8_t ucClickCnt);


typedef struct {
    uint8_t ucIoKeyNum;          // 物理按键数 (能力表大小)
    uint8_t ucFilterTime;        // 消抖阈值   (默认3,  30ms@10ms)
    uint8_t usLongTime;          // 长按阈值   (默认75, 750ms)
    uint8_t ucHoldTime;          // Hold间隔   (默认15, 150ms)
    uint8_t ucClickDelayTime;    // 多击延迟   (默认20, 200ms)
    uint8_t ucClickCntMax;       // 最多支持多少连击

    uxWide_t uxCapMultiClickEncode; // 多击编码
    uxWide_t uxCapOnlyClick;        // 配置按键只支持单击，只支持单击，肯定就不支持多击

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
    uint8_t ucMultiClickEncode;  // 连击记录编码
    uxWide_t uxFilterValue;      // 消抖中稳定的按键值
    uxWide_t uxLastValue;        // 上次确认的按键值
    uxWide_t uxNotifyValue;      // 多击时，待上报的按键值

    // fnKeyEvtHandler pfnHandler;  // 事件回调
} stXBTypes;

void vXB(stXBTypes * pstXB);

/*
使用示例：
1.创建静态按键实例并初始化
2.配置定时扫描xB程序
3.应用事件处理

stXBTypes stXB = {
    .stXBCfg = NULL,

};

*/

#ifdef __cplusplus
}
#endif

#endif

/*** (C) COPYRIGHT solonggroup.com ** axushilong@ 1 6 3 . com ***END OF FILE */
