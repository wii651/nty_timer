/***************************************************************
 * 
 * @file:    nty_timer.h
 * @author:  wilson
 * @version: 1.0
 * @date:    2021-08-21 13:54:25
 * @license: MIT
 * @brief:   
 * (1) 计时器(Timer)管理多个计时任务(TimerTask), 
 * 每个计时任务(TimerTask)拥有独立的计时周期, 周期循环次数和回调函数
 * (2) 采用红黑树维护计时任务结点
 * (3) Thread unsafety. 因此多线程编程时, 让每个线程独自持有一份Timer对象
 * (4) 支持linux\Windows\MacOS平台.
 * 其中MacOS_10.12之前版本时间精度为微妙级,
 * 而MacOS_10.12以后\linux\Windows平台下, 时间精度均可达纳秒级
 * 
 ***************************************************************/

#ifndef _NTY_TIMER_H_ 
#define _NTY_TIMER_H_ 

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "ngx_rbtree.h"

#ifdef __cpluscplus
extern "C" {
#endif

typedef ngx_rbtree_node_t	TimerTaskNode;

typedef struct TimerTask {
	void (*cb_func)(struct TimerTask*);		//[ 触发事件回调函数 ]
	void* func_arg;							//[ 回调函数补充传参 ]
	uint32_t next_trigger_time;				//[ 下次触发的时间点 ]
	uint32_t interval;						//[ 触发周期 ]
	int32_t rest_loop_count;				//[ 剩余触发周期次数 ]
} TimerTask;


typedef struct Timer {
	ngx_rbtree_t tasktree;					//[ 计时任务红黑树 ]
	ngx_rbtree_node_t sentinel;				//[ 红黑树哨兵节点 ]
	uint32_t tree_size;						//[ 红黑树节点数 ]
} Timer;

/* ============================ API ============================ */


/**
 * @brief 初始化计时器, 获取其句柄
 * 
 * @return Timer* -- 成功返回计时器句柄, 失败返回NULL
 */
Timer* nty_InitTimer();

/**
 * @brief 构造计时器任务
 * 
 * @param cb_func[in] 计时任务触发回调函数
 * @param cb_func_arg[in] 回调函数传参
 * @param interval[in] 计时任务触发周期(ms)
 * @param loop_count[in] 计时任务周期的循环次数. loop_count为正数时, 表示执行loop_count次循环以后, 计时任务自动销毁. loop_count为负数时, 表示endless-loop. loop_count为0时, 循环将执行1次后销毁.
 * @return TimerTask* -- 成功返回计时任务句柄, 失败返回NULL;
 */
TimerTask* nty_CreateTimerTask(void (*cb_func)(struct TimerTask*), void* cb_func_arg, uint32_t interval, int32_t loop_count);

/**
 * @brief 向计时器中添加计时任务
 * 
 * @param timer[in] 计时器句柄
 * @param task[in] 计时任务句柄
 * @return int -- 0 on success, -1 otherwise
 */
int nty_AddTimerTask(Timer* timer, TimerTask* task);

/**
 * @brief 获取计时器中的计时任务数量
 * 
 * @param timer[in] 计时器句柄
 * @return int -- 执行正确返回任务数量, -1表示执行错误
 */
int nty_GetNumTimerTask(Timer* timer);

/**
 * @brief 清空计时器中所有计时任务, 并释放计时器本身
 * 
 * @return int -- 0 on success, -1 otherwise
 */
int nty_DestroyTimer(Timer* timer);

/**
 * @brief 获取最接近触发的Timertask的触发事件
 * 
 * @return uint32_t -- 触发事件
 */
uint32_t nty_GetNearestTriggerTime(Timer* timer);

/**
 * @brief 更新计时器时间戳
 * 
 * @param timer[in] 计时器句柄
 */
void nty_TimerTicks(Timer* timer);

/**
 * @brief 获取当前时间戳(ms)
 * 
 * @return uint32_t -- 当前时间戳
 */
uint32_t nty_GetCurrentTime();

#ifdef __cpluscplus
}
#endif

#endif	// _NTY_TIMER_H_
