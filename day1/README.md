# 编译运行fork1.c 看打印几个hello，分析为什么；

# 编译运行fork2.c 看打印几个a, b, c，分析为什么；

# 编译运行life_period.c，
## 在if 1改为if 0的情况下，

     21 #if 1 /* define 1 to make child process always a zomie */
     22                 printf("ppid:%d\n", getpid());
     23                 while(1);
     24 #endif
杀死子进程，观察父进程能监控到子进程死亡原因。

## 在if 1不改为if 0的情况下，
杀死子进程，观察父进程的僵尸态。

# cpulimit的用法:
编译cpulimit.c这个死循环程序，gcc cpulimit.c，运行之./a.out，通过pidof a.out命令获得它的pid，然后开一个terminal观察top，发现a.out这个进程的CPU利用率接近100%，之后通过cpulimit控制它的CPU利用率，再看top
* cpulimit -l 20 -p  a.out的pid
* cpulimit -l 30 -p  a.out的pid
...
 
