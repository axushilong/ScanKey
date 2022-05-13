#include "ScanKeyPort.h"

#define DE_SKEventCallBack(Key) ((struct ScanKeyType *)Key)->pfSKCallBack(((struct ScanKeyType *)Key))

//static skwide_t kwContinue = 0;  /**< continue(继续、连续)按键按下保持的信号 */
//static skwide_t kwTriger = 0;    /**< triger(触发)对应按键按下的信号,记录当前扫描是否有新的按键按下,有的话对应的位置1 */
//static skwide_t kwReset = 0;

static void vScanKeyRead(struct ScanKeyCtrlDataType * pstKeyCtrlData, struct ScanKeyType * pSK){
	skwide_t kwReadData = kwSKPortReadKeyVal(pSK);/**< 用于记录激活的按键数据,按键按下表示激活,对应位置1 */
	skwide_t kwChange = kwReadData ^ pstKeyCtrlData->kwContinue;/**< 记录当前和上次不同状态的按键数据,对应位为1表示此按键电平对比上次有变动 */

	pstKeyCtrlData->kwContinue = kwReadData;/**< 保存本次激活的位 */

	pstKeyCtrlData->kwTriger = kwReadData & kwChange;/**< 当前激活的位是否包含,当前激活和上次激活的不同的位;也就是上次未激活,当前激活的位 */
	pstKeyCtrlData->kwReset = kwChange & (~kwReadData);/**< 记录由按下到抬起的触发 */
}

void vScanKey(struct ScanKeyCtrlDataType * pstKeyCtrlData, struct ScanKeyType * pSK){
	uint8_t ucI = 0;
	vScanKeyRead(pstKeyCtrlData,pSK);
	for(ucI =0;  ucI < DE_SKCount; ucI++){
		if(pSK[ucI].ucSKStatus){
			pSK[ucI].usSKScanCount++;
		}
		switch(pSK[ucI].ucSKStatus){

			case DE_SKStatusStart:{/*开始状态*/
				if(pstKeyCtrlData->kwTriger & (1<<ucI)){/*对应按键动作按下*/
					pSK[ucI].ucDebounceTick = 1;/*开始抖动过滤检测*/
					pSK[ucI].ucSKEvent = DE_SKEventDown0;/*按下事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else if(pstKeyCtrlData->kwContinue & (1<<ucI)){/*对应按键保持按下*/
					pSK[ucI].ucDebounceTick++;
					if(pSK[ucI].ucDebounceTick >= DE_SKDebounceTick){/*通过抖动验证*/
						pSK[ucI].usSKScanCount = pSK[ucI].ucDebounceTick;
						pSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
						pSK[ucI].ucSKStatus = DE_SKStatusDown0;
						pSK[ucI].ucSKEvent = DE_SKEventDown0Yes;/*确认按下0事件*/
						DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
					}else{/*按键是按下的,但没有通过抖动事件,继续抖动验证*/
						;
					}
				}else if(pSK[ucI].ucDebounceTick){/*按键没有按下,且经过抖动验证,说明没通过按键按键抖动验证*/
					pSK[ucI].ucDebounceTick = 0;
					pSK[ucI].ucSKEvent = DE_SKEventDown0No;/*按键没有按下,抖动验证不通过事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else{/*按键是初始状态,一直没动*/
					;
				}
			}break;

			case DE_SKStatusDown0:{
				if(pstKeyCtrlData->kwReset & (1<<ucI)){/*对应按键动作抬起,复位*/
					pSK[ucI].ucDebounceTick = 1;/*开始抖动过滤检测*/
					pSK[ucI].ucSKEvent = DE_SKEventUp0;/*抬起事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else if((~pstKeyCtrlData->kwContinue) & (1<<ucI)){/*对应按键保持抬起*/
					pSK[ucI].ucDebounceTick++;
					if(pSK[ucI].ucDebounceTick >= DE_SKDebounceTick){/*通过抖动验证*/
						pSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
						pSK[ucI].usDoubleStartTick = pSK[ucI].usSKScanCount;/*用于双击抬起的时间判断*/
						pSK[ucI].ucSKStatus = DE_SKStatusUp0;
						pSK[ucI].ucSKEvent = DE_SKEventUp0Yes;/*确认抬起0事件*/
						DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
					}else{/*按键是抬起的,但没有通过抖动事件,继续抖动验证*/
						;
					}
				}else if(pSK[ucI].ucDebounceTick){/*按键按下,且经过抖动验证,说明没通过按键按键抖动验证*/
					pSK[ucI].ucDebounceTick = 0;
					pSK[ucI].ucSKEvent = DE_SKEventUp0No;/*按键没有抬起,抖动过滤成功事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else if(pSK[ucI].usSKScanCount >= De_SKLongStartTick){/*按键是一直按下的,且超过长按值*/
					pSK[ucI].ucSKStatus = DE_SKStatusDownLong;
					pSK[ucI].ucSKEvent = DE_SKEventDownLongYes;/*确认按下L事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else{/*按键是一直按下的,但没有超过长按值*/
					;
				}
			}break;

			case DE_SKStatusUp0:{
				if(pstKeyCtrlData->kwTriger & (1<<ucI)){/*对应按键触发按下*/
					pSK[ucI].ucDebounceTick = 1;/*开始抖动过滤检测*/
					pSK[ucI].ucSKEvent = DE_SKEventDown1;/*按下事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else if(pstKeyCtrlData->kwContinue & (1<<ucI)){/*对应按键保持按下*/
					pSK[ucI].ucDebounceTick++;
					if(pSK[ucI].ucDebounceTick >= DE_SKDebounceTick){/*通过抖动验证*/
						pSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
						pSK[ucI].ucSKStatus = DE_SKStatusDown1;
						pSK[ucI].ucSKEvent = DE_SKEventDown1Yes;/*确认按下0事件*/
						DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
					}else{/*按键是按下的,但没有通过抖动事件,继续抖动验证*/
						;
					}
				}else if(pSK[ucI].ucDebounceTick){/*按键抬起,且经过抖动验证没有通过*/
					pSK[ucI].ucDebounceTick = 0;
					pSK[ucI].ucSKEvent = DE_SKEventDown0No;/*按键没有按下,抖动过滤成功事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else if((pSK[ucI].usSKScanCount - pSK[ucI].usDoubleStartTick) >= DE_SKIntetvalTick){/*按键是抬起的,一直没动*/
					pSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
					pSK[ucI].ucSKStatus = DE_SKStatusStart/*SKStatusUp1Yes*/;
					pSK[ucI].ucSKEvent = DE_SKEventDoubleNo;/*确认非双击事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}
				else{/*按键是抬起的,一直没动,且抬起时间＜DE_SKClickTick*/
					;
				}
			}break;

			case DE_SKStatusDown1:{
				if(pstKeyCtrlData->kwReset & (1<<ucI)){/*对应按键触发抬起,复位*/
					pSK[ucI].ucDebounceTick = 1;/*开始抖动过滤检测*/
					pSK[ucI].ucSKEvent = DE_SKEventUp1;/*抬起事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else if((~pstKeyCtrlData->kwContinue) & (1<<ucI)){/*对应按键是抬起的*/
					pSK[ucI].ucDebounceTick++;
					if(pSK[ucI].ucDebounceTick >= DE_SKDebounceTick){/*通过抖动验证*/
						pSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
						pSK[ucI].ucSKStatus = DE_SKStatusStart/*SKStatusUp1Yes*/;
						pSK[ucI].ucSKEvent = DE_SKEventUp1Yes;/*确认抬起1事件*/
						DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
					}else{/*按键是抬起的,但没有通过抖动事件,继续抖动验证*/
						;
					}
				}else if(pSK[ucI].ucDebounceTick){/*按键按下,且经过抖动验证没有通过*/
					pSK[ucI].ucDebounceTick = 0;
					pSK[ucI].ucSKEvent = DE_SKEventUp1No;/*按键没有抬起,抖动过滤成功事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else{/*按键是初始状态,一直没动*/
					;
				}
			}break;

			case DE_SKStatusDownLong:{/*长按确认*/
				if(pstKeyCtrlData->kwReset & (1<<ucI)){/*对应按键触发抬起,复位*/
					pSK[ucI].ucDebounceTick = 1;/*开始抖动过滤检测*/
					pSK[ucI].ucSKEvent = DE_SKEventUpL;/*抬起事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else if((~pstKeyCtrlData->kwContinue) & (1<<ucI)){/*对应按键是抬起的*/
					pSK[ucI].ucDebounceTick++;
					if(pSK[ucI].ucDebounceTick >= DE_SKDebounceTick){/*通过抖动验证*/
						pSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
						pSK[ucI].ucSKStatus = DE_SKStatusStart/*SKStatusUp1Yes*/;
						pSK[ucI].ucSKEvent = DE_SKEventUpLYes;/*确认抬起0事件*/
						DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
					}else{/*按键是抬起的,但没有通过抖动事件,继续抖动验证*/
						;
					}
				}else if(pSK[ucI].ucDebounceTick){/*按键按下,且经过抖动验证没有通过*/
					pSK[ucI].ucDebounceTick = 0;
					pSK[ucI].ucSKEvent = DE_SKEventUpLNo;/*按键没有抬起,抖动过滤成功事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else if(pSK[ucI].usSKScanCount >= De_SKContinueStartTick){/*按键是一直按下的,且超过长按值*/
					// pSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
					pSK[ucI].usDoubleStartTick = pSK[ucI].usSKScanCount;/*用于双击抬起的时间判断*/
					pSK[ucI].ucSKStatus = DE_SKStatusDownContinue;
					pSK[ucI].ucSKEvent = DE_SKEventDownContinueYes;/*确认按下L事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else{/*按键是一直按下的,但没到连击起始值*/
					;
				}
			}break;

			case DE_SKStatusDownContinue:{/*连击确认*/
				if(pstKeyCtrlData->kwReset & (1<<ucI)){/*对应按键触发抬起,复位*/
					pSK[ucI].ucDebounceTick = 1;/*开始抖动过滤检测*/
					pSK[ucI].ucSKEvent = DE_SKEventUpC;/*抬起事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else if((~pstKeyCtrlData->kwContinue) & (1<<ucI)){/*对应按键是抬起的*/
					pSK[ucI].ucDebounceTick++;
					if(pSK[ucI].ucDebounceTick >= DE_SKDebounceTick){/*通过抖动验证*/
						pSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
						pSK[ucI].ucSKStatus = DE_SKStatusStart/*SKStatusUp1Yes*/;
						pSK[ucI].ucSKEvent = DE_SKEventUpCYes;/*确认抬起0事件*/
						DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
					}else{/*按键是抬起的,但没有通过抖动事件,继续抖动验证*/
						;
					}
				}else if(pSK[ucI].ucDebounceTick){/*按键按下,且经过抖动验证没有通过*/
					pSK[ucI].ucDebounceTick = 0;
					pSK[ucI].ucSKEvent = DE_SKEventUpCNo;/*按键没有抬起,抖动过滤成功事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else if((pSK[ucI].usSKScanCount - pSK[ucI].usDoubleStartTick) >= De_SKContinueTick){/*按键是一直按下的,且超连击值*/
					// pSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
					pSK[ucI].usDoubleStartTick = pSK[ucI].usSKScanCount;/*用于双击抬起的时间判断*/
					// pSK[ucI].ucSKStatus = SKStatusDownContinue;
					pSK[ucI].ucSKEvent = DE_SKEventDownContinueYes;/*确认按下L事件*/
					DE_SKEventCallBack(&pSK[ucI]);//pSK[ucI].pfSKCallBack(&pSK[ucI].ucSKEvent);
				}else{/*按键是一直按下的,但没到连击起始值*/
					;
				}
			}break;
			
		}
	}
}
