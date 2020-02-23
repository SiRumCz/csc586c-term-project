#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "pageRank.hpp"

using namespace std;
using namespace csc586;
using namespace csc586::aos;

int main()
{
    ifstream infile;
    infile.open("in.txt");

    if (infile.fail()) {
        cerr << "Error opeing a file" << endl;
        infile.close();
        exit(1);
    }

	unordered_map< Id, Node* > nodeMap; // Map id's to nodes.

    string line;
    string a, b;
    Node *A, *B;
    while (getline(infile, line))
    {
		istringstream iss(line);
		if (!(iss >> a >> b)) { break; } // Format error.

    	// Process pair (a, b).
        //cout << a << " " << b << endl;
		
		if (nodeMap.find(a) == nodeMap.end())
		{
			A = new struct Node({a, vector< Node* > {}, vector< Node* > {}, 0, 0});
			nodeMap.insert({a, A});
		} else {
			A = nodeMap[a];
		}

		if (nodeMap.find(b) == nodeMap.end())
		{

			B = new struct Node({b, vector< Node* > {}, vector< Node* > {}, 0, 0});
			nodeMap.insert({b, B});
		} else {
			B = nodeMap[b];
		}

		A->nodesTo.push_back(B);
		A->countTo++;
		B->nodesFrom.push_back(A);
		B->countFrom++;

		//cout << A->id << " " << A->countFrom <<" " << A->countTo << endl;
		//cout << B->id << " " << B->countFrom <<" " << B->countTo << endl;
	}

	infile.close();

	std::unordered_map< Id, Node* >::iterator it = nodeMap.begin();
	while(it != nodeMap.end())
	{
		// cout << it->second->nodesFrom[0]->id << endl;
		cout << it->first << " #From: " << it->second->countFrom << " #To: " << it->second->countTo << endl;
		it++;
	}
}