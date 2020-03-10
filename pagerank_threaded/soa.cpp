#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <chrono>
#include <omp.h>

#include "pageRank.hpp"

using namespace std;
using namespace csc586;
using namespace csc586::soa;

static const string filename = "../test/erdos-100000.txt";
static const int nIter = 10; // Number of iterations.
static const float d = 0.85; // Damping factor.

// PageRank Algorithm.
void pageRank(Nodes *nodes)
{
	int n = nodes->nodesFrom.size();

	//int count = 0;
	//#pragma omp parallel for
	for (auto i = 0; i < nodes->countTo.size(); ++i)
	{
		// Dangling node: no outgoing nodes.
		if (nodes->countTo[i] == 0)
		{
			// Let it outgoing to everyone!
			nodes->countTo[i] = n;

			for (auto j = 0; j < n; ++j)
			{
				nodes->nodesFrom[j].push_back(i);
			}

			//count++;
		}
	}

	//cout << count << endl;
	// Iteration.
	for (auto iter = 0; iter < nIter; ++iter)
	{

		if (iter == 0)
		{
			// Initialize scores.
			//#pragma omp parallel for
			for (auto i = 0; i < n; ++i)
			{
				nodes->score[i] = 1.0 / n;
			}
		} else {
			// Move current score to previous score, and assign (1-d)/n to each current score (do addition later).
			//#pragma omp parallel for
			for (auto i = 0; i < n; ++i)
			{
				nodes->scorePrev[i] = nodes->score[i];
				nodes->score[i] = (1 - d)/n;
			}
			// For every node i, visit the nodes *j going to node i (node *j -> node i).
			#pragma omp parallel for num_threads(4)
			for (auto i = 0; i < n; ++i)
			{
				for (vector< Id >::iterator j = nodes->nodesFrom[i].begin(); j != nodes->nodesFrom[i].end(); ++j)
				{
					// cout << *j << endl;
					nodes->score[i] += (nodes->scorePrev[*j]/nodes->countTo[*j])*d;
				}
				// for (auto j = 0; j < nodes->nodesFrom[i].size(); ++j)
				// {
				// 	nodes->score[i] += (nodes->scorePrev[nodes->nodesFrom[i][j]]/nodes->countTo[nodes->nodesFrom[i][j]])*d;
				// }
			}
		}
	}
}


int main()
{
	// Read file and build node.
    ifstream infile;
    infile.open(filename);

    if (infile.fail()) {
        cerr << "Error opening a file" << endl;
        infile.close();
        exit(1);
    }
	
	// Initialize Nodes.
    Nodes *nodes = new struct Nodes
    ({
    	vector< vector< Id > > {}, // nodesFrom
    	vector< Count > {}, // countTo
    	vector< Score > {}, // score
    	vector< Score > {} // scorePrev
    });

    string line;
    Id a, b;
    while (getline(infile, line))
    {
		istringstream iss(line);
		// Format error.
		if (!(iss >> a >> b))
		{ 
			cout << "Error on: " << line << endl;
			exit(-1);
		}
		//cout << a << " " << b << endl;

		// Initialize vectors if index larger than size.
		if (b >= nodes->nodesFrom.size()) // a < b in all the edge lists.
		{
			int padding = b - nodes->nodesFrom.size() + 1;
			for (auto i = 0; i < padding; ++i)
			{
				nodes->nodesFrom.push_back(vector <Id> {});
				nodes->countTo.push_back(0);
				nodes->score.push_back(0);
				nodes->scorePrev.push_back(0);
			}
		}

		// Update attributes.
		nodes->nodesFrom[b].push_back(a);
		nodes->countTo[a] += 1;
	}

	infile.close();

	auto const start_time = std::chrono::steady_clock::now();
	pageRank(nodes);
	auto const end_time = std::chrono::steady_clock::now();

	// Print results.
	
	float sum = 0.0;
	for (long unsigned int i = 0; i < nodes->nodesFrom.size(); ++i)
	{
		//cout << i << " = " << nodes->score[i] << endl;
		sum += nodes->score[i];
	}
	cout << "sum = " << sum << endl;
	cout << "Calculation time = "
			  << chrono::duration_cast<chrono::microseconds>( end_time - start_time ).count()
			  << " us" << endl;
	
	return 0;
}