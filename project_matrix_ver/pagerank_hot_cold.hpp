#ifndef CSC586C_PAGERANK_HC
#define CSC586C_PAGERANK_HC

#include <vector>

namespace csc586_matrix
{
    using Score = double;
    using Entry = double;
    using Count = uint32_t;

    namespace soa_matrix
    {
        /* data accessed during the pagerank algorithm */
        struct Tables_Hot
        {
            std::vector< Entry > entries_col;
            Score score;
        };
        /* data used in prep stage */
        struct Tables_Cold
        {
            std::vector< Count > visited_col;
            Count num_entry;
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