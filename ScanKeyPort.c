#include "ScanKeyPort.h"
#include "gpio.h"
#include "KeyTask.h"








skwide_t kwSKPortReadKeyVal(struct ScanKeyType * pSK){/**< ���ؼ���İ�������,�������±�ʾ����,��Ӧλ��1 */
//	skwide_t skKeyData;
//	;
//	return skKeyData;
	return (pSK->ucSKActiveLevel == Gpio_GetInputIO(DE_KeyPort , DE_KeyPin)) ? 1 : 0 ;
}




