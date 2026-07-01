/**
 * @file    ScanKeyApp.h
 * @author  axushilong@163.com
 * @version v0.0.2
 * @date    2025.12.06
 * @brief   ScanKeyApp开机后的正常工作流程处理
 */

#ifndef __SCANKEYAPP_H__
#define __SCANKEYAPP_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>

#define DE_PowerOnKeydown               (0x88)
#define DE_PowerOnKeyLongDown           (0x84)
#define DE_PowerOnKeyLongDownUp         (0x83)
#define DE_PowerOnKeyDoubleDown         (0x82)
#define DE_PowerOnKeyContinueDown       (0x81)
#define DE_PowerOnKeyUp                 (0x80)

#define DE_ModeKeydown                  (0x48)
#define DE_ModeKeyLongDown              (0x44)
#define DE_ModeKeyDoubleDown            (0x42)

#define DE_AddKeydown              (0x28)
#define DE_AddKeyLongDown          (0x24)
#define DE_AddKeyDoubleDown        (0x22)
#define DE_AddKeyContinueDown      (0x21)

#define DE_SubtractKeydown         (0x18)
#define DE_SubtractKeyLongDown     (0x14)
#define DE_SubtractKeyDoubleDown   (0x12)
#define DE_SubtractKeyContinueDown (0x11)

typedef void (*pvFuncPtr_t)(void);
extern uint8_t ucPowerState;
extern const pvFuncPtr_t pvFuncPtr[3];


void vScanKeyProjectInit(void);
/**
  * @brief   开机后的正常工作流程
  * @param   None
  * @retval  None
  * @author  axushilong@163.com
  * @version v0.0.1
  * @date    2025.07.15
  */
void vPowerOnProcess(void);
void vPowerOffProcess(void){;}
void vChargeProcess(void){;}
#ifdef __cplusplus
}
#endif

#endif

/***************** (C) COPYRIGHT axushilong@163.com *****END OF FILE***********/
