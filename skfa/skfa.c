/**
 * @file    skfa.c
 * @author  axushilong@163.com
 * @version v0.0.1
 * @date    2026.04.27
 * @brief   skfa按键驱动程序文件
 */
#include <stdint.h>

#define KEY_EVENT_CLICK_ONLY_SUPPORT 1





static void key_driver_scan(void *_scan_para){
    struct key_driver_para *scan_para = (struct key_driver_para *)_scan_para;
    u8 KeyEvent = 0;
    u8 ucNowKeyIndex = NO_KEY;
    u8 KeyIndex = 0;
    struct sys_event e;
    static u8 poweron_cnt = 0;
    ucNowKeyIndex = scan_para->get_value();
    if (ucNowKeyIndex != NO_KEY) {
        is_key_active = 35;      //35*10Ms
    } else if (is_key_active) {
        is_key_active --;
    }
//===== 按键消抖处理
    if (ucNowKeyIndex != scan_para->ucFilterKeyIndex && scan_para->filter_time) {	//当前按键值与上一次按键值如果不相等, 重新消抖处理, 注意filter_time != 0;
        scan_para->filter_cnt = 0; 		//消抖次数清0, 重新开始消抖
        scan_para->ucFilterKeyIndex = ucNowKeyIndex;	//记录上一次的按键值
        return; 		//第一次检测, 返回不做处理
    } 		//当前按键值与上一次按键值相等, filter_cnt开始累加;
    if (scan_para->filter_cnt < scan_para->filter_time) {
        scan_para->filter_cnt++;
        return;
    }
//===== 按键消抖结束, 开始判断按键类型(单击, 双击, 长按, 多击, HOLD, (长按/HOLD)抬起)
    if /*按键序号保持*/(ucNowKeyIndex == scan_para->ucLastKeyIndex) {
        if /*保持抬起*/(ucNowKeyIndex == NO_KEY) {
            if (scan_para->click_cnt > 0) {  //有按键需要消息需要处理
#if KEY_EVENT_CLICK_ONLY_SUPPORT 	//是否支持某些按键只响应单击事件
                if (scan_para->notify_value & BIT(7)) {  //BIT(7)按键特殊处理标志, 只发送单击事件, 也可以用于其它扩展
                    KeyEvent = KEY_EVENT_CLICK;  //单击
                    KeyIndex = scan_para->notify_value;
                    goto _notify;
                }
#endif
                if (scan_para->click_delay_cnt > scan_para->click_delay_time) { //按键被抬起后延时到
                    //TODO: 在此可以添加任意多击事件
                    if (scan_para->click_cnt >= 5) {
                        KeyEvent = KEY_EVENT_FIRTH_CLICK;  //五击
                    } else if (scan_para->click_cnt >= 4) {
                        KeyEvent = KEY_EVENT_FOURTH_CLICK;  //4击
                    } else if (scan_para->click_cnt >= 3) {
                        KeyEvent = KEY_EVENT_TRIPLE_CLICK;  //三击
                    } else if (scan_para->click_cnt >= 2) {
                        KeyEvent = KEY_EVENT_DOUBLE_CLICK;  //双击
                    } else {
                        KeyEvent = KEY_EVENT_CLICK;  //单击
                    }
                    KeyIndex = scan_para->notify_value;
                    goto _notify;
                } else {	//按键抬起后等待下次延时时间未到
                    scan_para->click_delay_cnt++;
                    goto _scan_end; //按键抬起后延时时间未到, 返回
                }
            } else {
                goto _scan_end;  //没有按键需要处理
            }
        } else /*保持按下*/{
            scan_para->press_cnt++;
            if (scan_para->press_cnt == scan_para->long_time){
                KeyEvent = KEY_EVENT_LONG;
            } else if (scan_para->press_cnt == scan_para->hold_time) {
                KeyEvent = KEY_EVENT_HOLD;
                scan_para->press_cnt = scan_para->long_time;
            } else {
                goto _scan_end;  //press_cnt没到长按和HOLD次数, 返回
            }
            //press_cnt没到长按和HOLD次数, 发消息
            KeyIndex = ucNowKeyIndex;
            goto _notify;
        }
    } else /*按键序号改变*/{
        if /*按键抬起*/(ucNowKeyIndex == NO_KEY) {
            if (scan_para->press_cnt >= scan_para->long_time){  //长按/HOLD状态之后被按键抬起;
                KeyEvent = KEY_EVENT_UP;
                KeyIndex = scan_para->ucLastKeyIndex;
                goto _notify;  	//发送抬起消息
            }
            scan_para->click_delay_cnt = 1;  //按键等待下次连击延时开始
        } else /*按键按下*/{
            scan_para->press_cnt = 1;  //用于判断long和hold事件的计数器重新开始计时;
            if /*新序号按键按下*/(ucNowKeyIndex != scan_para->notify_value) {  //第一次单击/连击时按下的是不同按键, 单击次数重新开始计数
                scan_para->click_cnt = 1;
                scan_para->notify_value = ucNowKeyIndex;
            } else /*之前的序号按键再次按下*/{
                scan_para->click_cnt++;  //单击次数累加
            }
        }
        goto _scan_end;  //返回, 等待延时时间到
    }
_notify:
    KeyIndex &= ~BIT(7);  //BIT(7) 用作按键特殊处理的标志
    e.type = SYS_KEY_EVENT;
    e.u.key.init = 1;
    e.u.key.type = scan_para->key_type;//区分按键类型
    e.u.key.event = KeyEvent;
    e.u.key.value = KeyIndex;
    e.u.key.tmr = timer_get_ms();

    scan_para->click_cnt = 0;  //单击次数清0
    scan_para->notify_value = NO_KEY;

    e.arg  = (void *)DEVICE_EVENT_FROM_KEY;
    /* printf("KeyIndex: 0x%x, event: %d, key_poweron_flag: %d\n", KeyIndex, KeyEvent, key_poweron_flag); */
    if (key_poweron_flag) {
        if (KeyEvent == KEY_EVENT_UP) {
            clear_key_poweron_flag();
            return;
        }
        return;
    }
    if (key_event_remap(&e)) {
        sys_event_notify(&e);
    }
_scan_end:
    scan_para->ucLastKeyIndex = ucNowKeyIndex;
    return;
}










