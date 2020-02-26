#ifndef CSC586C_PAGERANK_HC
#define CSC586C_PAGERANK_HC

#include <vector>

namespace csc586_matrix
{
    using Score = double;
    using Entry = double;
    using Count = short int;

    namespace soa_matrix
    {
        /* data accessed during the pagerank algorithm */
        struct Tables_Hot
        {
            std::vector< std::vector< Entry > > ij_entries_matrix;
            std::vector< Score > scores;
        };
        /* data used in prep stage */
        struct Tables_Cold
        {
            std::vector< std::vector< Count > > visited_matrix;
            std::vector< Count > num_entries;
        };
        
    }
}

#endif