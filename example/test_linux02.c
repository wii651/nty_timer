/***************************************************************
 * 
 * @file:    test_linux02.c
 * @author:  wilson
 * @version: 1.0
 * @date:    2021-08-16 15:27:24
 * @license: MIT
 * @brief:   多线程测试例
 * 编译:
 * 		gcc test_linux02.c  nty_timer.c  ngx_rbtree.c  -pthread  -o   test_linux02
 * 使用:
 * 		./test_linux02
 * 注意:
 * 		由于定时器并非线程安全, 因此多线程环境下, 每个线程需单独持有一个Timer对象
 * 
 ***************************************************************/

#include "nty_timer.h"
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

//[ 定时器回调事件01 ]
void cb01(TimerTask* task) {
	int id = *((int*)task->func_arg);
	time_t rawtime;
	time(&rawtime);
	struct tm* now = localtime(&rawtime);
	char time_str[128] = {0};
	strftime(time_str, 128, "%H:%M:%S", now);


	printf("task01[%d],  rest_loop[%d],  interval[%.1lfs], now[%s]\n", id, task->rest_loop_count, (double)task->interval / 1000, time_str);
	return;
}

//[ 定时器回调事件02 ]
void cb02(TimerTask* task) {
	int id = *((int*)task->func_arg);
	
	time_t rawtime;
	time(&rawtime);

	struct tm* now = localtime(&rawtime);
	char time_str[128] = {0};
	strftime(time_str, 128, "%H:%M:%S", now);


	printf("task02[%d],  rest_loop[%d],  interval[%.1lfs], now[%s]\n", id, task->rest_loop_count, (double)task->interval / 1000, time_str);
	return;
}

//[ 定时器自动结束主循环事件 ]
void quit_cb(TimerTask* task) {
	*(int*)(task->func_arg) = 1;
}

void* thread_routine(void* arg) {
	int thread_id = *((int*)arg);
	int quit_flag = 0;
	//[ 初始化定时器 ]
	Timer* timer = nty_InitTimer();

	int id[3] = {thread_id * 10 + 1, thread_id * 10 + 2, thread_id * 10 + 3};
	//[ 创建5个定时任务 ]
	TimerTask* task01 = nty_CreateTimerTask(cb01, (void*)&id[0], 1000, 5);
	TimerTask* task02 = nty_CreateTimerTask(cb01, (void*)&id[1], 2000, 3);
	TimerTask* task03 = nty_CreateTimerTask(cb02, (void*)&id[2], 3000, -1);
	TimerTask* task04 = nty_CreateTimerTask(quit_cb, (void*)&quit_flag, 20000, 1);
	
	//[ 将定时任务添加到定时器中 ]
	nty_AddTimerTask(timer, task01);
	nty_AddTimerTask(timer, task02);
	nty_AddTimerTask(timer, task03);
	nty_AddTimerTask(timer, task04);

	while (!quit_flag) {
		//[ 每隔10ms, 刷新一下计时, 看下是否有定时任务需要触发 ]
		nty_TimerTicks(timer);
		usleep(10000); //[ 时间精度10ms ]
	}
	printf("quitted...\n");

	//[ 销毁定时器及其中所有任务 ]
	nty_DestroyTimer(timer);
}


int main() {
	pthread_t pid[2];
	int thread_id[2] = {1, 2};
	for (int i = 0; i < 2; i++) {
		pthread_create(&pid[i], NULL, thread_routine, (void*)&thread_id[i]);
	}
	for (int i = 0; i < 2; i++) {
		pthread_join(pid[i], NULL);
	}
	return 0;
}