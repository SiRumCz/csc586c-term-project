The source codes are in templates/, without .cpp or .cu extension.

Because the filename and number of nodes are defined as static variables in our C++ scripts, and we also defined types using the number of nodes, so we must hard-code theses variables in the scripts. By running the Python script run_tests.py, we replace these variables in our templates and create scripts, then run the scripts and extract results from the outputs.

To let each of the programs (naive, baseline, CPU and GPGPU) run the tests of 6 datasets (must in a CUDA enviornment):

    $ python3 run_tests.py
    
The results will be output to results.txt, where the first line is the number of tests (n), followed by the program mode (naive, baseline, CPU or GPGPU), then followed by n lines, and each line has 4 tokens: test input filename, number of nodes in test, algorithm runtime, totoal runtime. See results.txt for our output example.

Then, you can make plots with another Python script plot.py, which will make a directory plots/ and output 5 images for the all the 4 modes and a comparision between CPU & GPGPU to baseline:

    $ python3 plot.py
