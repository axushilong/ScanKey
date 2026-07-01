/**
 * @file    skfaPort.h
 * @author  axushilong@163.com
 * @version v0.0.1
 * @date    2026.04.27
 * @brief   skfa按键硬件底层驱动程序接口文件
 */
#ifndef __SKFA_H__
#define __SKFA_H__

#ifdef __cplusplus
extern "C"{
#endif

/* 按键连接类型：
1.一个IO口连接一个按键，按键按下时IO口电平为0
2.一个IO口连接一个按键，按键按下时IO口电平为1
3.按键矩阵连接，行列交叉连接按键，按键按下时对应行列的IO口电平一致
// 4.其他连接方式，按键按下时IO口电平变化规律自定义
*/

/*
1.独立按键
2.矩阵按键
3.组合按键
*/






typedef struct{
    uint8_t u8Enable;
    uint8_t u8Count;
    const struct ;
    
}stSkfaTypes, *pstSkfaTypes;

void vSkfaPortInit(const stSkfaTypes *pstSkfaTypes);



#ifdef __cplusplus
}
#endif

#endif

