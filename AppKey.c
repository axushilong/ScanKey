/********************************** (C) COPYRIGHT *******************************
 * File Name          : AppKey.c
 * Author             : axushilong@163.com
 * Version            : V0.1
 * Date               : 2022/09/
 * Description
 * Copyright (c) 2022 axushilong
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
#include "AppKey.h"
#include "ScanKeyPort.h"
#include "WifiSmartPowerMeter.h"


tmosTaskID ucKeyAppTaskID;

#define DE_ThisAppTaskID ucKeyAppTaskID

#define DE_KeyPeriodicEventNms (30) /* 160/1.6 = 100ms */

void vAppKey1CallBack(void * pData);

static stScanKeyCtrlDataTypes astKeyCtrlData = {0};/**< 用于按键识别的数据 */
static stScanKeyTypes astSK[DE_SKCount] = {/**< 按键实例数据 */
    {
        0,
        DE_SKStatusStart,
        0,
        0,
        0,
        0,
        vAppKey1CallBack
    }
};

void vAppKey1CallBack(void * pData){
    unsigned char ucKeyEvent;
    stScanKeyTypes * pstSk1 = (stScanKeyTypes *)pData;
    uint8_t * pucEnergyMeterMsgKeyDown;
    stMeterMsgTypes * pstMeterMsg;
    PRINT("AppKey pstSk1->ucSKEvent = %d\r\n",pstSk1->ucSKEvent);
    switch (pstSk1->ucSKEvent){
    case DE_SKEventDown0:{/**< 0按下事件 */

    }
        break;

    case DE_SKEventDown0Yes:{/**< 1通按下过抖动事件 */
//        ucKeyEvent = DE_Key1Click;
//        iResult = tuya_hal_queue_post(pvCallKeyEventQueueEvent, &ucKeyEvent, 100);
//        if(iResult != OPRT_OK){
//            PR_ERR("Cat1Helmet AppKey iResult = %d == tuya_hal_queue_post(pvCallKeyEventQueueEvent,%d)",iResult,ucKeyEvent);
//        }else{
//            PR_NOTICE("Cat1Helmet AppKey Succeed tuya_hal_queue_post(pvCallKeyEventQueueEvent,DE_Key1Click)");
//        }
    }
        break;

    case DE_SKEventDown0No:{/**< 2按下不通过抖动事件 */

    }
        break;

    case DE_SKEventUp0:{/**< 3抬起事件 */

    }
        break;

    case DE_SKEventUp0Yes:{/**< 4抬起通过抖动事件 */
//        pucEnergyMeterMsgKeyDown = tmos_msg_allocate(sizeof(stMeterMsgTypes));
//        if(pucEnergyMeterMsgKeyDown){
//            pstMeterMsg = (stMeterMsgTypes * )pucEnergyMeterMsgKeyDown;
        pstMeterMsg = (stMeterMsgTypes * )tmos_msg_allocate(sizeof(stMeterMsgTypes));
        if(pstMeterMsg){
            pstMeterMsg->stTmosEventHdr.event = DE_MeterKeyMsgId;
            pstMeterMsg->ucEventType = DE_KeyDown;
        }
        tmos_msg_send(ucDisplayAppTaskID, (uint8_t *)pstMeterMsg);
        PRINT("AppKey DE_SKEventUp0Yes\r\n");
    }
        break;

    case DE_SKEventUp0No:{/**< 5抬起不通过抖动事件 */

    }
        break;

    case DE_SKEventDown1:{/**< 6再次按下事件 */

    }
        break;

    case DE_SKEventDown1Yes:{/**< 7再次按下通过抖动事件 */

    }
        break;

    case DE_SKEventDown1No:{/**< 8再次按下不通过抖动事件 */

    }
        break;

    case DE_SKEventDoubleNo:{/**< 9非双击事件 */

    }
        break;

    case DE_SKEventUp1:{/**< 10再次抬起事件 */

    }
        break;

    case DE_SKEventUp1Yes:{/**< 11再次抬起通过抖动事件 */
        pstMeterMsg = (stMeterMsgTypes * )tmos_msg_allocate(sizeof(stMeterMsgTypes));
        if(pstMeterMsg){
            pstMeterMsg->stTmosEventHdr.event = DE_MeterKeyMsgId;
            pstMeterMsg->ucEventType = DE_KeyDown;
        }
        tmos_msg_send(ucDisplayAppTaskID, (uint8_t *)pstMeterMsg);

        PRINT("AppKey DE_SKEventUp1Yes\r\n");
    }
        break;

    case DE_SKEventUp1No:{/**< 12再次抬起不通过抖动事件 */

    }
        break;

    case DE_SKEventDownLongYes:{/**< 13长按确认事件 */
        pstMeterMsg = (stMeterMsgTypes * )tmos_msg_allocate(sizeof(stMeterMsgTypes));
        if(pstMeterMsg){
            pstMeterMsg->stTmosEventHdr.event = DE_MeterKeyMsgId;
            pstMeterMsg->ucEventType = DE_KeyDownLong1;
        }
        tmos_msg_send(ucEnergyAppTaskID, (uint8_t *)pstMeterMsg);//透支

        PRINT("AppKey DE_SKEventDownLongYes\r\n");
    }
        break;

    case DE_SKEventUpL:{/**< 14长按抬起事件 */

    }
        break;

    case DE_SKEventUpLYes:{/**< 15长按抬起通过抖动事件 */

    }
        break;

    case DE_SKEventUpLNo:{/**< 16长按抬起不通过抖动事件 */

    }
        break;

    case DE_SKEventDownContinueYes0:{/**< 17连按确认开始事件 */
        pstMeterMsg = (stMeterMsgTypes * )tmos_msg_allocate(sizeof(stMeterMsgTypes));
        if(pstMeterMsg){
            pstMeterMsg->stTmosEventHdr.event = DE_MeterKeyMsgId;
            pstMeterMsg->ucEventType = DE_KeyDownLong2;
        }
        tmos_msg_send(ucDisplayAppTaskID, (uint8_t *)pstMeterMsg);//配网

        pstMeterMsg = (stMeterMsgTypes * )tmos_msg_allocate(sizeof(stMeterMsgTypes));
        if(pstMeterMsg){
            pstMeterMsg->stTmosEventHdr.event = DE_MeterKeyMsgId;
            pstMeterMsg->ucEventType = DE_KeyDownLong2;
        }
        tmos_msg_send(ucCommunicationAppTaskID, (uint8_t *)pstMeterMsg);//配网

        PRINT("AppKey DE_SKEventDownContinueYes0\r\n");
    }
        break;

    case DE_SKEventDownContinueYes:{/**< 18连按确认事件 */

        PRINT("AppKey DE_SKEventDownContinueYes\r\n");
    }
        break;

    case DE_SKEventUpC:{/**< 19连按抬起事件 */

    }
        break;

    case DE_SKEventUpCYes:{/**< 20连按抬起通过抖动事件 */

    }
        break;

    case DE_SKEventUpCNo:{/**< 21连按抬起不通过抖动事件 */

    }
        break;

    default:{

    }
        break;
    }
}

/*********************************************************************
 * @brief   按键应用的TMOS消息事件处理。获取其他任务ID的消息数据。
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void KeyAppProcessTMOSMsg(unsigned char *pMsg) {
    ;
}

/*********************************************************************
 * @brief   按键应用周期事件处理，30ms扫描一次按键值，然后根据对应值处理。
 *
 * @param   NONE
 *
 * @return  NONE
 */
void vKeyAppPeriodicProcess(void){
    vScanKey(&astKeyCtrlData,&astSK[0]);/**< 按键扫描，间隔10ms */
}

/*********************************************************************
 * @brief   按键应用任务事件处理
 *
 * @param   ucTaskID - The TMOS assigned task ID.
 * @param   usEvents  - events to process.  This is a bit map and can
 *                      contain more than one event.
 *
 * @return  events.
 */
tmosEvents usKeyAppProcessEvent(tmosTaskID ucTaskID, tmosEvents usEvents){
    if(usEvents & SYS_EVENT_MSG){//系统消息处理
        unsigned char *pucMsg;
        if((pucMsg = tmos_msg_receive(DE_ThisAppTaskID)) != NULL){
//            KeyAppProcessTMOSMsg(pucMsg);//获取其他模块的数据
            // Release the TMOS message
            tmos_msg_deallocate(pucMsg);
        }
        // return unprocessed events
        return (usEvents ^ SYS_EVENT_MSG);
    }

    if(usEvents & DE_KeyPeriodicEvent){//周期行任务处理
        // Restart timer
//        if(DE_KeyPeriodicEvent100ms)
//        {
            tmos_start_task(DE_ThisAppTaskID, DE_KeyPeriodicEvent, MS1_TO_SYSTEM_TIME(DE_KeyPeriodicEventNms)); //每100ms,读一次电能数据到-共享-内存区，并将读到的电能数据通过消息事件发送到存储模块的电能内存区、显示模块的电能内存区和联网通讯模块的电能内存区。
//        }
        // Perform periodic application task
            vKeyAppPeriodicProcess();//每500ms根据轮显模式，显示对应的界面
        return (usEvents ^ DE_KeyPeriodicEvent);
    }

    // Discard unknown events
    return 0;
}

/*********************************************************************
 * @brief   按键部分初始化，主要是初始化按键的相关引脚
 *
 * @param   NONE
 *
 * @return  NONE
 */
void vKeyAppInit(void){
    //PB3上拉输入
    GPIOB_SetBits(GPIO_Pin_3);
    GPIOB_ModeCfg(GPIO_Pin_3, GPIO_ModeIN_PU);
//注册按键应用模块任务事件和打印任务ID
    DE_ThisAppTaskID = TMOS_ProcessEventRegister(usKeyAppProcessEvent);
    PRINT("ucKeyAppTaskID = %d\r\n",DE_ThisAppTaskID);
    tmos_start_task(DE_ThisAppTaskID, DE_KeyPeriodicEvent, MS1_TO_SYSTEM_TIME(DE_KeyPeriodicEventNms));//周期执行按键扫描程序
}

/*

*/
