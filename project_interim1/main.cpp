#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <chrono>

#include "pageRank.hpp"

using namespace std;
using namespace csc586;
using namespace csc586::aos;

static const string filename = "../test/erdos-10000.txt";
static const int nIter = 500; // Number of iterations.
static const float d = 0.85; // Damping factor.

// PageRank Algorithm.
void pageRank(map< Id, Node* > &nodeMap)
{
	Node *A, *B;
	map< Id, Node* >::iterator it0;
	float n = nodeMap.size();

	// Initialze scores to 1/n;
	//cout << "  i = 0" << endl;
	it0 = nodeMap.begin();
	while (it0 != nodeMap.end())
	{
		A = it0->second;
		A->score = 1.0 / n;
		//cout << A->id << ": " << A->score << endl;
		it0++;
	}

	for (auto i = 1; i < nIter; ++i) // nIter iterations.
	{
		//cout << "  i = " << i << endl;

		// Move score's to scorePrev's.
		it0 = nodeMap.begin();
		while (it0 != nodeMap.end())
		{
			A = it0->second;
			A->scorePrev = A->score;
			A->score = (1 - d)/n; // Reset score for addition.
			it0++;
		}

		// For Node A in nodeMap, compute A's score with B's previous score (B->A).
		it0 = nodeMap.begin();
		while (it0 != nodeMap.end())
		{
			A = it0->second;
			//cout << "Score " << A->id << " = 0";
			// For node B to A.
			for (vector< Node* >::iterator it1 = A->nodesFrom.begin(); it1 != A->nodesFrom.end(); ++it1)
			{
				B = *it1;
				A->score += (B->scorePrev / B->countTo)*d; // Update the score of A;
				//cout << " + ("<< B->id << ")" << B->scorePrev << "/" <<B->countTo;
			}
			//cout << " = " << A->score << endl;
			it0++;
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

	map< Id, Node* > nodeMap; // Map id's to nodes.
	
    string line;
    Id a, b;
    Node *A, *B;
    while (getline(infile, line))
    {
		istringstream iss(line);
		if (!(iss >> a >> b)) { break; } // Format error.

    	// Process pair (a, b).
        //cout << a << " " << b << endl;
		
		if (nodeMap.find(a) == nodeMap.end())
		{
			A = new struct Node({vector< Node* > {}, 0.0, 0.0, a, 0});
			nodeMap.insert({a, A});
		} else {
			A = nodeMap[a];
		}

		if (nodeMap.find(b) == nodeMap.end())
		{
			B = new struct Node({vector< Node* > {}, 0.0, 0.0, b, 0});
			nodeMap.insert({b, B});
		} else {
			B = nodeMap[b];
		}

		//A->nodesTo.push_back(B);
		A->countTo++;
		B->nodesFrom.push_back(A);
		//B->countFrom++;
	}

	infile.close();

	auto const start_time = std::chrono::steady_clock::now();
	pageRank(nodeMap);
	auto const end_time = std::chrono::steady_clock::now();

	// Print results.

	float sum = 0.0;
	map< Id, Node* >::iterator it = nodeMap.begin();
	while (it != nodeMap.end())
	{
		cout << it->first << " = " << it->second->score << endl;
		sum += it->second->score;
		it++;
	}
	cout << "sum = " << sum << endl;
	cout << "Calculation time = "
			  << chrono::duration_cast<chrono::microseconds>( end_time - start_time ).count()
			  << " us" << endl;
	return 0;
}