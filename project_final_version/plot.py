import os
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patheffects as pe

# read results from file
n = None
mode = None
filenames = []
num_nodes = []
update_time = {"bl": [], "cpu": [], "gpu": []}
calc_time = {"bl": [], "cpu": [], "gpu": []}
total_time = {"bl": [], "cpu": [], "gpu": []}

with open("results.txt", "r") as f:
	first = True
	for line in f:
		line = line.strip()
		if first:
			n = int(line)
			first = False
		else:
			line = line.split()
			if len(line) == 1:
				mode = line[0]
			else:
				if len(filenames) < n:
					filenames += [line[0]]
					num_nodes += [int(line[1])]
				update_time[mode] += [int(line[2])]
				calc_time[mode] += [int(line[3])]
				total_time[mode] += [int(line[4])]

print(num_nodes)
os.system("mkdir plots")

# plot runtimes (update, pagerank, total) for each mode
for mode in ["bl", "cpu", "gpu"]:
	plt.figure()
	plt.scatter(num_nodes, update_time[mode])
	plt.plot(num_nodes,update_time[mode], label="Entry Update Time")
	plt.scatter(num_nodes, calc_time[mode])
	plt.plot(num_nodes, calc_time[mode], label="PR Calculation Time")
	plt.scatter(num_nodes, total_time[mode])
	plt.plot(num_nodes, total_time[mode], label="Total Time")
	plt.xlabel("Number of Nodes")
	plt.ylabel("Time (microseconds)")
	plt.legend()
	plt.savefig("plots/{}.png".format(mode))


# plot total runtimes for all the three modes in one graph
X = np.arange(n)
cpu_spdup = [total_time["bl"][i]/total_time["cpu"][i] for i in range(n)]
gpu_spdup = [total_time["bl"][i]/total_time["gpu"][i] for i in range(n)]
plt.figure()

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

plt.savefig("plots/compare_modes.png")