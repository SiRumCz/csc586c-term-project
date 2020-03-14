### single thread vs multi threads
 - Single thread
```
MacOS 8-core Intel Core i9-9880h w/ 16GB RAM
zkchen@w87-170-181 project_matrix_ver % ./pr_hot_cold_Og
9134(0.000503905) 9714(0.000470566) 7994(0.000430627) 9701(0.000406934) 5645(0.000393468) 
s=1.00002
Entries update time = 564707 us
Calculation time = 4966393 us
Total time = 5531100 us
```
```
Ubuntu 4-core Intel Core i7-4790 w/ 20GB RAM
(base) zkchen@zkchen-ubuntu:~/csc586c-term-project/project_matrix_ver$ sudo perf stat -d ./pr_hot_cold_Og
9134(0.000503905) 9714(0.000470566) 7994(0.000430627) 9701(0.000406934) 5645(0.000393468) 
s=1.00002
Entries update time = 525789 us
Calculation time = 2048125 us
Total time = 2573914 us

 Performance counter stats for './pr_hot_cold_Og':

          2,847.73 msec task-clock                #    1.000 CPUs utilized          
                 6      context-switches          #    0.002 K/sec                  
                 0      cpu-migrations            #    0.000 K/sec                  
           195,742      page-faults               #    0.069 M/sec                  
    11,318,489,596      cycles                    #    3.975 GHz                      (50.00%)
    22,192,356,417      instructions              #    1.96  insn per cycle           (62.50%)
     4,921,331,239      branches                  # 1728.157 M/sec                    (62.50%)
        21,926,490      branch-misses             #    0.45% of all branches          (62.50%)
     9,661,495,659      L1-dcache-loads           # 3392.697 M/sec                    (62.13%)
       275,600,142      L1-dcache-load-misses     #    2.85% of all L1-dcache hits    (25.00%)
        78,724,638      LLC-loads                 #   27.645 M/sec                    (25.00%)
         4,020,534      LLC-load-misses           #    5.11% of all LL-cache hits     (37.50%)

       2.848108161 seconds time elapsed

       2.652078000 seconds user
       0.196005000 seconds sys
```
 - Multi threads
```
MacOS 8-core Intel Core i9-9880h w/ 16GB RAM
zkchen@w87-170-181 project_matrix_ver % ./pr_hot_cold_Og
9134(0.000503905) 9714(0.000470566) 7994(0.000430627) 9701(0.000406934) 5645(0.000393468) 
s=1.00002
Entries update time = 191099 us
Calculation time = 384625 us
Total time = 575724 us
```
```
Ubuntu 4-core Intel Core i7-4790 w/ 20GB RAM
(base) zkchen@zkchen-ubuntu:~/csc586c-term-project/project_matrix_ver$ sudo perf stat -d ./pr_hot_cold_Og
9134(0.000503905) 9714(0.000470566) 7994(0.000430627) 9701(0.000406934) 5645(0.000393468) 
s=1.00002
Entries update time = 133368 us
Calculation time = 380943 us
Total time = 514311 us

 Performance counter stats for './pr_hot_cold_Og':

          4,373.07 msec task-clock                #    5.549 CPUs utilized          
                99      context-switches          #    0.023 K/sec                  
                 1      cpu-migrations            #    0.000 K/sec                  
           195,773      page-faults               #    0.045 M/sec                  
    16,541,230,327      cycles                    #    3.783 GHz                      (49.88%)
    18,578,428,083      instructions              #    1.12  insn per cycle           (62.34%)
     1,304,554,887      branches                  #  298.316 M/sec                    (62.49%)
        31,606,824      branch-misses             #    2.42% of all branches          (62.50%)
     7,930,138,044      L1-dcache-loads           # 1813.403 M/sec                    (41.29%)
       265,064,279      L1-dcache-load-misses     #    3.34% of all L1-dcache hits    (29.18%)
        84,388,893      LLC-loads                 #   19.297 M/sec                    (25.69%)
         4,315,351      LLC-load-misses           #    5.11% of all LL-cache hits     (37.66%)

       0.788088472 seconds time elapsed

       4.149810000 seconds user
       0.224097000 seconds sys
```

### conclusion

As we can see from the running time for pagerank calculation, by utilizing 8 threads on a 4-core Intel i7-4790 Desktop Linux platform, we now have a 5x speed up from 2,000,000 us down to 400,000 us (12.9x on 8-core MacOS).