#ifndef CSC586C_PAGERANK_HC
#define CSC586C_PAGERANK_HC

#include <vector>

namespace csc586_matrix
{
    using Score = float;
    using Entry = float;
    using Count = uint32_t;

    namespace soa_matrix
    {
        /* data accessed during the pagerank algorithm */
        struct Tables_Hot
        {
            Score score; // 8 bytes
            std::vector< Entry > entries_col; // 24 bytes
        };
        /* data used in prep stage */
        struct Tables_Cold
        {
            Count num_entry; // 4 bytes
            std::vector< Count > visited_col;
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