/**
 * @file    ScanKey.h
 * @author  axushilong@163.com
 * @version v0.0.2
 * @date    2025.12.06
 * @brief   ScanKey按键扫描程序头文件
 */
#ifndef __SCANKEY_H__
#define __SCANKEY_H__

#ifdef __cplusplus
extern "C"{
#endif


#include <stdint.h>
#include "ScanKeyPort.h"



typedef void(** vfpSKCallBackTpye)(void*);/**< 定义扫描按键回调处理函数指针类型 */



typedef uint32_t (*puiIsKeyReleaseTypes)(void);//按键没有动作返回1，动作返回0
typedef void (*pvKeyCallBackTypes)(uint8_t ucKeyEvent);//按键动作回调函数，传入对应的动作事件

// typedef struct {
//     const uint8_t ucDebounceTick;
//     const uint8_t ucUpIntervalTick;
// }stSKDoubleDownArgTypes;
// typedef struct {
//     const uint8_t ucDebounceTick;
//     const uint8_t ucLongDownTick;
//     const uint8_t ucContinueTick;
// }stSKLongDownArgTypes;
typedef struct {
    const uint8_t ucDebounceTick;   /**< 按键消抖所经历的tick数 */
    const uint8_t ucUpIntervalTick; /**< 按键抬起所经历的tick数 */
    const uint8_t ucLongDownTick;   /**< 按键长按所经历的tick数 */
    const uint8_t ucContinueTick;   /**< 按键连按所经历的tick数 */
}stSKArgTypes;

// typedef struct {
//     uint8_t ucStatus;      /**< 扫描按键的状态机 */
//     uint8_t ucTick;        /**< 用于识别按键动作后所经历的tick数 */
//     uint8_t ucReleaseTick; /**< 用于识别按键释放后所经历的tick数 */
// }stSKCtrlDataTypes;

typedef struct {
    const puiIsKeyReleaseTypes puiIsKeyRelease; /**< 底层按键释放判断函数 */
    const pvKeyCallBackTypes   pvKeyCallBack;   /**< 应用层按键回调函数   */
    // const union unSKArgTypes {
    //     const stSKArgTypes stSKArg;
    //     stSKDoubleDownArgTypes stSKDoubleDownArg;
    //     stSKLongDownArgTypes stSKLongDownArg;
    // };
    // stSKCtrlDataTypes  stSKCtrlData;
    uint8_t ucStatus;      /**< 扫描按键的状态机 */
    uint8_t ucTick;        /**< 用于识别按键动作后所经历的tick数 */
    uint8_t ucReleaseTick; /**< 用于识别按键释放后所经历的tick数 */
}stSKTypes;




void vKeyLongProcess(stSKTypes * pstSK,uint8_t ucKeyNum,const stSKArgTypes stSKArg);




typedef struct {
    uint8_t ucStatus;  /**< 扫描按键的状态机 */
    uint8_t ucContinue;/**< continue(继续、连续)按键按下保持的信号 */
    uint8_t ucTriger;  /**< triger(触发)对应按键按下的信号,记录当前扫描是否有新的按键按下,有的话对应的位置1 */
    uint8_t ucReset;   /**< 记录由按下到抬起的触发 */
}stScanKeyCtrlDataTypes;


typedef struct {
    // enum ScanKeyEventType ucSKEvent       /*: 4*/;/**< 扫描按键的事件 */
    // enum ScanKeyStatusType ucSKStatus     /*: 4*/;/**< 扫描按键的状态机 */
    unsigned char ucSKEvent         /*: 5*/;/**< 扫描按键的事件 */
    unsigned char ucSKStatus        /*: 3*/;/**< 扫描按键的状态机 */
    unsigned char ucDebounceTick    /*: 7*/;/**< 按键消抖经历的滴答时间 */
    // unsigned char ucSKActiveLevel   /*: 1*/;/**< 激活电平 */

    unsigned short usDoubleStartTick;/**< 记录按键双击开始的滴答时刻和连击开始的滴答时刻 */

    unsigned short usSKScanCount;/**< 按键经历的滴答时间消抖验证通过后为非0值 */

    vfpSKCallBackTpye vfpSKCallBack;/**< 按键事件回调函数 */
}stScanKeyTypes;

/*

*/

/* ↓ 下面是需要配置的参数.注:本代码所有的滴答,指的是多长时间来扫描一次按键值的按键扫描周期时间 */
#define DE_SKCount (1)/**< 要扫描的按键数 */

/**< 按键数位宽类型,按键数在1~8之间可以是uint8_t;在9~16之间可以是uint16_t;在17~32之间可以是uint32_t */
typedef unsigned char  skwide_t;

#define DE_SKDebounceTick      (3)   /**< 按键消抖需要的循环滴答次数(30ms,100ms) */
#define DE_SKIntervalTick      (10)  /**< 判断双击动作,按键第一次抬起的最长滴答次数(200ms,400ms) */
#define De_SKLongStartTick     (150) /**< 按键长按开始所经历的滴答次数(2500ms,5000ms) */
#define De_SKContinueStartTick (233) /**< 按键长按触发连击时所需的连击间隔滴答次数(4000ms,8000ms) */
#define De_SKContinueTick      (16)  /**< 按键长按触发连击时所需的连击间隔滴答次数(250ms,500ms) */

/* 上面面是需要配置的参数 ↑ */

// typedef enum {/**< 定义扫描按键状态枚举类型 */
//     SKStatusStart = 0,    /**< 起始状态 */
//     SKStatusDown0,        /**< 按下状态 */
//     SKStatusUp0,          /**< 按下抬起状态 */
//     SKStatusDown1,        /**< 再次按下状态 */
//     SKStatusDownLong,     /**< 长按状态 */
//     SKStatusDownContinue, /**< 连按状态 */
// }eScanKeyStatusTypes;

#define DE_SKStatusStart        (0)
#define DE_SKStatusDown0        (1)
#define DE_SKStatusUp0          (2)
#define DE_SKStatusDown1        (3)
#define DE_SKStatusDownLong     (4)
#define DE_SKStatusDownContinue (5)

// enum ScanKeyEventType{/**< 定义扫描按键事件枚举类型 */
//     SKEventDown0 = 0,      /**< 按下事件 */
//     SKEventDown0Yes,       /**< 按下通过抖动事件 */
//     SKEventDown0No,        /**< 按下不通过抖动事件 */
//     SKEventUp0,            /**< 抬起事件 */
//     SKEventUp0Yes,         /**< 抬起通过抖动事件 */
//     SKEventUp0No,          /**< 抬起不通过抖动事件 */
//     SKEventDown1,          /**< 再次按下事件 */
//     SKEventDown1Yes,       /**< 再次按下通过抖动事件 */
//     SKEventDown1No,        /**< 再次按下不通过抖动事件 */
//     SKEventDoubleNo,       /**< 非双击事件 */
//     SKEventUp1,            /**< 再次抬起事件 */
//     SKEventUp1Yes,         /**< 再次抬起通过抖动事件 */
//     SKEventUp1No,          /**< 再次抬起不通过抖动事件 */
//     SKEventDownLongYes,    /**< 长按确认事件 */
//     SKEventUpL,            /**< 长按抬起事件 */
//     SKEventUpLYes,         /**< 长按抬起通过抖动事件 */
//     SKEventUpLNo,          /**< 长按抬起不通过抖动事件 */
//     SKEventDownContinueYes,/**< 连按确认开始事件 */
//     SKEventUpC,            /**< 连按抬起事件 */
//     SKEventUpCYes,         /**< 连按抬起通过抖动事件 */
//     SKEventUpCNo,          /**< 连按抬起不通过抖动事件 */
//     SKEventNone,           /**< 没有事件 */
// };

#define DE_SKEventDown0             (0)/**< 按下事件 */
#define DE_SKEventDown0Yes          (1)/**< 按下通过抖动事件 */
#define DE_SKEventDown0No           (2)/**< 按下不通过抖动事件 */
#define DE_SKEventUp0               (3)/**< 抬起事件 */
#define DE_SKEventUp0Yes            (4)/**< 抬起通过抖动事件 */
#define DE_SKEventUp0No             (5)/**< 抬起不通过抖动事件 */
#define DE_SKEventDown1             (6)/**< 再次按下事件 */
#define DE_SKEventDown1Yes          (7)/**< 再次按下通过抖动事件 */
#define DE_SKEventDown1No           (8)/**< 再次按下不通过抖动事件 */
#define DE_SKEventDoubleNo          (9)/**< 非双击事件 */
#define DE_SKEventUp1              (10)/**< 再次抬起事件 */
#define DE_SKEventUp1Yes           (11)/**< 再次抬起通过抖动事件 */
#define DE_SKEventUp1No            (12)/**< 再次抬起不通过抖动事件 */
#define DE_SKEventDownLongYes      (13)/**< 长按确认事件 */
#define DE_SKEventUpL              (14)/**< 长按抬起事件 */
#define DE_SKEventUpLYes           (15)/**< 长按抬起通过抖动事件 */
#define DE_SKEventUpLNo            (16)/**< 长按抬起不通过抖动事件 */
#define DE_SKEventDownContinueYes0 (17)/**< 连按确认开始事件 */
#define DE_SKEventDownContinueYes  (18)/**< 连按确认事件 */
#define DE_SKEventUpC              (19)/**< 连按抬起事件 */
#define DE_SKEventUpCYes           (20)/**< 连按抬起通过抖动事件 */
#define DE_SKEventUpCNo            (21)/**< 连按抬起不通过抖动事件 */
#define DE_SKEventNone             (22)/**< 异常事件，没有事件，回归到初始状态 */

typedef struct {
     skwide_t skwContinue;/**< continue(继续、连续)按键按下保持的信号 */
     skwide_t skwTriger;  /**< triger(触发)对应按键按下的信号,记录当前扫描是否有新的按键按下,有的话对应的位置1 */
     skwide_t skwReset;   /**< 记录由按下到抬起的触发 */
}stScanKeyCtrlDataTypes;

typedef skwide_t (* skwfpSKPortReadKeyValFType)(void);/**< 返回激活的按键数据,按键按下表示激活,对应位置1 */
typedef void(*vfpSKCallBackTpye)(void*);/**< 定义扫描按键回调处理函数指针类型 */

typedef struct {
    // enum ScanKeyEventType ucSKEvent       /*: 4*/;/**< 扫描按键的事件 */
    // enum ScanKeyStatusType ucSKStatus     /*: 4*/;/**< 扫描按键的状态机 */
    unsigned char ucSKEvent         /*: 5*/;/**< 扫描按键的事件 */
    unsigned char ucSKStatus        /*: 3*/;/**< 扫描按键的状态机 */
    unsigned char ucDebounceTick    /*: 7*/;/**< 按键消抖经历的滴答时间 */
    unsigned char ucSKActiveLevel   /*: 1*/;/**< 激活电平 */

    unsigned short usDoubleStartTick;/**< 记录按键双击开始的滴答时刻和连击开始的滴答时刻 */

    unsigned short usSKScanCount;/**< 按键经历的滴答时间消抖验证通过后为非0值 */

    vfpSKCallBackTpye vfpSKCallBack;/**< 按键事件回调函数 */
}stScanKeyTypes;

extern  skwide_t skwSKPortReadKeyVal(stScanKeyTypes * pstSK);/**< 返回激活的按键数据,按键按下表示激活,对应位置1 */

/**
  * @brief   
  * @param   [in,out] pstKeyCtrlData 扫描按键模块的状态控制,和按键数据
  * @param   [in,out] pstSK 按键数据和按键回调函数,为按键实例的数组指针
  * @retval  None
  * @author  axushilong@163.com
  * @version v0.0.2
  * @date    2025/12/06
  */
extern void vScanKey(stScanKeyCtrlDataTypes * pstKeyCtrlData, stScanKeyTypes * pstSK, unsigned char ucKeyCount);



#ifdef __cplusplus
}
#endif

#endif
