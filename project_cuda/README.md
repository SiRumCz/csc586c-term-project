## CUDA GPU Multi-threaded Implementation

Usage:

Baseline

	$ g++ -O3 -Wall -std=c++17 main_baseline.cpp -o baseline
  
	$ ./baseline
  
GPU (with CUDA environment configured)

	$ nvcc -O3 main_cuda.cu
  
	$ ./a.out

---

Four sample datasets has been provided in this directory, the runtimes of executing on these datasets are recorded in results.png.

The parameters are **hard-coded** as gobal variables in the scripts, where:

* N is the number of nodes in the input edge list, must be changed when a different input is used;

* filename is the path of the input file;

* num_iter is the number of iterations in the PageRank algorithm;

* d is the damping factor;

* blocksize is the number of threads per block.

Not that other inputs must have the same format as these four: the nodes should be labeled as integers, and range from 0 to N-1.

It is not recommended to run the programs on datasets with more nodes than which in the examples, as the variable may be overflowed when calculating N x N. 
