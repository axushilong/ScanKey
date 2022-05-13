#include "ScanKeyPort.h"
#include "gpio.h"
#include "KeyTask.h"








skwide_t kwSKPortReadKeyVal(struct ScanKeyType * pSK){/**< 返回激活的按键数据,按键按下表示激活,对应位置1 */
//	skwide_t skKeyData;
//	;
//	return skKeyData;
	return (pSK->ucSKActiveLevel == Gpio_GetInputIO(DE_KeyPort , DE_KeyPin)) ? 1 : 0 ;
}




