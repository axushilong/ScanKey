#ifndef __SCANKEYPORT_H__
#define __SCANKEYPORT_H__

#include <stdint.h>
/*

*/
/*↓下面是需要配置的参数.注:本代码所有的滴答,指的是多长时间来扫描一次按键值的按键扫描周期时间*/
#define DE_SKCount (1)/**< 要扫描的按键数 */
/**< 按键数位宽类型,按键数在1~8之间可以是uint8_t;在9~16之间可以是uint16_t;在17~32之间可以是uint32_t */
typedef uint8_t skwide_t;

#define DE_SKDebounceTick      (5)  /**< 按键消抖需要的循环滴答时间 */
#define DE_SKIntetvalTick      (20) /**< 判断双击动作,按键第一次抬起的最长滴答时间 */
#define De_SKLongStartTick     (250)/**< 按键长按开始所经历的滴答时间 */
#define De_SKContinueStartTick (400) /**< 按键长按触发连击时所需的连击间隔滴答时间 */
#define De_SKContinueTick      (25) /**< 按键长按触发连击时所需的连击间隔滴答时间 */
/*上面面是需要配置的参数↑*/

typedef void(*SKCallBackTpye)(void*);/**< 定义扫描按键回调处理函数指针类型 */

//enum ScanKeyStatusType{/**< 定义扫描按键状态枚举类型 */
//	SKStatusStart = 0,    /**< 起始状态 */
//	SKStatusDown0,        /**< 按下状态 */
//	SKStatusUp0,          /**< 按下抬起状态 */
//	SKStatusDown1,        /**< 再次按下状态 */
//	SKStatusDownLong,     /**< 长按状态 */
//	SKStatusDownContinue, /**< 连按状态 */
//};

#define DE_SKStatusStart        (0)
#define DE_SKStatusDown0        (1)
#define DE_SKStatusUp0          (2)
#define DE_SKStatusDown1        (3)
#define DE_SKStatusDownLong     (4)
#define DE_SKStatusDownContinue (5)

//enum ScanKeyEventType{/**< 定义扫描按键事件枚举类型 */
//	SKEventDown0 = 0,      /**< 按下事件 */
//	SKEventDown0Yes,       /**< 按下通过抖动事件 */
//	SKEventDown0No,        /**< 按下不通过抖动事件 */
//	SKEventUp0,            /**< 抬起事件 */
//	SKEventUp0Yes,         /**< 抬起通过抖动事件 */
//	SKEventUp0No,          /**< 抬起不通过抖动事件 */
//	SKEventDown1,          /**< 再次按下事件 */
//	SKEventDown1Yes,       /**< 再次按下通过抖动事件 */
//	SKEventDown1No,        /**< 再次按下不通过抖动事件 */
//	SKEventDoubleNo,       /**< 非双击事件 */
//	SKEventUp1,            /**< 再次抬起事件 */
//	SKEventUp1Yes,         /**< 再次抬起通过抖动事件 */
//	SKEventUp1No,          /**< 再次抬起不通过抖动事件 */
//	SKEventDownLongYes,    /**< 长按确认事件 */
//	SKEventUpL,            /**< 长按抬起事件 */
//	SKEventUpLYes,         /**< 长按抬起通过抖动事件 */
//	SKEventUpLNo,          /**< 长按抬起不通过抖动事件 */
//	SKEventDownContinueYes,/**< 连按确认开始事件 */
//	SKEventUpC,            /**< 连按抬起事件 */
//	SKEventUpCYes,         /**< 连按抬起通过抖动事件 */
//	SKEventUpCNo,          /**< 连按抬起不通过抖动事件 */
//	SKEventNone,           /**< 没有事件 */
//};

#define DE_SKEventDown0            (0)/**< 按下事件 */
#define DE_SKEventDown0Yes         (1)/**< 按下通过抖动事件 */
#define DE_SKEventDown0No          (2)/**< 按下不通过抖动事件 */
#define DE_SKEventUp0              (3)/**< 抬起事件 */
#define DE_SKEventUp0Yes           (4)/**< 抬起通过抖动事件 */
#define DE_SKEventUp0No            (5)/**< 抬起不通过抖动事件 */
#define DE_SKEventDown1            (6)/**< 再次按下事件 */
#define DE_SKEventDown1Yes         (7)/**< 再次按下通过抖动事件 */
#define DE_SKEventDown1No          (8)/**< 再次按下不通过抖动事件 */
#define DE_SKEventDoubleNo         (9)/**< 非双击事件 */
#define DE_SKEventUp1             (10)/**< 再次抬起事件 */
#define DE_SKEventUp1Yes          (11)/**< 再次抬起通过抖动事件 */
#define DE_SKEventUp1No           (12)/**< 再次抬起不通过抖动事件 */
#define DE_SKEventDownLongYes     (13)/**< 长按确认事件 */
#define DE_SKEventUpL             (14)/**< 长按抬起事件 */
#define DE_SKEventUpLYes          (15)/**< 长按抬起通过抖动事件 */
#define DE_SKEventUpLNo           (16)/**< 长按抬起不通过抖动事件 */
#define DE_SKEventDownContinueYes (17)/**< 连按确认开始事件 */
#define DE_SKEventUpC             (18)/**< 连按抬起事件 */
#define DE_SKEventUpCYes          (19)/**< 连按抬起通过抖动事件 */
#define DE_SKEventUpCNo           (20)/**< 连按抬起不通过抖动事件 */
#define DE_SKEventNone            (21)/**< 没有事件 */

struct ScanKeyType{
//	enum ScanKeyEventType ucSKEvent       /*: 4*/;/**< 扫描按键的事件 */
//	enum ScanKeyStatusType ucSKStatus     /*: 4*/;/**< 扫描按键的状态机 */
	uint8_t ucSKEvent         /*: 5*/;/**< 扫描按键的事件 */
	uint8_t ucSKStatus        /*: 3*/;/**< 扫描按键的状态机 */
	uint8_t ucDebounceTick    /*: 7*/;/**< 按键消抖经历的滴答时间 */
	uint8_t ucSKActiveLevel /*: 1*/;/**< 激活电平 */

	uint16_t usDoubleStartTick;/**< 记录按键双击开始的滴答时刻和连击开始的滴答时刻 */

	uint16_t usSKScanCount;/**< 按键经历的滴答时间消抖验证通过后为非0值 */

	SKCallBackTpye pfSKCallBack;/**< 按键事件回调函数 */
};

struct ScanKeyCtrlDataType{
	skwide_t kwContinue;/**< continue(继续、连续)按键按下保持的信号 */
	skwide_t kwTriger;  /**< triger(触发)对应按键按下的信号,记录当前扫描是否有新的按键按下,有的话对应的位置1 */
	skwide_t kwReset;   /**< 记录由按下到抬起的触发 */
};

#ifdef __cplusplus
extern "C"{
#endif

skwide_t kwSKPortReadKeyVal(struct ScanKeyType * pSK);/**< 返回激活的按键数据,按键按下表示激活,对应位置1 */

/**
  * @brief   
  * @param   [in,out] pstKeyCtrlData 扫描按键模块的状态控制,和按键数据
  * @param   [in,out] pSK 按键数据和按键回调函数
  * @retval  None
  * @author  axushilong@163.com
  * @version V1.0.0
  * @date    2021/02/08
  */
void vScanKey(struct ScanKeyCtrlDataType * pstKeyCtrlData, struct ScanKeyType * pSK);

#ifdef __cplusplus
}
#endif

#endif
