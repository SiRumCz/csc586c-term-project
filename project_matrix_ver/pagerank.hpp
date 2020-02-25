#ifndef CSC586C_pageRank
#define CSC586C_pageRank

#include <vector>

namespace csc586_matrix
{
    using Score = double;
    using Entry = double;
    using Count = short int;

    namespace soa_matrix
    {
        struct Tables
        {
            /* TODO: separate hot and cold data */
            std::vector< std::vector< Count > > visited_matrix;
            std::vector< std::vector< Entry > > ij_entries_matrix;
            std::vector< Score > scores;
            std::vector< Count > num_entries;
        };
    }
}

#endif