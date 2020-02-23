#ifndef CSC586C_pageRank
#define CSC586C_pageRank

#include <vector>

using namespace std;

namespace csc586 {

using Id = string;
using Count = int;

namespace aos {

struct Node
{
	Id id;
	vector< Node* > nodesFrom;
	vector< Node* > nodesTo;
	Count countFrom;
	Count countTo;
};

}
}

#endif