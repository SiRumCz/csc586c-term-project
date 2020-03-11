all: pageRank.hpp pagerank.hpp soa.cpp main.cpp main_hot_cold.cpp pagerank_hot_cold.hpp
	# soa
	g++ -Og -Wall -std=c++17 -fopenmp soa.cpp -o soa

	# matrix
	g++ -Og -Wall -std=c++17 -fopenmp main.cpp -o main

	# matrix_hot_cold
	g++ -Og -Wall -std=c++17 -fopenmp main_hot_cold.cpp -o main_hot_cold