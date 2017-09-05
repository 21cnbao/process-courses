# 编译two-loops.c, gcc two-loops.c -pthread，运行两份

    baohua@baohua-VirtualBox:~/develop/training/process-courses/day3$ gcc two-loops.c -pthread
    baohua@baohua-VirtualBox:~/develop/training/process-courses/day3$ ./a.out &
    [1] 13682
    baohua@baohua-VirtualBox:~/develop/training/process-courses/day3$ main pid:13682, tid:3075434240
    thread pid:13682, tid:3067038528
    thread pid:13682, tid:3075431232

    baohua@baohua-VirtualBox:~/develop/training/process-courses/day3$ ./a.out &
    [2] 13685
    baohua@baohua-VirtualBox:~/develop/training/process-courses/day3$ main pid:13685, tid:3075925760
    thread pid:13685, tid:3067530048
    thread pid:13685, tid:3075922752
<br> top命令观察CPU利用率：
    13682 baohua    20   0   18684    616    552 S  98.4  0.0   1:12.09 a.out                                                                                                    
    13685 baohua    20   0   18684    644    580 S  98.1  0.0   1:07.32 a.out   
    
    
