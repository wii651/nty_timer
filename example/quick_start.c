/***************************************************************
 * 
 * @file:    quick_start.c
 * @author:  wilson
 * @version: 1.0
 * @date:    2021-08-21 14:01:02
 * @license: MIT
 * @brief:   
 * 编译:
 * 	gcc quick_start.c nty_timer.c ngx_rbtree.c -o quick_start
 ***************************************************************/

/* quick_start.c */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "nty_timer.h"

void timeout_cb(TimerTask* task) {
    // 触发事件内容
	printf("cb executing...\n");
}

int main() {
    // 初始化计时器
    Timer* timer = nty_InitTimer();
    // 创建计时任务1, 执行周期1000ms, 执行5次后销毁
    TimerTask* task01 = nty_CreateTimerTask(timeout_cb, NULL, 1000, 5);     
    // 创建计时任务2, 执行周期2000ms, 执行1次后销毁
    TimerTask* task02 = nty_CreateTimerTask(timeout_cb, NULL, 2000, 1);
    // 将计时任务添加到计时器中
    nty_AddTimerTask(timer, task01);
    nty_AddTimerTask(timer, task02);
    
    for (;;) {
        // 每隔10ms, 刷新计时器状态, 看下是否有计时任务需要触发
		// uint32_t timeout = nty_GetNearestTriggerTime(timer) - nty_GetCurrentTime();
		// printf("%d\n", timeout);
        nty_TimerTicks(timer);
        usleep(10000);
    }
    
    // ...
    
    // 销毁计时器及其中未完成的计时任务
    nty_DestroyTimer(timer);
    return 0;
}