#ifndef CSC586C_PAGERANK_BASELINE
#define CSC586C_PAGERANK_BASELINE

#include <vector>

namespace csc586_matrix
{
    using Score = double;
    using Entry = double;
    using Count = uint32_t;

    namespace soa_matrix
    {
        struct Tables
        {
            std::vector< Score > scores;
            std::vector< Count > num_entries;
            std::vector< std::vector< Count > > visited_matrix; // 1: visited, 0: unvisited
            std::vector< std::vector< Entry > > ij_entries_matrix; // NxN matrix of (i, j)-entries
        };
    }
}

#endif