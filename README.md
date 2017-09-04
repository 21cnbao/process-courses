# 打通Linux脉络系列：进程、线程和调度

本课程分成4个组成部分，每次课60分钟。每次课后会留下3~4个练习题，可以在微信群或者Linuxer公众号留言讨论答案和做题心得。

* 第一部分深入彻底地搞清楚进程生命周期，进程生命周期创建、退出、停止，以及僵尸是个什么意思；
* 第二部分，深入分析进程创建的写时拷贝技术、以及Linux的线程究竟是怎么回事（为什么称为轻量级进程），此部分也会搞清楚进程0、进程1和托孤，以及睡眠时的等待队列；
* 第三部分，搞清楚Linux进程调度算法，不同的调度策略、实时性，完全公平调度算法；
* 第四部分，讲解Linux多核下CPU、中断、软负载均衡，cgroups调度算法以及Linux为什么不是一个实时操作系统。

## 9月13日、9月15日、9月19日、9月22日晚20点开始

# 第一部分大纲

* Linux进程生命周期(就绪、运行、睡眠、停止、僵死)
* 僵尸是个什么鬼？
* 停止状态与作业控制，cpulimit
* 内存泄漏的真实含义
* task_struct以及task_struct之间的关系
* 初见fork和僵尸
## 练习题
* fork的例子
* life-period例子，观察僵尸
* 用cpulimit控制CPU利用率
 

# 第二部分大纲

* fork、vfork、clone
* 写时拷贝技术
* Linux线程的实现本质
* 进程0和进程1
* 进程的睡眠和等待队列
* 孤儿进程的托孤，SUBREAPER
## 练习题
* fork、vfork、Copy-on-Write例子
* life-period例子，实验体会托孤
* pthread_create例子，strace它
* 彻底看懂等待队列的案例

# 第三部分大纲

* CPU/IO消耗型进程
* 吞吐率 vs. 响应
* SCHED_FIFO、SCHED_RR
* SCHED_NORMAL和CFS
* nice、renice
* chrt
## 练习题
* 运行2个高CPU利用率程序，调整他们的nice
* 用chrt把一个死循环程序调整为SCHED_FIFO
* 阅读ARM的big.LITTLE架构资料，并论述为什么ARM要这么做？

# 第四部分大纲

* 多核下负载均衡
* 中断负载均衡、RPS软中断负载均衡
* cgroups和CPU资源分群分配
* Android和NEON对cgroups的采用
* Linux为什么不是硬实时的
* preempt-rt对Linux实时性的改造
## 练习题
* 用time命令跑1个含有2个死循环线程的进程
* 用taskset调整多线程依附的CPU
* 创建和分群CPU的cgroup，调整权重和quota
* cyclictest
