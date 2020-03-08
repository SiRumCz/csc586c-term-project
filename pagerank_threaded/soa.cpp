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

static const string filename = "../test/meredith.txt";
static const int nIter = 10000; // Number of iterations.
static const float d = 0.85; // Damping factor.

// PageRank Algorithm.
void pageRank(Nodes *nodes)
{
	float n = nodes->nodesFrom.size();

	// Iteration.
	for (auto iter = 0; iter < nIter; ++iter)
	{

		if (iter == 0)
		{
			// Initialize scores.
			for (auto i = 0; i < n; ++i)
			{
				nodes->score[i] = 1.0 / n;
			}
		} else {
			// Move current score to previous score, and assign (1-d)/n to each current score (do addition later).
			for (auto i = 0; i < n; ++i)
			{
				nodes->scorePrev[i] = nodes->score[i];
				nodes->score[i] = (1 - d)/n;
			}
			// For every node i, visit the nodes *j going to node i (node *j -> node i).
			for (auto i = 0; i < n; ++i)
			{
				for (auto j=0; j<nodes->nodesFrom[i].size(); ++j)
				{
					nodes->score[i] += (nodes->scorePrev[j]/nodes->countTo[j])*d;
				}
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
        cerr << "Error opeing a file" << endl;
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
		if (!(iss >> a >> b)) { break; } // Format error.

		// Process pair (a, b).
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
		cout << i << " = " << nodes->score[i] << endl;
		sum += nodes->score[i];
	}
	cout << "sum = " << sum << endl;
	cout << "Calculation time = "
			  << chrono::duration_cast<chrono::microseconds>( end_time - start_time ).count()
			  << " us" << endl;

	return 0;
}