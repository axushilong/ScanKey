/**
 * @file    ScanKeyApp.c
 * @author  axushilong@163.com
 * @version v0.0.2
 * @date    2025.12.06
 * @brief   ScanKeyApp开机后的正常工作流程处理
 */
#include "ScanKeyApp.h"
#include "board.h"
#include <string.h>
#include "TimerWDog.h"
#include "ScanKey.h"



uint8_t ucPowerState;
const pvFuncPtr_t pvFuncPtr[3] = {
    vPowerOffProcess,
    vPowerOnProcess,
    vChargeProcess
};


unsigned char ucKeyEventArray[4] = {0};
stRbType stRbForKey = {0};

const stSKArgTypes stSKArg = {2,25,150,10};/**< 按键参数结构体：消抖时间，双击抬起时间，长按时间，连击时间 */
void vAddKeyCallBack(uint8_t ucKeyEvent);
void vSubtractKeyCallBack(uint8_t ucKeyEvent);
void vUsbLeveInCallBack(uint8_t ucKeyEvent);
stSKTypes stSKAddSub[3] = {
    {
        .puiIsKeyRelease = uiIsKeyAddRelease,/**< 按键1回调函数 */
        .pvKeyCallBack = vAddKeyCallBack,  /**< 按键2回调函数 */
        0,0,0
    },
    {
        .puiIsKeyRelease = uiIsKeySubtractRelease,/**< 按键1回调函数 */
        .pvKeyCallBack = vSubtractKeyCallBack,  /**< 按键2回调函数 */
        0,0,0
    },
    {
        .puiIsKeyRelease = uiIsUsbRelease,/**< 按键1回调函数 */
        .pvKeyCallBack = vUsbLeveInCallBack,  /**< 按键2回调函数 */
        0,0,0
    },
};

void vNullKeyCallBack(uint8_t ucKeyEvent){
    ;
}
void vNullKeyCB(void * pData){
    ;
}

void vAddKeyCallBack(uint8_t ucKeyEvent){
    // PRINT("AppKey ucKeyEvent = %d\r\n",ucKeyEvent);
    switch (ucKeyEvent){
    case DE_SKEventDown0:{/**< 0按下事件 */
    }
        break;

    case DE_SKEventDown0Yes:{/**< 1通按下过抖动事件。注意：应用中和双击不能同时应用，不然有冲突 */
        ;
    }
        break;

    case DE_SKEventDown0No:{/**< 2按下不通过抖动事件 */
    }
        break;

    case DE_SKEventUp0:{/**< 3抬起事件 */
    }
        break;

    case DE_SKEventUp0Yes:{/**< 4抬起通过抖动事件 */
        vRbWrite(&stRbForKey,DE_AddKeydown);
        // PRINT("AppKey DE_SKEventUp0Yes\r\n");
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
        // vRbWrite(&stRbForKey,DE_AddKeydown);
        // stTMData.unCtrlData.bits.b2PowerMode = DE_PowerModeOn;//进入正常开机模式
    }
        break;

    case DE_SKEventUp1:{/**< 10再次抬起事件 */
    }
        break;

    case DE_SKEventUp1Yes:{/**< 11再次抬起通过抖动事件 */
    //如果检测到双击，打开强启标识，关闭lptimer唤醒，并退出关机低功耗模式，进入正常模式
        // stTMData.unCtrlData.bits.bForceEnable = 1;
        // vRbWrite(&stRbForKey,DE_AddKeyDoubleDown);
        // stTMData.unCtrlData.bits.b2PowerMode = DE_PowerModeOn;//进入正常开机模式
        // PRINT("AppKey DE_SKEventUp1Yes\r\n");
    }
        break;

    case DE_SKEventUp1No:{/**< 12再次抬起不通过抖动事件 */
    }
        break;

    case DE_SKEventDownLongYes:{/**< 13长按确认事件 */
        vRbWrite(&stRbForKey,DE_AddKeyLongDown);
        // PRINT("AppKey DE_SKEventDownLongYes\r\n");
    }
        break;

    case DE_SKEventUpL:{/**< 14长按抬起事件 */
    }
        break;

    case DE_SKEventUpLYes:{/**< 15长按抬起通过抖动事件 */
    //如果检测到按键长按，关闭lptimer唤醒，并退出关机低功耗模式，进入正常模式
        // vRbWrite(&stRbForKey,DE_AddKeyLongDown);
        // stTMData.unCtrlData.bits.b2PowerMode = DE_PowerModeOn;//进入正常开机模式

    }
        break;

    case DE_SKEventUpLNo:{/**< 16长按抬起不通过抖动事件 */
    }
        break;

    case DE_SKEventDownContinueYes0:{/**< 17连按确认开始事件 */
        vRbWrite(&stRbForKey,DE_AddKeyContinueDown);
        // PRINT("AppKey DE_SKEventDownContinueYes0\r\n");
    }
        break;

    case DE_SKEventDownContinueYes:{/**< 18连按确认事件 */
        vRbWrite(&stRbForKey,DE_AddKeyContinueDown);
        // PRINT("AppKey DE_SKEventDownContinueYes\r\n");
    }
        break;

    case DE_SKEventUpC:{/**< 19连按抬起事件 */
    }
        break;

    case DE_SKEventUpCYes:{/**< 20连按抬起通过抖动事件 */
        // vPowerOnKeyWakeupOn();
        // __HAL_LPTIM_DISABLE(&stLptimHandle);/* 关闭LPTIM定时器 */
        // HAL_NVIC_DisableIRQ(LPTIM1_IRQn);  /* 开启LPTIM全局中断 */
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

void vSubtractKeyCallBack(uint8_t ucKeyEvent){
    // PRINT("AppKey ucKeyEvent = %d\r\n",ucKeyEvent);
    switch (ucKeyEvent){
    case DE_SKEventDown0:{/**< 0按下事件 */
    }
        break;

    case DE_SKEventDown0Yes:{/**< 1通按下过抖动事件。注意：应用中和双击不能同时应用，不然有冲突 */
        ;
    }
        break;

    case DE_SKEventDown0No:{/**< 2按下不通过抖动事件 */
    }
        break;

    case DE_SKEventUp0:{/**< 3抬起事件 */
    }
        break;

    case DE_SKEventUp0Yes:{/**< 4抬起通过抖动事件 */
        vRbWrite(&stRbForKey,DE_SubtractKeydown);
        // PRINT("AppKey DE_SKEventUp0Yes\r\n");
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
        // vRbWrite(&stRbForKey,DE_SubtractKeydown);
        // stTMData.unCtrlData.bits.b2PowerMode = DE_PowerModeOn;//进入正常开机模式
    }
        break;

    case DE_SKEventUp1:{/**< 10再次抬起事件 */
    }
        break;

    case DE_SKEventUp1Yes:{/**< 11再次抬起通过抖动事件 */
    //如果检测到双击，打开强启标识，关闭lptimer唤醒，并退出关机低功耗模式，进入正常模式
        // stTMData.unCtrlData.bits.bForceEnable = 1;
        // vRbWrite(&stRbForKey,DE_SubtractKeyDoubleDown);
        // stTMData.unCtrlData.bits.b2PowerMode = DE_PowerModeOn;//进入正常开机模式
        // PRINT("AppKey DE_SKEventUp1Yes\r\n");
    }
        break;

    case DE_SKEventUp1No:{/**< 12再次抬起不通过抖动事件 */
    }
        break;

    case DE_SKEventDownLongYes:{/**< 13长按确认事件 */
        vRbWrite(&stRbForKey,DE_SubtractKeyLongDown);
        // PRINT("AppKey DE_SKEventDownLongYes\r\n");
    }
        break;

    case DE_SKEventUpL:{/**< 14长按抬起事件 */
    }
        break;

    case DE_SKEventUpLYes:{/**< 15长按抬起通过抖动事件 */
    //如果检测到按键长按，关闭lptimer唤醒，并退出关机低功耗模式，进入正常模式
        // vRbWrite(&stRbForKey,DE_SubtractKeyLongDown);
        // stTMData.unCtrlData.bits.b2PowerMode = DE_PowerModeOn;//进入正常开机模式

    }
        break;

    case DE_SKEventUpLNo:{/**< 16长按抬起不通过抖动事件 */
    }
        break;

    case DE_SKEventDownContinueYes0:{/**< 17连按确认开始事件 */
        vRbWrite(&stRbForKey,DE_SubtractKeyContinueDown);
        // PRINT("AppKey DE_SKEventDownContinueYes0\r\n");
    }
        break;

    case DE_SKEventDownContinueYes:{/**< 18连按确认事件 */
        vRbWrite(&stRbForKey,DE_SubtractKeyContinueDown);
        // PRINT("AppKey DE_SKEventDownContinueYes\r\n");
    }
        break;

    case DE_SKEventUpC:{/**< 19连按抬起事件 */
    }
        break;

    case DE_SKEventUpCYes:{/**< 20连按抬起通过抖动事件 */
        // vPowerOnKeyWakeupOn();
        // __HAL_LPTIM_DISABLE(&stLptimHandle);/* 关闭LPTIM定时器 */
        // HAL_NVIC_DisableIRQ(LPTIM1_IRQn);  /* 开启LPTIM全局中断 */
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

void vOnKeyCallBack(void * pData){
    // unsigned char ucKeyEvent;
    stScanKeyTypes * pstSk1 = (stScanKeyTypes *)pData;
    // PRINT("AppKey pstSk1->ucSKEvent = %d\r\n",pstSk1->ucSKEvent);
    switch (pstSk1->ucSKEvent){
    case DE_SKEventDown0:{/**< 0按下事件 */
    }
        break;

    case DE_SKEventDown0Yes:{/**< 1通按下过抖动事件。注意：应用中和双击不能同时应用，不然有冲突 */
        ;
    }
        break;

    case DE_SKEventDown0No:{/**< 2按下不通过抖动事件 */
    }
        break;

    case DE_SKEventUp0:{/**< 3抬起事件 */
    }
        break;

    case DE_SKEventUp0Yes:{/**< 4抬起通过抖动事件 */
        // vRbWrite(&stRbForKey,DE_PowerOnKeydown);
        // PRINT("AppKey DE_SKEventUp0Yes\r\n");
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
        vRbWrite(&stRbForKey,DE_PowerOnKeydown);
        // stTMData.unCtrlData.bits.b2PowerMode = DE_PowerModeOn;//进入正常开机模式
    }
        break;

    case DE_SKEventUp1:{/**< 10再次抬起事件 */
    }
        break;

    case DE_SKEventUp1Yes:{/**< 11再次抬起通过抖动事件 */
    //如果检测到双击，打开强启标识，关闭lptimer唤醒，并退出关机低功耗模式，进入正常模式
        // stTMData.unCtrlData.bits.bForceEnable = 1;
        vRbWrite(&stRbForKey,DE_PowerOnKeyDoubleDown);
        // stTMData.unCtrlData.bits.b2PowerMode = DE_PowerModeOn;//进入正常开机模式
        // PRINT("AppKey DE_SKEventUp1Yes\r\n");
    }
        break;

    case DE_SKEventUp1No:{/**< 12再次抬起不通过抖动事件 */
    }
        break;

    case DE_SKEventDownLongYes:{/**< 13长按确认事件 */
        vRbWrite(&stRbForKey,DE_PowerOnKeyLongDown);
        // PRINT("AppKey DE_SKEventDownLongYes\r\n");
    }
        break;

    case DE_SKEventUpL:{/**< 14长按抬起事件 */
    }
        break;

    case DE_SKEventUpLYes:{/**< 15长按抬起通过抖动事件 */
    //如果检测到按键长按，关闭lptimer唤醒，并退出关机低功耗模式，进入正常模式
        // vRbWrite(&stRbForKey,DE_PowerOnKeyLongDown);
        // stTMData.unCtrlData.bits.b2PowerMode = DE_PowerModeOn;//进入正常开机模式
        // vRbWrite(&stRbForKey,DE_PowerOnKeyUp);
        vRbWrite(&stRbForKey,DE_PowerOnKeyLongDownUp);
                // LCD_Fill(0,0,LCD_W,LCD_H,BLACK);
                // vLcdShowNumChar(0,stMainUI.stLogo.ucStartX,stMainUI.stLogo.ucStartY,stMainUI.stLogo.stChar,DE_LogoCharCorlor,BLACK,0);//显示开机LOGO
                // stTMData.unCtrlData.bits.b2PowerMode = DE_PowerModeOff;//进入关机低功耗模式
    }
        break;

    case DE_SKEventUpLNo:{/**< 16长按抬起不通过抖动事件 */
    }
        break;

    case DE_SKEventDownContinueYes0:{/**< 17连按确认开始事件 */
        vRbWrite(&stRbForKey,DE_PowerOnKeyContinueDown);
        // PRINT("AppKey DE_SKEventDownContinueYes0\r\n");
    }
        break;

    case DE_SKEventDownContinueYes:{/**< 18连按确认事件 */
        // BSP_LED_Toggle(LED1);
        // BSP_LED_Toggle(LED2);
        // BSP_LED_Toggle(LED3);
        // PRINT("AppKey DE_SKEventDownContinueYes\r\n");
    }
        break;

    case DE_SKEventUpC:{/**< 19连按抬起事件 */
        ;
    }
        break;

    case DE_SKEventUpCYes:{/**< 20连按抬起通过抖动事件 */
        // vPowerOnKeyWakeupOn();
        // __HAL_LPTIM_DISABLE(&stLptimHandle);/* 关闭LPTIM定时器 */
        // HAL_NVIC_DisableIRQ(LPTIM1_IRQn);  /* 开启LPTIM全局中断 */
        vRbWrite(&stRbForKey,DE_PowerOnKeyUp);
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

void vGetKeyEventProcess(void){
    static uint32_t uiN10ms = 0;
    unsigned char ucKeyEvent;
    unsigned char ucVoltageTmp,ucNPSFrequencyTmp;
    unsigned char ucM123 = stTMData.unCtrlData.bits.b2M123;
    if(ucRbRead(&stRbForKey,&ucKeyEvent)){
        uiN10ms = 0;
        switch(ucKeyEvent){
            case DE_PowerOnKeydown:{
                stTMData.unCtrlData.bits.bStartStop ^= 1;
            }break;
            case DE_PowerOnKeyLongDown:{
                if(stTMData.unCtrlData.bits.bStartStop == DE_MotorRun){
                    stTMData.unCtrlData.bits.bStartStop = DE_MotorStop;
                }
            }break;
            case DE_PowerOnKeyLongDownUp:{
                LCD_Fill(0,0,LCD_W,LCD_H,BLACK);
                // vLcdShowNumPic(0,stMainUI.stLogo.ucStartX,stMainUI.stLogo.ucStartY,stMainUI.stLogo.stChar);//,DE_LogoCharCorlor,BLACK,0);//显示开机LOGO
                stTMData.unCtrlData.bits.b2PowerMode = DE_PowerModeOff;//进入关机低功耗模式
            }break;
            case DE_PowerOnKeyContinueDown:{
                LCD_Fill(0,0,LCD_W,LCD_H,BLACK);
                // vLcdShowNumPic(0,stMainUI.stLogo.ucStartX,stMainUI.stLogo.ucStartY,stMainUI.stLogo.stChar);//,DE_LogoCharCorlor,BLACK,0);//显示开机LOGO
                stSKAddSub[0].pvKeyCallBack = vNullKeyCallBack;
                stSKAddSub[1].pvKeyCallBack = vNullKeyCallBack;
                stSKAddSub[2].pvKeyCallBack = vNullKeyCallBack;
                astOnPowerOnKeySK[1].vfpSKCallBack = vNullKeyCB;
            }break;
            case DE_PowerOnKeyUp:{
                // LCD_Fill(0,0,LCD_W,LCD_H,BLACK);
                // vLcdShowNumChar(0,stMainUI.stLogo.ucStartX,stMainUI.stLogo.ucStartY,stMainUI.stLogo.stChar,DE_LogoCharCorlor,BLACK,0);//显示开机LOGO
                stTMData.unCtrlData.bits.b2PowerMode = DE_PowerModeOff;//进入关机低功耗模式
            }break;
            case DE_PowerOnKeyDoubleDown:{
                if(stTMData.unCtrlData.bits.bStartStop == DE_MotorStop){
                    stTMData.uiMotorRunTimeSec = 0;//电机启动运行累计时间清零
                    uiSecondCount = 0;//秒计数清零
                }
            }break;
            // case DE_ModeKeydown:{
            //     ucM123 = (ucM123 + 1) % 4;
            //     stTMData.unCtrlData.bits.b2M123 = ucM123;
            //     // if(0 == stTMData.unMData[ucM123].bits.bNPSModeEnable){
            //         uiNpsSetFlag = 0;
            //     // }
            // }break;
            // case DE_ModeKeyLongDown:{//启动针刺模式
            //     stTMData.unMData[ucM123].bits.bNPSModeEnable ^= 1;
            //     // if(0 == stTMData.unMData[ucM123].bits.bNPSModeEnable){
            //         uiNpsSetFlag = 0;
            //     // }
            // }break;
            // case DE_ModeKeyDoubleDown:{//启动针刺频率调节开关
            //     if(stTMData.unMData[ucM123].bits.bNPSModeEnable){
            //         uiNpsSetFlag ^= 1;
            //     }
            //     if(1 == uiNpsSetFlag){//启动针刺频率调节超时恢复
            //         /* code */
            //     }
            // }break;
            case DE_AddKeydown:{
                if(stTMData.unMData[ucM123].bits.bNPSModeEnable){//针刺模式
                    if(0 == uiNpsSetFlag){//按键为调节针刺频率
                        if (stTMData.unMData[ucM123].bits.ucNPSFrequencySet < 10){
                            stTMData.unMData[ucM123].bits.ucNPSFrequencySet++;
                        }else{
                            stTMData.unMData[ucM123].bits.bNPSModeEnable = 0;
                            stTMData.unMData[ucM123].bits.ucStdVoltageSet = DE_MinSetVoltage;
                        }
                    }else{//按键为调节针刺电压值
                        if (stTMData.unMData[ucM123].bits.ucVoltageSet < DE_MaxSetVoltage){
                            stTMData.unMData[ucM123].bits.ucVoltageSet++;
                        }
                    }
                }else{//标准模式，按键为调节标准电压值
                    if (stTMData.unMData[ucM123].bits.ucStdVoltageSet < DE_MaxSetVoltage){
                        stTMData.unMData[ucM123].bits.ucStdVoltageSet++;
                    }
                }
            }break;
            case DE_AddKeyLongDown:
            case DE_AddKeyContinueDown:{
                if(stTMData.unMData[ucM123].bits.bNPSModeEnable){//针刺模式
                    if(0 == uiNpsSetFlag){//按键为调节针刺频率
                        if (stTMData.unMData[ucM123].bits.ucNPSFrequencySet < (10 - 1)){
                            stTMData.unMData[ucM123].bits.ucNPSFrequencySet += 2;
                        }else{
                            stTMData.unMData[ucM123].bits.bNPSModeEnable = 0;
                            stTMData.unMData[ucM123].bits.ucStdVoltageSet = DE_MinSetVoltage;
                        }
                    }else{//按键为调节针刺电压值
                        if (stTMData.unMData[ucM123].bits.ucVoltageSet < (DE_MaxSetVoltage -2)){
                            stTMData.unMData[ucM123].bits.ucVoltageSet += 3;
                        }else {
                            stTMData.unMData[ucM123].bits.ucVoltageSet = DE_MaxSetVoltage;
                        }
                    }
                }else{//标准模式，按键为调节标准电压值
                    if (stTMData.unMData[ucM123].bits.ucStdVoltageSet < (DE_MaxSetVoltage -2)){
                        stTMData.unMData[ucM123].bits.ucStdVoltageSet += 3;
                    }else {
                        stTMData.unMData[ucM123].bits.ucStdVoltageSet = DE_MaxSetVoltage;
                    }
                }
            }break;
            // case DE_AddKeyDoubleDown:{
            //     ;
            // }break;
            case DE_SubtractKeydown:{
                if(stTMData.unMData[ucM123].bits.bNPSModeEnable){//针刺模式
                    if(0 == uiNpsSetFlag){//按键为调节针刺频率
                        if(stTMData.unMData[ucM123].bits.ucNPSFrequencySet > 1){
                            stTMData.unMData[ucM123].bits.ucNPSFrequencySet--;
                        }
                    }else{//按键为调节针刺电压值
                        if (stTMData.unMData[ucM123].bits.ucVoltageSet > DE_MinSetVoltage){
                            stTMData.unMData[ucM123].bits.ucVoltageSet--;
                        }
                    }
                }else{//标准模式，按键为调节标准电压值
                    if (stTMData.unMData[ucM123].bits.ucStdVoltageSet > DE_MinSetVoltage){
                        stTMData.unMData[ucM123].bits.ucStdVoltageSet--;
                    }else{
                        stTMData.unMData[ucM123].bits.bNPSModeEnable = 1;
                        stTMData.unMData[ucM123].bits.ucVoltageSet = DE_MaxSetVoltage;
                        stTMData.unMData[ucM123].bits.ucNPSFrequencySet = 10;
                    }
                }
            }break;
            case DE_SubtractKeyLongDown:
            case DE_SubtractKeyContinueDown:{
                if(stTMData.unMData[ucM123].bits.bNPSModeEnable){//针刺模式
                    if(0 == uiNpsSetFlag){//按键为调节针刺频率
                        if(stTMData.unMData[ucM123].bits.ucNPSFrequencySet > (1 + 1)){
                            stTMData.unMData[ucM123].bits.ucNPSFrequencySet -= 2;
                        }else{
                            stTMData.unMData[ucM123].bits.ucNPSFrequencySet = 1;
                        }
                    }else{//按键为调节针刺电压值
                        if (stTMData.unMData[ucM123].bits.ucVoltageSet > (DE_MinSetVoltage + 2 )){
                            stTMData.unMData[ucM123].bits.ucVoltageSet -= 3;
                        }else{
                            stTMData.unMData[ucM123].bits.ucVoltageSet = DE_MinSetVoltage;
                        }
                    }
                }else{//标准模式，按键为调节标准电压值
                    if (stTMData.unMData[ucM123].bits.ucStdVoltageSet > (DE_MinSetVoltage + 2 )){
                        stTMData.unMData[ucM123].bits.ucStdVoltageSet -= 3;
                    }else{
                        stTMData.unMData[ucM123].bits.bNPSModeEnable = 1;
                        stTMData.unMData[ucM123].bits.ucVoltageSet = DE_MaxSetVoltage;
                        stTMData.unMData[ucM123].bits.ucNPSFrequencySet = 10;
                    }
                }
            }break;
            // case DE_UsbLeveldown:{
            case DE_UsbLevelLongDown:{
                if(stTMData.unCtrlData.bits.bStartStop == DE_MotorRun){
                    stTMData.unCtrlData.bits.bStartStop = DE_MotorStop;
                }
                stTMData.unCtrlData.bits.b2PowerMode = DE_PowerModeCharge;//进入关机低功耗模式
                // PRINT("DE_UsbLeveldown\r\n");
                PRINT("DE_UsbLevelLongDown\r\n");
            }break;
            default:{
                ;
            }break;
        }
    }else{
        if( DE_MotorStop == stTMData.unCtrlData.bits.bStartStop){
            if(++uiN10ms >= 90000){
                stTMData.unCtrlData.bits.b2PowerMode = DE_PowerModeOff;//进入关机低功耗模式
                uiN10ms = 0;
            }
        }else{
            uiN10ms = 0;
        }
    }
}

DE_TTaskSection(ttaskTable)

void vScan(void){
   vScanKey(&astOnKeyCtrlData,astOnPowerOnKeySK,2);
   vKeyLongProcess(stSKAddSub,3,stSKArg);
   vGetKeyEventProcess();
}

DE_AddToTTaskTable(10,vScan,ttaskTable,1)

void vScanKeyProjectInit(void){
    /* 上电时，ScanKeyApp工程数据初始化 */
    ucPowerState = 1;//注释此行为上电直接进入关机低功耗模式
}

static void vStepInPowerOnInit(void){/* 开机时，ScanKeyApp工程初始化 */
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOF);

    // vDelayNms(2000/(1000/DE_SystickFrq));
    // LL_mDelay(10000/(1000/DE_SystickFrq));

    //配置开机按键为上拉输入模式，并等待按键释放
    DE_CfgPinInput(DE_PowerKeyPort,DE_PowerKeyPin,LL_GPIO_SPEED_FREQ_LOW,LL_GPIO_PULL_UP);
    DE_CfgPinInput(DE_ModeKeyPort,DE_ModeKeyPin,LL_GPIO_SPEED_FREQ_LOW,LL_GPIO_PULL_UP);
    while(0 == LL_GPIO_IsInputPinSet(DE_PowerKeyPort,DE_PowerKeyPin)){;}//等待按键释放，不然后续按键识别会有bug
    DE_CfgPinInput(DE_AddKeyPort,DE_AddKeyPin,LL_GPIO_SPEED_FREQ_LOW,LL_GPIO_PULL_UP);
    DE_CfgPinInput(DE_SubtractKeyPort,DE_SubtractKeyPin,LL_GPIO_SPEED_FREQ_LOW,LL_GPIO_PULL_UP);
    //配置电池供电输出
    DE_BatteryOupPinInit();
    DE_BatteryOutOn();

    memset(astOnPowerOnKeySK,0,sizeof(astOnPowerOnKeySK));
    // astOnPowerOnKeySK[0].ucSKStatus = 0xff;/**< 按键1回调函数 */
    astOnPowerOnKeySK[0].vfpSKCallBack = vOnKeyCallBack;/**< 按键1回调函数 */
    // astOnPowerOnKeySK[1].ucSKStatus = 0xff;/**< 按键1回调函数 */
    astOnPowerOnKeySK[1].vfpSKCallBack = vMemoryKeyCallBack;/**< 按键1回调函数 */
    vScanKeyRead(&astOnKeyCtrlData,astOnPowerOnKeySK);
    // astOnKeyCtrlData.kwContinue = skwSKPortReadKeyVal(astOnPowerOnKeySK);/**< 用于记录激活的按键数据,按键按下表示激活,对应位置1 */
    // astOnPowerOnKeySK[2].vfpSKCallBack = vAddKeyCallBack;/**< 按键1回调函数 */
    // astOnPowerOnKeySK[3].vfpSKCallBack = vSubtractKeyCallBack;/**< 按键1回调函数 */
    // astOnPowerOnKeySK[2].vfpSKCallBack = vUsbLeveInCallBack;/**< 按键1回调函数 */
    vRbInit(&stRbForKey,ucKeyEventArray,sizeof(ucKeyEventArray) - 1);
    stSKAddSub[0].pvKeyCallBack = vAddKeyCallBack;
    stSKAddSub[1].pvKeyCallBack = vSubtractKeyCallBack;
    stSKAddSub[2].pvKeyCallBack = vUsbLeveInCallBack;
    DE_WDogTimerInit(2000);
    DE_WDogTimerStart();
}

static void vStepOutPowerOn(void){
    DE_WDogTimerStop();
    DE_BatteryOutOFF();
}

void vPowerOnProcess(void){
    vStepInPowerOnInit();
    while(ucPowerState == DE_PowerModeOn){//正常开机模式，主循环
        DE_WDogTimerFeeds();
        // while(0 == LL_GPIO_IsInputPinSet(DE_PowerKeyPort,DE_PowerKeyPin)){;}//等待按键释放,用于测试定时器模拟看门狗功能
        // vScanKey(&astOnKeyCtrlData,astOnPowerOnKeySK,5);/**< 按键扫描，间隔1ms */
        // vGetKeyEventProcess();

        #ifdef EnAutoPoweroff
            ucPowerState = DE_PowerModeOff;
        #endif
    }
    vStepOutPowerOn();
}

void vPowerOffProcess(void){;}
void vChargeProcess(void){;}


/***************** (C) COPYRIGHT axushilong@163.com *****END OF FILE***********/
