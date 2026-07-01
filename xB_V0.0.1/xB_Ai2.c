/**
  * @file    xB.c
  * @brief   通用按键扫描中间件 - 零硬件依赖实现
  * @author  axushilong@163.com
  * @version v0.0.1
  * @date    2026.06.29
  *
  * 核心设计:
  *   - pfnReadRaw读原始值 → 消抖 → 事件检测
  *   - 按键值 = 位掩码: 单键=单bit, 组合=多bit, 天然兼容
  *   - 所有硬件操作通过回调, 本文件不#include任何平台头文件
  */

#include "xB.h"

void vXB(stXBTypes * pstXB){
    uint32_t uiEvent;
    uxWide_t uxNotifyVal;
    uxWide_t changedBits;
    int i;
    stXBCfgTypes * pstXBCfg = pstXB->pstXBCfg;
    uxWide_t uxCurVal = pstXBCfg->puxReadVal(); // 读取原始值
    
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
        if(uxCurVal == DE_NoPress){ // 全部松开
            pstXB->ucReleasCnt = 1;
            
            // 检查是否之前有按键按下
            if(pstXB->uxLastValue != DE_NoPress) {
                // 对于每个之前按下的按键，检查它们的状态
                changedBits = pstXB->uxLastValue & (~uxCurVal); // 找出被释放的按键
                
                // 检查是否是长按释放
                if(pstXB->usPressCnt >= pstXBCfg->usLongTime) {
                    // 长按结束
                    uiEvent = DE_XBEventUp;
                    uxNotifyVal = changedBits;
                    pstXB->ucClickCnt = 1; // 长按只算一次
                    goto _LabelXBEventNotify;
                } else if(pstXB->usPressCnt > 0) {
                    // 单击事件，需要等待确认是否有多击
                    pstXB->uxNotifyValue = changedBits; // 记录释放的按键
                }
            }
        } else { // 按下
            changedBits = uxCurVal & (~pstXB->uxLastValue); // 找出新按下的按键
            
            if(pstXB->uxLastValue == DE_NoPress) { // 从全松开状态按下
                // 新按键按下，记录按键值和增加点击次数
                pstXB->uxNotifyValue = changedBits;
                pstXB->ucClickCnt = 1;
            } else { // 从部分按键状态改变
                // 如果是新的按键按下（不是之前按下的按键释放）
                if(changedBits != 0) {
                    // 检查是否是快速连续点击（多击）
                    if(pstXB->ucReleasCnt > 0 && pstXB->ucReleasCnt <= pstXBCfg->ucClickDelayTime) {
                        // 在多击时间内再次按下，增加计数
                        if(pstXB->ucClickCnt < pstXBCfg->ucClickCntMax) {
                            pstXB->ucClickCnt++;
                        }
                    } else {
                        // 超过多击时间，重新开始计数
                        pstXB->ucClickCnt = 1;
                        pstXB->uxNotifyValue = changedBits;
                    }
                }
            }
            
            pstXB->usPressCnt = 1; // 开始计时
        }
    } else {
        // 值保持不变
        if(uxCurVal == DE_NoPress) {
            // 保持松开状态
            if(pstXB->ucReleasCnt > 0) {
                pstXB->ucReleasCnt++;
                
                // 检查是否达到多击间隔时间
                if(pstXB->ucReleasCnt > pstXBCfg->ucClickDelayTime) {
                    // 多击超时，确认发送点击事件
                    if(pstXB->ucClickCnt > 0) {
                        uiEvent = DE_XBEventClick;
                        uxNotifyVal = pstXB->uxNotifyValue;
                        goto _LabelXBEventNotify;
                    }
                    
                    // 进入空闲状态
                    if(pstXBCfg->pvIdleHandler) {
                        pstXBCfg->pvIdleHandler(pstXB->uxLastValue);
                    }
                    pstXB->ucReleasCnt = 0;
                }
            }
        } else {
            // 保持按下状态
            pstXB->usPressCnt++;
            
            // 检查是否达到长按时间
            if(pstXB->usPressCnt == pstXBCfg->usLongTime) {
                uiEvent = DE_XBEventLong;
                uxNotifyVal = uxCurVal;
                pstXB->ucClickCnt = 1; // 长按只算一次
                goto _LabelXBEventNotify;
            }
            
            // 检查是否达到Hold时间（周期性触发）
            if((pstXB->usPressCnt > pstXBCfg->usLongTime) && 
               ((pstXB->usPressCnt - pstXBCfg->usLongTime) % pstXBCfg->ucHoldTime == 0)) {
                uiEvent = DE_XBEventHold;
                uxNotifyVal = uxCurVal;
                pstXB->ucClickCnt = 1; // Hold也只算一次
                goto _LabelXBEventNotify;
            }
        }
    }
    
    goto _LabelXBEnd;

_LabelXBEventNotify:
    // 事件回调处理
    pstXB->pstXBCfg->pvEventHandler(uxNotifyVal, uiEvent, pstXB->ucClickCnt);
    
    // 特殊处理：如果是点击事件，需要继续等待可能的多击
    if(uiEvent == DE_XBEventClick) {
        // 不重置计数器，等待更多点击
    } else if(uiEvent == DE_XBEventUp || uiEvent == DE_XBEventLong || uiEvent == DE_XBEventHold) {
        // 其他事件，重置状态
        pstXB->ucReleasCnt = 0;
        pstXB->ucClickCnt = 0;
    }

_LabelXBEnd:
    pstXB->uxLastValue = uxCurVal;
}