#ifndef CSC586C_PAGERANK_MATRIX
#define CSC586C_PAGERANK_MATRIX

#include <vector>

namespace csc586_matrix
{
    using Score = float;
    using Entry = float;
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

    namespace soa_matrix_hot_cold
    {
        /* data accessed during the pagerank algorithm */
        struct Tables_Hot
        {
            Score score; // 4 bytes
            std::vector< Entry > entries_col; // 24 bytes
        };
        /* data used in prep stage */
        struct Tables_Cold
        {
            Count num_entry; // 4 bytes
            std::vector< Count > visited_col; // 24 bytes
        };
        /* final matrix table consists of hot and cold data */
        struct Matrix_soa
        {
            std::vector< Tables_Hot > hot;
            std::vector< Tables_Cold > cold;
        };
    }
}

#endif