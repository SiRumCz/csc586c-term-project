#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "pageRank.hpp"

using namespace std;
using namespace csc586;
using namespace csc586::aos;

static const string filename = "in.txt";
static const int nIter = 3;

// PageRank Algorithm.
void pageRank(map< Id, Node* > &nodeMap)
{
	Node *A, *B;
	map< Id, Node* >::iterator it0;
	float n = nodeMap.size();

	// Initialze scores to 1/n;
	cout << "  i = 0" << endl;
	it0 = nodeMap.begin();
	while (it0 != nodeMap.end())
	{
		A = it0->second;
		A->score = 1 / n;
		cout << A->id << ": " << A->score << endl;
		it0++;
	}

	for (auto i = 1; i < nIter; ++i) // nIter iterations.
	{
		cout << "  i = " << i << endl;

		// Move score's to scorePrev's.
		it0 = nodeMap.begin();
		while (it0 != nodeMap.end())
		{
			A = it0->second;
			A->scorePrev = A->score;
			A->score = 0.0; // Reset score.
			it0++;
		}

		// For Node A in nodeMap, compute A's score with B's previous score (B->A).
		it0 = nodeMap.begin();
		while (it0 != nodeMap.end())
		{
			A = it0->second;
			cout << "Score " << A->id << " = 0";
			// For node B to A.
			for (vector< Node* >::iterator it1 = A->nodesFrom.begin(); it1 != A->nodesFrom.end(); ++it1)
			{
				B = *it1;
				A->score += B->scorePrev / B->countTo; // Update the score of A;
				cout << " + ("<< B->id << ")" << B->scorePrev << "/" <<B->countTo;
			}
			cout << " = " << A->score << endl;
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
    int a, b;
    Node *A, *B;
    while (getline(infile, line))
    {
		istringstream iss(line);
		if (!(iss >> a >> b)) { break; } // Format error.

    	// Process pair (a, b).
        //cout << a << " " << b << endl;
		
		if (nodeMap.find(a) == nodeMap.end())
		{
			A = new struct Node({a, vector< Node* > {}, vector< Node* > {}, 0, 0, 0.0, 0.0});
			nodeMap.insert({a, A});
		} else {
			A = nodeMap[a];
		}

		if (nodeMap.find(b) == nodeMap.end())
		{
			B = new struct Node({b, vector< Node* > {}, vector< Node* > {}, 0, 0, 0.0, 0.0});
			nodeMap.insert({b, B});
		} else {
			B = nodeMap[b];
		}

		A->nodesTo.push_back(B);
		A->countTo++;
		B->nodesFrom.push_back(A);
		B->countFrom++;
	}

	infile.close();

	pageRank(nodeMap);

	/*
	map< Id, Node* >::iterator it = nodeMap.begin();
	while(it != nodeMap.end())
	{
		cout << it->first << "  #From: " << it->second->countFrom << " #To: " << it->second->countTo << endl;
		it++;
	}
	*/



	return 0;
}