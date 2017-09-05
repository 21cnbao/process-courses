# 1.编译two-loops.c, gcc two-loops.c -pthread，运行:
    $ time ./a.out 
    main pid:14958, tid:3075917568
    thread pid:14958, tid:3067521856
    thread pid:14958, tid:3075914560
    ^C
    
    real	1m10.050s
    user	2m20.016s
    sys	0m0.004s
* 我们得到时间分布比例，理解2个死循环被均分到2个core。

# 2.编译two-loops.c, gcc two-loops.c -pthread，运行一份
* top发现其CPU利用率接近200%
* 把它的所有线程affinity设置为01, 02, 03后分辨来看看CPU利用率

        taskset -a -p 02 进程PID
        taskset -a -p 01 进程PID
        taskset -a -p 03 进程PID
* 前两次设置后，a.out CPU利用率应该接近100%，最后一次接近200%
