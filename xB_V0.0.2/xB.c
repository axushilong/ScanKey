/**
  * @file    xB.c
  * @brief   通用按键扫描中间件 - 零硬件依赖实现
  * @author  axushilong@163.com
  * @version v0.0.2
  * @date    2026.06.29
  *
  * 核心设计:
  *   - pfnReadRaw读原始值 → 消抖 → 事件检测
  *   - 按键值 = 位掩码: 单键=单bit, 组合=多bit, 天然兼容
  *   - 所有硬件操作通过回调, 本文件不#include任何平台头文件
  */

#include "xB.h"



static uint8_t ucIsOnlyClick(uxWide_t uxXBCapOnlyClick, uxWide_t uxXBValue){
    if(uxXBValue & (uxXBValue -1)){ //组合按键
    #if defined(DE_OnlyClickForComb)
        return 1;
    #else
        return 0;
    #endif
    }else{
        return (uxXBValue & uxXBCapOnlyClick);
    }
}

// static bool bIsNoMultiClick(uxWide_t uxXBCapNoMultiClick, uxWide_t uxXBValue){
//     // if(uxXBValue & (uxXBValue -1)){ //组合按键
//     //     return false;
//     // }else{
//     //     return uxXBValue & uxXBCapNoMultiClick;
//     // }
//     return (uxXBValue & (uxXBValue -1))? false : (uxXBValue & uxXBCapNoMultiClick);
// }
#define DE_IsNoMultiClick(a, b) ((b & (b -1)) ? 0 : (b & a))

void vXB(stXBTypes * pstXB){
    stXBCfgTypes * pstXBCfg = pstXB->pstXBCfg;

    // static uxWide_t uxNotifyVal = DE_NoPress; // 事件通知值
    uint32_t uiEvent;

    uxWide_t uxCurVal = pstXBCfg->puxReadVal(); // 调用回调读取原始值
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
        if/*松开开始*/(uxCurVal == DE_NoPress){ // 变成无按键按下，即松开
            //不是长按或按住后松开，如果是只支持单击功能的按键松开，则释放单击松开事件
            pstXB->ucReleasCnt = 1;
            if(ucIsOnlyClick(pstXBCfg->uxCapOnlyClick, pstXB->uxLastValue)){
                uiEvent = DE_XBEventUp; // 单击松开事件
                printf("xBEventNotify-%d: keyVal=%d, event=%d, clickCnt=%d\n", __LINE__, pstXB->uxNotifyValue, uiEvent, pstXB->ucClickCnt);
                goto _LabelXBEventNotify;
            }
            if(DE_IsNoMultiClick(pstXBCfg->uxCapNoMultiClick, pstXB->uxLastValue)){ //不支持多击功能的按键松开
                if(pstXB->usPressCnt >= pstXBCfg->usLongTime){ // 长按或按住后松开
                    uiEvent = DE_XBEventUp; // 长按松开事件
                    printf("xBEventNotify-%d: keyVal=%d, event=%d, clickCnt=%d\n", __LINE__, pstXB->uxNotifyValue, uiEvent, pstXB->ucClickCnt);
                    goto _LabelXBEventNotify;
                }
                //不是长按或按住后松开，默认功能的按键松开，则释放松开单击事件
                pstXB->uxNotifyValue = pstXB->uxLastValue;
                uiEvent = DE_XBEventClick; // 松开单击事件
                printf("xBEventNotify-%d: keyVal=%d, event=%d, clickCnt=%d\n", __LINE__, pstXB->uxNotifyValue, uiEvent, pstXB->ucClickCnt);
                goto _LabelXBEventNotify;
            }
            // 多击按键松开只在超过最大多击次数时才上报
            if(pstXB->ucClickCnt == pstXBCfg->ucClickCntMax){ //已经是最大多击次数时
                uiEvent = pstXB->ucMultiClickEvent;
                printf("xBEventNotify-%d: keyVal=%d, event=%d, clickCnt=%d\n", __LINE__, pstXB->uxNotifyValue, uiEvent, pstXB->ucClickCnt);
                goto _LabelXBEventNotify;
            }
        }else/*按下开始*/{ //变成其他键按下{1.从松开变到按下；2.从一个按键按下变到另一个按键按下}
            pstXB->usPressCnt = 1;
            if(uxCurVal != pstXB->uxNotifyValue){ // 从一个按键按下变到另一个按键按下
                /* 不同按键: 先上报旧按键 */
                if(pstXB->ucClickCnt > 0 && pstXB->uxNotifyValue != DE_NoPress){
                    uiEvent = (pstXB->ucClickCnt >= 2) ? pstXB->ucMultiClickEvent : DE_XBEventClick;
                    pstXB->uxNotifyValue = uxCurVal;    // 记录当前按键值
                    printf("xBEventNotify-%d: keyVal=%d, event=%d, clickCnt=%d\n", __LINE__, pstXB->uxNotifyValue, uiEvent, pstXB->ucClickCnt);
                    pstXBCfg->pvEventHandler(pstXB->uxNotifyValue, uiEvent, pstXB->ucClickCnt);
                    pstXB->ucClickCnt = 0;
                    pstXB->ucMultiClickEvent = 0;
                }
                pstXB->uxNotifyValue = uxCurVal;    // 记录当前按键值
            }
            pstXB->ucClickCnt++;
            if(ucIsOnlyClick(pstXBCfg->uxCapOnlyClick, uxCurVal)){
                pstXB->uxNotifyValue = uxCurVal;
                uiEvent = DE_XBEventClick; // 单击事件
                printf("xBEventNotify-%d: keyVal=%d, event=%d, clickCnt=%d\n", __LINE__, pstXB->uxNotifyValue, uiEvent, pstXB->ucClickCnt);
                goto _LabelXBEventNotify;
            }
            goto _LabelXBEnd;
        }
    }
    //保持松开uxCurVal == pstXB->uxLastValue == DE_NoPress
    if(uxCurVal == DE_NoPress){
        if(pstXB->ucReleasCnt < 254){// pstXBCfg->ucClickDelayTime + pstXBCfg->ucHoldTime){
            pstXB->ucReleasCnt++;
        }
        if(pstXB->ucReleasCnt == 254){// pstXBCfg->ucClickDelayTime + pstXBCfg->ucHoldTime){ //保持松开超过一定时间，上报按键空闲事件
            pstXB->ucReleasCnt++;
            uiEvent = DE_XBEnentIdel; // 进入空闲状态
            printf("xBEventNotify-%d: keyVal=%d, event=%d, clickCnt=%d\n", __LINE__, pstXB->uxNotifyValue, uiEvent, pstXB->ucClickCnt);
            goto _LabelXBEventNotify;
        }
        if(pstXB->ucClickCnt > 0){//有多击事件要处理
            if(pstXB->ucReleasCnt >= pstXBCfg->ucClickDelayTime  && pstXB->ucClickCnt < pstXBCfg->ucClickCntMax){ //保持松开超过一定时间
                uiEvent = (pstXB->ucClickCnt >= 2) ? pstXB->ucMultiClickEvent : pstXB->ucMultiClickEvent ? DE_XBEventUp : DE_XBEventClick;
                printf("xBEventNotify-%d: keyVal=%d, event=%d, clickCnt=%d\n", __LINE__, pstXB->uxNotifyValue, uiEvent, pstXB->ucClickCnt);
                goto _LabelXBEventNotify;
            }
        }
        goto _LabelXBEnd;
    }
    //保持按下uxCurVal == pstXB->uxLastValue
    if(ucIsOnlyClick(pstXBCfg->uxCapOnlyClick, uxCurVal)){
        goto _LabelXBEnd;
    }
    pstXB->usPressCnt++;
    if(DE_IsNoMultiClick(pstXBCfg->uxCapNoMultiClick, pstXB->uxLastValue)){ //不支持多击功能的按键松开
        if(pstXB->usPressCnt == pstXBCfg->usLongTime){ // 长按
            pstXB->uxNotifyValue = uxCurVal;
            uiEvent = DE_XBEventLong; // 长按事件
            printf("xBEventNotify-%d: keyVal=%d, event=%d, clickCnt=%d\n", __LINE__, pstXB->uxNotifyValue, uiEvent, pstXB->ucClickCnt);
            goto _LabelXBEventNotify;
        }
        if(pstXB->usPressCnt >= pstXBCfg->usLongTime + pstXBCfg->ucHoldTime){ // 按住
            pstXB->usPressCnt = pstXBCfg->usLongTime;
            pstXB->uxNotifyValue = uxCurVal;
            uiEvent = DE_XBEventHold; // 按住事件
            printf("xBEventNotify-%d: keyVal=%d, event=%d, clickCnt=%d\n", __LINE__, pstXB->uxNotifyValue, uiEvent, pstXB->ucClickCnt);
            goto _LabelXBEventNotify;
        }
    }else{ // 支持多击的按键，长按只标记到 ucMultiClickEvent，不立即上报
        if(pstXB->usPressCnt == pstXBCfg->usLongTime){ // 长按标记
            pstXB->ucMultiClickEvent |= (1<<(pstXB->ucClickCnt-1)); // bit(N-1)=第N击是长击
            if(pstXB->ucClickCnt == 1){
                uiEvent = DE_XBEventLong; // 长按事件
                printf("xBEventNotify-%d: keyVal=%d, event=%d, clickCnt=%d\n", __LINE__, pstXB->uxNotifyValue, uiEvent, pstXB->ucClickCnt);
                // goto _LabelXBEventNotify;
                pstXBCfg->pvEventHandler(pstXB->uxNotifyValue, uiEvent, pstXB->ucClickCnt);
                pstXB->uxLastValue = uxCurVal;
                return;
            }
        }
    }
    goto _LabelXBEnd;
_LabelXBEventNotify:
    pstXBCfg->pvEventHandler(pstXB->uxNotifyValue, uiEvent, pstXB->ucClickCnt);
    pstXB->ucClickCnt = 0; // 重置多击计数
    pstXB->ucMultiClickEvent = 0; // 重置多击事件标记
_LabelXBEnd:
    pstXB->uxLastValue = uxCurVal;
}


