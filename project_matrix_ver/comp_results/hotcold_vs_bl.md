### Baseline

- SOA(Struct of Array) data structure
```
struct Tables
{
       std::vector< std::vector< Count > > visited_matrix; // 1: visited, 0: unvisited
       std::vector< std::vector< Entry > > ij_entries_matrix; // NxN matrix of (i, j)-entries
       std::vector< Score > scores;
       std::vector< Count > num_entries;
};
```
 - Result
```
>> sudo perf stat -d ./pr_baseline_Og
s=1
Calculation time = 1013247 us

 Performance counter stats for './pr_baseline_Og':

          2,561.87 msec task-clock                #    1.000 CPUs utilized          
                 8      context-switches          #    0.003 K/sec                  
                 0      cpu-migrations            #    0.000 K/sec                  
           320,312      page-faults               #    0.125 M/sec                  
    10,085,505,223      cycles                    #    3.937 GHz                      (50.04%)
    21,959,565,453      instructions              #    2.18  insn per cycle           (62.53%)
     3,122,825,824      branches                  # 1218.963 M/sec                    (62.53%)
        24,783,482      branch-misses             #    0.79% of all branches          (62.53%)
     7,052,309,641      L1-dcache-loads           # 2752.798 M/sec                    (62.10%)
       440,905,330      L1-dcache-load-misses     #    6.25% of all L1-dcache hits    (24.98%)
       122,263,704      LLC-loads                 #   47.724 M/sec                    (24.98%)
        11,496,229      LLC-load-misses           #    9.40% of all LL-cache hits     (37.47%)

       2.562389886 seconds time elapsed

       2.194464000 seconds user
       0.367742000 seconds sys
```

### Separating hot and cold data

 - SOAOS(Struct of Array of Structs) data structure
```
/* data accessed during the pagerank algorithm */
struct Tables_Hot
{
       std::vector< Entry > entries_col;
       Score score;
};
/* data used in prep stage */
struct Tables_Cold
{
       std::vector< Count > visited_col;
       Count num_entry;
};
/* final matrix table consists of hot and cold data */
struct Matrix_soa
{
       std::vector< Tables_Hot > hot;
       std::vector< Tables_Cold > cold;
};
```
 - Result
```
sudo perf stat -d ./pr_hot_cold_Og
s=1
Calculation time = 964481 us

 Performance counter stats for './pr_hot_cold_Og':

          2,454.98 msec task-clock                #    1.000 CPUs utilized          
                 6      context-switches          #    0.002 K/sec                  
                 0      cpu-migrations            #    0.000 K/sec                  
           315,615      page-faults               #    0.129 M/sec                  
     9,737,513,724      cycles                    #    3.966 GHz                      (49.82%)
    20,853,006,290      instructions              #    2.14  insn per cycle           (62.36%)
     3,111,980,292      branches                  # 1267.622 M/sec                    (62.39%)
        29,190,641      branch-misses             #    0.94% of all branches          (62.56%)
     6,977,755,404      L1-dcache-loads           # 2842.291 M/sec                    (62.24%)
       452,169,726      L1-dcache-load-misses     #    6.48% of all L1-dcache hits    (25.06%)
       117,309,126      LLC-loads                 #   47.784 M/sec                    (24.90%)
        11,646,896      LLC-load-misses           #    9.93% of all LL-cache hits     (37.28%)

       2.455433816 seconds time elapsed

       2.163396000 seconds user
       0.291918000 seconds sys
```

### Comparison and Implementation Decision

Theoretically, by separating data for preprocessing from data actually used during applying algorithm, it should take almost half of the memory size compares to baseline implementation because it only takes one Table struct with half size of the data every time. However, from the linux-perf result, our implementation with hot and cold data separated didn't give us a dramatically reduced in L1-dcache-loads or our cache miss rate on 10000-nodes test. Base on the results shown above, we choose to use the data structure defined in baseline implementation.