/********************************** (C) COPYRIGHT *******************************
 * File Name          : AppKey.h
 * Author             : axushilong@163.com
 * Version            : V0.1
 * Date               : 2022/09/
 * Description
 * Copyright (c) 2022 axushilong
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __APPKEY_H__
#define __APPKEY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "CH58xBLE_LIB.h"
//#include "CH58xBLE_ROM.h"
#include "CH58x_common.h"

#define DE_KeyPeriodicEvent (1<<0)

extern tmosTaskID ucKeyAppTaskID;

/**
 * @fn      vKeyAppInit
 *
 * @brief   显示部分初始化，主要是初始LCD驱动芯片TM1621C的相关引脚
 *
 * @param   NONE
 *
 * @return  NONE
 */
void vKeyAppInit(void);

/**
 * @fn      usEnergyAppProcessEvent
 *
 * @brief   按键应用任务事件处理
 *
 * @param   ucTaskID - The TMOS assigned task ID.
 * @param   usEvents  - events to process.  This is a bit map and can
 *                      contain more than one event.
 *
 * @return  events.
 */
tmosEvents usKeyAppProcessEvent(tmosTaskID ucTaskID, tmosEvents usEvents);

#ifdef __cplusplus
}
#endif

#endif // __BASE64_H__
