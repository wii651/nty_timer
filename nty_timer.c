/***************************************************************
 * 
 * @file:    nty_timer.c
 * @author:  wilson
 * @version: 1.0
 * @date:    2021-08-24 14:54:41
 * @license: MIT
 * @brief:   计时器组件
 * 
 ***************************************************************/
#include "nty_timer.h"

#ifdef __cpluscplus
extern "C" {
#endif

Timer* nty_InitTimer() {
	Timer* timer = (Timer*)calloc(1, sizeof(Timer));
	if (timer == NULL) {
		return NULL;
	}
	timer->tree_size = 0;
	ngx_rbtree_init(&timer->tasktree, &timer->sentinel, ngx_rbtree_insert_timer_value);

	return timer;
}

TimerTask* nty_CreateTimerTask(void (*cb_func)(struct TimerTask*), void* cb_func_arg, uint32_t interval, int32_t loop_count) {
	TimerTask* task = (TimerTask*)calloc(1, sizeof(TimerTask));
	if (task == NULL) {
		return NULL;
	}
	task->cb_func = cb_func;
	task->func_arg = cb_func_arg;
	task->interval = interval;
	task->rest_loop_count = loop_count;
	task->next_trigger_time = interval + nty_GetCurrentTime();
	return task;
}

int nty_AddTimerTask(Timer* timer, TimerTask* task) {

	TimerTaskNode* node = (TimerTaskNode*)calloc(1, sizeof(TimerTaskNode));
	node->key = task->next_trigger_time;
	node->data = (void*)task;
	
	ngx_rbtree_insert(&timer->tasktree, node);

	timer->tree_size++;
	return 0;
}

int nty_GetNumTimerTask(Timer* timer) {
	return timer->tree_size;
}

int nty_DestroyTimer(Timer* timer) {

	if (timer->tasktree.root != &timer->sentinel) {
		TimerTaskNode* tmpNode = ngx_rbtree_min(timer->tasktree.root, &timer->sentinel);
		while (tmpNode) {
			TimerTaskNode* nextNode = ngx_rbtree_next(&timer->tasktree, tmpNode);
			free(tmpNode->data);
			ngx_rbtree_delete(&timer->tasktree, tmpNode);
			free(tmpNode);
			tmpNode = nextNode;
		}
	}
	free(timer);
}

void nty_TimerTicks(Timer* timer) {
	uint32_t now = nty_GetCurrentTime();
	//[ 遍历每个节点, 是否需要被触发 ]
	while (timer->tree_size) {
		TimerTaskNode* minNode = ngx_rbtree_min(timer->tasktree.root, &timer->sentinel);
		TimerTask* task = (TimerTask*)minNode->data;
		if (now >= task->next_trigger_time) {
			task->cb_func(task);
			if (task->rest_loop_count == 1 || task->rest_loop_count == 0) {	//[ 执行完了最后一次, 可以删除了 ]
				free(minNode->data);
				ngx_rbtree_delete(&timer->tasktree, minNode);
				free(minNode);
				timer->tree_size--;
			} else {	//[ 重新插入 ]
				ngx_rbtree_delete(&timer->tasktree, minNode);
				free(minNode);
				timer->tree_size--;

				task->next_trigger_time += task->interval;
				if (task->rest_loop_count > 1)	//[ rest_loop_count < 0时表示endless-loop, 不用-- ]
					task->rest_loop_count--;
				nty_AddTimerTask(timer, task);
			}
		} else {
			break;
		}
	}
}

uint32_t nty_GetNearestTriggerTime(Timer* timer) {
	TimerTaskNode* minNode = ngx_rbtree_min(timer->tasktree.root, &timer->sentinel);
	return ((TimerTask*)minNode->data)->next_trigger_time;
}


uint32_t nty_GetCurrentTime() {
	uint32_t time_ms;
#if defined(__linux__) || defined(AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER)
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);	//[ 精度最高的计时 ]
	time_ms = (uint32_t)ts.tv_sec * 1000;	//[ 1s = 1000ms ]
	time_ms += ts.tv_nsec / 1000000;		//[ 1ms = 1000000nano_second ]
#elif defined(_MSC_VER)
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);			//[ 精度最高的计时 ]
	time_ms = (uint32_t)ts.tv_sec * 1000;	//[ 1s = 1000ms ]
	time_ms += ts.tv_nsec / 1000000;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	t = (uint32_t)tv.tv_sec * 1000;
	t += tv.tv_usec / 1000;
#endif
	return time_ms;
}

#ifdef __cpluscplus
}
#endif