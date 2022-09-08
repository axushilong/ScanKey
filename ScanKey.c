/**
 * @file    ScanKey.c
 * @author  axushilong@163.com
 * @version v0.0.1
 * @date    2022.05.16
 * @brief   ScanKey组件实现
 */
#include "ScanKeyPort.h"
//#include "CH58x_gpio.h"


#define DE_SKEventCallBack(Key) ((stScanKeyTypes *)Key)->vfpSKCallBack(((stScanKeyTypes *)Key))

//static skwide_t kwContinue = 0;  /**< continue(继续、连续)按键按下保持的信号 */
//static skwide_t kwTriger = 0;    /**< triger(触发)对应按键按下的信号,记录当前扫描是否有新的按键按下,有的话对应的位置1 */
//static skwide_t kwReset = 0;

static void vScanKeyRead(stScanKeyCtrlDataTypes * pstKeyCtrlData, stScanKeyTypes * pstSK){
	skwide_t kwReadData = kwSKPortReadKeyVal(pstSK);/**< 用于记录激活的按键数据,按键按下表示激活,对应位置1 */
	skwide_t kwChange = kwReadData ^ pstKeyCtrlData->kwContinue;/**< 记录当前和上次不同状态的按键数据,对应位为1表示此按键电平对比上次有变动 */

	pstKeyCtrlData->kwContinue = kwReadData;/**< 保存本次激活的位 */

	pstKeyCtrlData->kwTriger = kwReadData & kwChange;/**< 当前激活的位是否包含,当前激活和上次激活的不同的位;也就是上次未激活,当前激活的位 */
	pstKeyCtrlData->kwReset = kwChange & (~kwReadData);/**< 记录由按下到抬起的触发 */
}

void vScanKey(stScanKeyCtrlDataTypes * pstKeyCtrlData, stScanKeyTypes * pstSK){
	unsigned char ucI = 0;
	vScanKeyRead(pstKeyCtrlData,pstSK);
	for(ucI =0;  ucI < DE_SKCount; ucI++){
		if(pstSK[ucI].ucSKStatus){
			pstSK[ucI].usSKScanCount++;
		}
		switch(pstSK[ucI].ucSKStatus){

			case DE_SKStatusStart:{/*开始状态*/
				if(pstKeyCtrlData->kwTriger & (1<<ucI)){/*对应按键动作按下*/
					pstSK[ucI].ucDebounceTick = 1;/*开始抖动过滤检测*/
					pstSK[ucI].ucSKEvent = DE_SKEventDown0;/*按下事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else if(pstKeyCtrlData->kwContinue & (1<<ucI)){/*对应按键保持按下*/
					pstSK[ucI].ucDebounceTick++;
					if(pstSK[ucI].ucDebounceTick >= DE_SKDebounceTick){/*通过抖动验证*/
						pstSK[ucI].usSKScanCount = pstSK[ucI].ucDebounceTick;
						pstSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
						pstSK[ucI].ucSKStatus = DE_SKStatusDown0;
						pstSK[ucI].ucSKEvent = DE_SKEventDown0Yes;/*确认按下0事件*/
						DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
					}else{/*按键是按下的,但没有通过抖动事件,继续抖动验证*/
						;
					}
				}else if(pstSK[ucI].ucDebounceTick){/*按键没有按下,且经过抖动验证,说明没通过按键按键抖动验证*/
					pstSK[ucI].ucDebounceTick = 0;
					pstSK[ucI].ucSKEvent = DE_SKEventDown0No;/*按键没有按下,抖动验证不通过事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else{/*按键是初始状态,一直没动*/
					;
				}
			}break;

			case DE_SKStatusDown0:{
				if(pstKeyCtrlData->kwReset & (1<<ucI)){/*对应按键动作抬起,复位*/
					pstSK[ucI].ucDebounceTick = 1;/*开始抖动过滤检测*/
					pstSK[ucI].ucSKEvent = DE_SKEventUp0;/*抬起事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else if((~pstKeyCtrlData->kwContinue) & (1<<ucI)){/*对应按键保持抬起*/
					pstSK[ucI].ucDebounceTick++;
					if(pstSK[ucI].ucDebounceTick >= DE_SKDebounceTick){/*通过抖动验证*/
						pstSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
						pstSK[ucI].usDoubleStartTick = pstSK[ucI].usSKScanCount;/*用于双击抬起的时间判断*/
						pstSK[ucI].ucSKStatus = DE_SKStatusUp0;
						pstSK[ucI].ucSKEvent = DE_SKEventUp0Yes;/*确认抬起0事件*/
						DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
					}else{/*按键是抬起的,但没有通过抖动事件,继续抖动验证*/
						;
					}
				}else if(pstSK[ucI].ucDebounceTick){/*按键按下,且经过抖动验证,说明没通过按键按键抖动验证*/
					pstSK[ucI].ucDebounceTick = 0;
					pstSK[ucI].ucSKEvent = DE_SKEventUp0No;/*按键没有抬起,抖动过滤成功事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else if(pstSK[ucI].usSKScanCount >= De_SKLongStartTick){/*按键是一直按下的,且超过长按值*/
					pstSK[ucI].ucSKStatus = DE_SKStatusDownLong;
					pstSK[ucI].ucSKEvent = DE_SKEventDownLongYes;/*确认按下L事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else{/*按键是一直按下的,但没有超过长按值*/
					;
				}
			}break;

			case DE_SKStatusUp0:{
				if(pstKeyCtrlData->kwTriger & (1<<ucI)){/*对应按键触发按下*/
					pstSK[ucI].ucDebounceTick = 1;/*开始抖动过滤检测*/
					pstSK[ucI].ucSKEvent = DE_SKEventDown1;/*按下事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else if(pstKeyCtrlData->kwContinue & (1<<ucI)){/*对应按键保持按下*/
					pstSK[ucI].ucDebounceTick++;
					if(pstSK[ucI].ucDebounceTick >= DE_SKDebounceTick){/*通过抖动验证*/
						pstSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
						pstSK[ucI].ucSKStatus = DE_SKStatusDown1;
						pstSK[ucI].ucSKEvent = DE_SKEventDown1Yes;/*确认按下0事件*/
						DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
					}else{/*按键是按下的,但没有通过抖动事件,继续抖动验证*/
						;
					}
				}else if(pstSK[ucI].ucDebounceTick){/*按键抬起,且经过抖动验证没有通过*/
					pstSK[ucI].ucDebounceTick = 0;
					pstSK[ucI].ucSKEvent = DE_SKEventDown0No;/*按键没有按下,抖动过滤成功事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else if((pstSK[ucI].usSKScanCount - pstSK[ucI].usDoubleStartTick) >= DE_SKIntervalTick){/*按键是抬起的,一直没动*/
					pstSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
					pstSK[ucI].ucSKStatus = DE_SKStatusStart/*SKStatusUp1Yes*/;
					pstSK[ucI].ucSKEvent = DE_SKEventDoubleNo;/*确认非双击事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}
				else{/*按键是抬起的,一直没动,且抬起时间＜DE_SKClickTick*/
					;
				}
			}break;

			case DE_SKStatusDown1:{
				if(pstKeyCtrlData->kwReset & (1<<ucI)){/*对应按键触发抬起,复位*/
					pstSK[ucI].ucDebounceTick = 1;/*开始抖动过滤检测*/
					pstSK[ucI].ucSKEvent = DE_SKEventUp1;/*抬起事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else if((~pstKeyCtrlData->kwContinue) & (1<<ucI)){/*对应按键是抬起的*/
					pstSK[ucI].ucDebounceTick++;
					if(pstSK[ucI].ucDebounceTick >= DE_SKDebounceTick){/*通过抖动验证*/
						pstSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
						pstSK[ucI].ucSKStatus = DE_SKStatusStart/*SKStatusUp1Yes*/;
						pstSK[ucI].ucSKEvent = DE_SKEventUp1Yes;/*确认抬起1事件*/
						DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
					}else{/*按键是抬起的,但没有通过抖动事件,继续抖动验证*/
						;
					}
				}else if(pstSK[ucI].ucDebounceTick){/*按键按下,且经过抖动验证没有通过*/
					pstSK[ucI].ucDebounceTick = 0;
					pstSK[ucI].ucSKEvent = DE_SKEventUp1No;/*按键没有抬起,抖动过滤成功事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else{/*按键是初始状态,一直没动*/
					;
				}
			}break;

			case DE_SKStatusDownLong:{/*长按确认*/
				if(pstKeyCtrlData->kwReset & (1<<ucI)){/*对应按键触发抬起,复位*/
					pstSK[ucI].ucDebounceTick = 1;/*开始抖动过滤检测*/
					pstSK[ucI].ucSKEvent = DE_SKEventUpL;/*抬起事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else if((~pstKeyCtrlData->kwContinue) & (1<<ucI)){/*对应按键是抬起的*/
					pstSK[ucI].ucDebounceTick++;
					if(pstSK[ucI].ucDebounceTick >= DE_SKDebounceTick){/*通过抖动验证*/
						pstSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
						pstSK[ucI].ucSKStatus = DE_SKStatusStart/*SKStatusUp1Yes*/;
						pstSK[ucI].ucSKEvent = DE_SKEventUpLYes;/*确认抬起0事件*/
						DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
					}else{/*按键是抬起的,但没有通过抖动事件,继续抖动验证*/
						;
					}
				}else if(pstSK[ucI].ucDebounceTick){/*按键按下,且经过抖动验证没有通过*/
					pstSK[ucI].ucDebounceTick = 0;
					pstSK[ucI].ucSKEvent = DE_SKEventUpLNo;/*按键没有抬起,抖动过滤成功事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else if(pstSK[ucI].usSKScanCount >= De_SKContinueStartTick){/*按键是一直按下的,且超过长按值*/
					// pstSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
					pstSK[ucI].usDoubleStartTick = pstSK[ucI].usSKScanCount;/*用于双击抬起的时间判断*/
					pstSK[ucI].ucSKStatus = DE_SKStatusDownContinue;
					pstSK[ucI].ucSKEvent = DE_SKEventDownContinueYes0;/*确认按下L事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else{/*按键是一直按下的,但没到连击起始值*/
					;
				}
			}break;

			case DE_SKStatusDownContinue:{/*连击确认*/
				if(pstKeyCtrlData->kwReset & (1<<ucI)){/*对应按键触发抬起,复位*/
					pstSK[ucI].ucDebounceTick = 1;/*开始抖动过滤检测*/
					pstSK[ucI].ucSKEvent = DE_SKEventUpC;/*抬起事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else if((~pstKeyCtrlData->kwContinue) & (1<<ucI)){/*对应按键是抬起的*/
					pstSK[ucI].ucDebounceTick++;
					if(pstSK[ucI].ucDebounceTick >= DE_SKDebounceTick){/*通过抖动验证*/
						pstSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
						pstSK[ucI].ucSKStatus = DE_SKStatusStart/*SKStatusUp1Yes*/;
						pstSK[ucI].ucSKEvent = DE_SKEventUpCYes;/*确认抬起0事件*/
						DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
					}else{/*按键是抬起的,但没有通过抖动事件,继续抖动验证*/
						;
					}
				}else if(pstSK[ucI].ucDebounceTick){/*按键按下,且经过抖动验证没有通过*/
					pstSK[ucI].ucDebounceTick = 0;
					pstSK[ucI].ucSKEvent = DE_SKEventUpCNo;/*按键没有抬起,抖动过滤成功事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else if((pstSK[ucI].usSKScanCount - pstSK[ucI].usDoubleStartTick) >= De_SKContinueTick){/*按键是一直按下的,且超连击值*/
					// pstSK[ucI].ucDebounceTick = 0;/*方便确认下次有没有经过抖动验证*/
					pstSK[ucI].usDoubleStartTick = pstSK[ucI].usSKScanCount;/*用于双击抬起的时间判断*/
					// pstSK[ucI].ucSKStatus = SKStatusDownContinue;
					pstSK[ucI].ucSKEvent = DE_SKEventDownContinueYes;/*确认按下L事件*/
					DE_SKEventCallBack(&pstSK[ucI]);//pstSK[ucI].pfSKCallBack(&pstSK[ucI].ucSKEvent);
				}else{/*按键是一直按下的,但没到连击起始值*/
					;
				}
			}break;
			
		}
	}
}
