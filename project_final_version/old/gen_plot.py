'''
Because the filename and number of nodes are defined as static variables
in our C++ scripts, and we also defined types using theses variables, so
we must hard-code theses variables in the scripts. We replace these variables
in our templates and create scripts, then run the scripts and extract results
from the outputs.
'''

import os
import subprocess
import matplotlib.pyplot as plt
import matplotlib.patheffects as pe

# configurations
modes = ["bl", ""]
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
		("erdos-10000.txt", "10000"),
		("p2p-Gnutella04.txt", "10876"),
		("erdos-20000.txt", "20000"),
		("p2p-Gnutella25.txt", "22687"),
		("erdos-50000.txt", "50000")]

# results
num_nodes = []
update_time = {"bl": [], "cpu": [], "gpu": []}
calc_time = {"bl": [], "cpu": [], "gpu": []}
total_time = {"bl": [], "cpu": [], "gpu": []}


# run scripts for each mode for each test input and get results
for mode in modes:
	print("Mode", mode)
	infile_name = "templates/" + template[mode] # template
	for filename, N in tests:
		print("	Test", filename)
		num_nodes += [int(N)]
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
		update_time[mode] += [int(stdout[2].split(" ")[-2])] # "update_time = ? us"
		calc_time[mode] += [int(stdout[3].split(" ")[-2])] # "pagerank_time = ? us"
		total_time[mode] += [int(stdout[4].split(" ")[-2])] # "total_time = ? us"

# clear up
os.system("rm -f temp.cpp")
os.system("rm -f temp.cu")
os.system("rm -f a.out")

os.system("mkdir results")
# plot runtimes (update, pagerank, total) for each mode
for mode in modes:
	plt.scatter(num_nodes, update_time[mode])
	plt.plot(num_nodes,update_time[mode], label="Entry Update Time")
	plt.scatter(num_nodes, calc_time[mode])
	plt.plot(num_nodes, calc_time[mode], label="PR Calculation Time")
	plt.scatter(num_nodes, total_time[mode])
	plt.plot(num_nodes, total_time[mode], label="Total Time")
	plt.xlabel("Number of Nodes")
	plt.ylabel("Time (microseconds)")
	plt.legend()
	plt.savefig("results/{}.png".format(mode))


# plot total runtimes for all the three modes in one graph
n = len(num_nodes) # number of tests
X = list(range(n))
cpu_spdup = [total_time["bl"][i]/total_time["cpu"][i] for i in range(n)]
gpu_spdup = [total_time["bl"][i]/total_time["gpu"][i] for i in range(n)]

fig, ax1 = plt.subplots()
ax1.bar(X + 0.25, total_time["bl"], color='#95E1EF', width=0.25, label = "Baseline")
ax1.bar(X, total_time["cpu"], color='#FEDBC6', width=0.25, label = "CPU Parallelism")
ax1.bar(X - 0.25, total_time["gpu"], color='#881600', width=0.25, label="GPGPU Parallelism")
ax1.set_ylabel("Total Runtime (microseconds)")
ax1.legend()

ax2 = ax1.twinx()
ax2.plot(cpu_spdup, label="CPU Speedup", color="#FEDBC6", path_effects=[pe.Stroke(linewidth=5, foreground='w'), pe.Normal()])
ax2.plot(gpu_spdup, label="GPUPU Speedup", color="#881600", path_effects=[pe.Stroke(linewidth=5, foreground='w'), pe.Normal()])
ax2.set_ylabel("Speedup (times)")
ax2.legend()

plt.xlabel("Dataset #")
plt.savefig("results/compare_modes.png")
