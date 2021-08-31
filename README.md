# nty_timer

一款简单实用的跨平台计时器组件

* 可创建多个计时任务(TimerTask), 可以设定每个计时任务的触发事件\触发周期\触发次数. 将计时任务(TimerTask)添加到计时器(Timer)中统一执行管理.

* 采用红黑树维护计时器(Timer)中的计时任务(TimerTask)队列.

* 计时器(Timer)对象非线程安全. 因此多线程环境时, 保证每个线程单独持有一个计时器(Timer)对象.

* 支持Linux\Windows\MacOS平台.

  其中MacOS_10.12之前版本时间精度为微秒级,

  而MacOS_10.12之后版本\Linux\Windows平台下, 时间精度均可达纳秒级



##  quick start

```c
/* quick_start.c */
#include "nty_timer.h"
#include <stdlib.h>
#include <unistd.h>

void timeout_cb(TimerTask* task) {
    // 触发事件内容
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
        nty_TimerTicks(timer);
        usleep(10000);
    }
    
    // ...
    
    // 销毁计时器及其中未完成的计时任务
    nty_DestroyTimer(timer);
    return 0;
}

```

编译:

```
gcc quick_start.c nty_timer.c ngx_rbtree.c -o quick_start
```

## 测试用例(linux平台)

单线程测试例 test_linux01.c

```
gcc test_linux01.c nty_timer.c ngx_rbtree.c -o test_linux01
```

多线程测试例 test_linux02.c

```
gcc test_linux02.c nty_timer.c ngx_rbtree.c -pthread -o test_linux02
```
