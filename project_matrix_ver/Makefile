OS := $(shell uname)

ifeq ($(OS), Darwin)
pagerank_matrix: pagerank.hpp main.cpp pagerank_hot_cold.hpp main_hot_cold.cpp
	@echo "You are running on MacOS"
	# baseline Og
	g++ -Og -Wall -std=c++17 -o pr_baseline_Og main.cpp 
	# separating hot and cold data Og
	g++-9 -fopenmp -Og -Wall -std=c++17 -o pr_hot_cold_Og main_hot_cold.cpp 
	# hot and cold, tiling Og
	g++ -Og -Wall -std=c++17 -o pr_tiling_Og main_tiling.cpp 
	# baseline O3
	g++ -O3 -Wall -std=c++17 -o pr_baseline_O3 main.cpp 
	# separating hot and cold data O3
	g++-9 -fopenmp -O3 -Wall -std=c++17 -o pr_hot_cold_O3 main_hot_cold.cpp
	# hot and cold, tiling O3
	g++ -Og -Wall -std=c++17 -o pr_tiling_O3 main_tiling.cpp
else ifeq ($(OS), Linux)
pagerank_matrix: pagerank.hpp main.cpp pagerank_hot_cold.hpp main_hot_cold.cpp
	@echo "You are running on Linux"
	# baseline Og
	g++ -Og -Wall -std=c++17 -o pr_baseline_Og main.cpp 
	# separating hot and cold data Og
	g++ -fopenmp -Og -Wall -std=c++17 -o pr_hot_cold_Og main_hot_cold.cpp 
	# hot and cold, tiling Og
	g++ -Og -Wall -std=c++17 -o pr_tiling_Og main_tiling.cpp 
	# baseline O3
	g++ -O3 -Wall -std=c++17 -o pr_baseline_O3 main.cpp 
	# separating hot and cold data O3
	g++ -fopenmp -O3 -Wall -std=c++17 -o pr_hot_cold_O3 main_hot_cold.cpp
	# hot and cold, tiling O3
	g++ -Og -Wall -std=c++17 -o pr_tiling_O3 main_tiling.cpp
endif
