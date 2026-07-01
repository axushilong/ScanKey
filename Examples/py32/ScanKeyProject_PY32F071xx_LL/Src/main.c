/**
 * @file    main.c
 * @author  axushilong@163.com
 * @version v0.0.2
 * @date    2025.12.06
 * @brief   ScanKey示例主代码
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "ScanKeyApp.h"



/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/



/**
 * @brief  Main program.
 * @param  None
 * @retval int
 */
int main(void){
    vScanKeyProjectInit();//上电ScanKey工程数据初始化成默认值
    vBoardPowerOnInit();  //上电时钟初始化，板硬件初始化
    while (1){
        pvFuncPtr[ucPowerState]();
    }
}



/**
 * @brief  Error handling function
 * @param  None
 * @retval None
 */
void APP_ErrorHandler(void){
    /* Infinite loop */
    while (1)
    {
    }
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file：Pointer to the source file name
 * @param  line：assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line){
    /* User can add His own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* Infinite loop */
    while (1)
    {
    }
}
#endif /* USE_FULL_ASSERT */


/***************** (C) COPYRIGHT axushilong@163.com *****END OF FILE***********/
