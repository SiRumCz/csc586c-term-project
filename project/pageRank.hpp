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
	Id id;
	vector< Node* > nodesFrom;
	vector< Node* > nodesTo;
	Count countFrom;
	Count countTo;
	Score scorePrev; // Score in iteration i-1;
	Score score; // Score in iteration i;
};

}
}

#endif