#ifndef CSC586C_pageRank
#define CSC586C_pageRank

#include <vector>

using namespace std;

namespace csc586 {

using Id = unsigned short int;
using Count = unsigned short int;
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
}

#endif