'''
Because the filename and number of nodes are defined as static variables
in our C++ scripts, and we also defined types using theses variables, so
we must hard-code theses variables in the scripts. We replace these variables
in our templates and create scripts, then run the scripts and extract results
from the outputs.
'''

import os
import subprocess

# configurations
modes = ["bl", "cpu", "gpu"]
compile_cmd = {"bl": "g++ -O3 -std=c++17 {}",
				"cpu": "g++ -O3 -std=c++17 -fopenmp {}",
				"gpu": "nvcc -O3 {}"}
template = {"bl": "baseline_cpu_tmpl",
			"cpu": "baseline_cpu_tmpl",
			"gpu": "gpu_tmpl"}
outfile = {"bl": "temp.cpp",
			"cpu": "temp.cpp",
			"gpu": "temp.cu"}
path = "../tests/"
# [(filename, num_nodes)]
tests = [("p2p-Gnutella08.txt", "6301"),
		("p2p-Gnutella05.txt", "8846"),
		("p2p-Gnutella04.txt", "10876"),
		("p2p-Gnutella25.txt", "22687"),
		("p2p-Gnutella24.txt", "26518"),
		("p2p-Gnutella30.txt", "36682")]

# results
pr_time = {"bl": [], "cpu": [], "gpu": []}
total_time = {"bl": [], "cpu": [], "gpu": []}


# run scripts for each mode for each test input and get results
for mode in modes:
	print("Mode", mode + "\n")
	infile_name = "templates/" + template[mode] # template
	for filename, N in tests:
		print("Test", filename)
		with open(infile_name, "r") as f_in:
			code = f_in.read()
			code = code.replace("{{filename}}", path + filename)
			code = code.replace("{{N}}", N)
			with open(outfile[mode], "w") as f_o:
				f_o.write(code)
		# compile
		os.system(compile_cmd[mode].format(outfile[mode]))
		# run
		out = subprocess.Popen(["./a.out"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		stdout, stderr = out.communicate()
		# extract output
		stdout = stdout.decode("utf-8")
		print(stdout)
		stdout = stdout.split("\n")
		pr_time[mode] += [int(stdout[2].split(" ")[-2])] # "pagerank_time = ? us"
		total_time[mode] += [int(stdout[3].split(" ")[-2])] # "total_time = ? us"

# clear up
os.system("rm -f temp.cpp")
os.system("rm -f temp.cu")
os.system("rm -f a.out")

# store results to a text file
with open("results.txt", "w") as f:
	n = len(tests) # number of tests
	f.write(str(n) + "\n")
	for mode in modes:
		f.write(mode + "\n")
		for i in range(n):
			f.write(tests[i][0] + " ")
			f.write(tests[i][1] + " ")
			f.write(str(pr_time[mode][i]) + " ")
			f.write(str(total_time[mode][i]) + "\n")
	