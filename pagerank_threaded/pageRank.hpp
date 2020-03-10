#ifndef CSC586C_pageRank
#define CSC586C_pageRank

#include <vector>

using namespace std;

namespace csc586 {

using Id = int;
using Count = int;
using Score = float;

namespace aos {

struct Node
{
	vector< Node* > nodesFrom; // 8*n bytes.
	Score scorePrev; // Score in iteration i-1; 4 bytes.
	Score score; // Score in iteration i; 4 bytes.
	Id id; // 2 bytes.
	Count countTo; // 2 bytes.
};

}


namespace soa {

struct Nodes
{
	vector< vector< Id > > nodesFrom;
	vector< Count > countTo;
	vector< Score > score;
	vector< Score > scorePrev;
};

}


namespace soa_v2 {

struct Nodes
{
	vector< vector< Id > > nodesFrom;
	vector< Count > countTo;
	// Merge score and scorePrev into one vector.
	// Node i: previous score at index i*2, current score at index i*2 + 1.
	vector< Score > score;
};

}
}

#endif
