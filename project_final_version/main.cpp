/*
* reference: http://home.ie.cuhk.edu.hk/~wkshum/papers/pagerank.pdf 
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <tuple>
#include <vector>
#include <chrono> // timing
#include <algorithm> // sort
#include <omp.h>    // for multi-core parallelism


/* global variables */
const long long N = 10876; // number of nodes
const int num_iter = 10; // number of pagerank iterations
const std::string filename = "../tests/p2p-Gnutella04.txt";
const float d = 0.85f; // damping factor. 0.85 as defined by Google

typedef float trans_m_col[N];
typedef int vis_m_col[N];

void read_inputfile( vis_m_col *visited_matrix, int outgoing_table[ N ] )
{
    // Read file and build node.
    std::ifstream infile;
    infile.open( filename );

    if (infile.fail()) {
        std::cerr << "Error opeing a file" << std::endl;
        infile.close();
        exit( 1 );
    }
    
    std::string line;
    int a, b;
    while ( getline( infile, line ) )
    {
        std::istringstream iss( line );
        if ( ! ( iss >> a >> b ) ) { break; } // Format error.

        // Process pair (a, b).
        // std::cout << a << " " << b << std::endl;
        
        visited_matrix[ a ][ b ] = 1;
        outgoing_table[ a ] += 1;
    }

    infile.close();
}

/**
 * outgoing_table, transition_matrix, visited_matrix
*/
void update_entries( trans_m_col *transition_matrix, vis_m_col *visited_matrix, int *outgoing_table )
{
    #pragma omp parallel for
    for ( auto i = 0; i < N; ++i )
    {
        for ( auto j = 0; j < N; ++j )
        {
            if ( outgoing_table[ j ] == 0 )
            {
                // dangling node: 1 / N
                transition_matrix[ i ][ j ] = 1.0f / N;
            }
            else if ( visited_matrix[ j ][ i ] == 1 )
            {
                // if v(j, i) is visited then a(ij) = 1/L(j)
                transition_matrix[ i ][ j ] = 1.0f / outgoing_table[ j ];
            }
            // else{ table->ij_entries_matrix[ i ][ j ] = 0.0; }
        }
    }
}

void pagerank( float score_table[ N ], trans_m_col *transition_matrix )
{
    for ( auto i = 0; i < num_iter - 1; ++i )
    {
        /* scores from previous iteration */
        float old_scores[ N ] = { 0.0f };
        #pragma omp parallel for
        for ( auto j = 0; j < N; ++j )
        {
            old_scores[ j ] = score_table[ j ];
        }
        /* update pagerank scores */
        float sum = 0.0f;
        /* handling critical section with omp reduction */
        #pragma omp parallel for reduction( +:sum )
        for ( auto j = 0; j < N; ++j )
        {
            sum = 0.0f;
            for ( auto k = 0; k < N; ++k )
            {
                sum += old_scores[ k ] * transition_matrix[ j ][ k ];
            }
            score_table[ j ] = d * old_scores[ j ] + ( 1.0f - d ) * sum;
        }
    }
}

int comp( std::tuple< int, float > const &i, std::tuple< int, float > const &j )
{
    return std::get< 1 >( i ) > std::get< 1 >( j );
}

void print_top_5( float arr[ N ] )
{
    std::vector< std::tuple< int, float > > sorted = {};
    for ( auto i = 0; i < N; ++i )
    {
        sorted.push_back( std::tuple< int, float >{ i, arr[ i ] } );
    }
    std::sort( sorted.begin(), sorted.end(), comp );
    for ( auto i = 0; i < std::min( ( long long ) 5, N ); ++i )
    {
        std::cout << std::get< 0 >( sorted[ i ] ) << "(" << std::get< 1 >( sorted[ i ] ) << ") ";
    }
    std::cout << std::endl;
}

void print_total( float arr[ N ] )
{
    float sum = 0.0f;
    for ( auto i = 0; i < N; ++i )
    {
        sum += arr[ i ];
    }
    std::cout << "s=" << sum << std::endl;
}

int main( int argc, char * argv[] )
{
    auto const total_start_time = std::chrono::steady_clock::now();

    auto const vis_m_size = N * N * sizeof(int);
    auto const trans_m_size = N * N * sizeof(float);
    vis_m_col *visited_matrix;
    visited_matrix = ( vis_m_col * )malloc( vis_m_size );
    std::memset(visited_matrix, 0, vis_m_size);

    trans_m_col *transition_matrix;
    transition_matrix = ( trans_m_col * )malloc( trans_m_size );
    std::memset(transition_matrix, 0, trans_m_size);

    float score_table[ N ] = { 0 };
    std::fill_n(score_table, N, 1.0f / N );
    int outgoing_table[ N ] = { 0 };

    read_inputfile( visited_matrix, outgoing_table );
    
    /* timing the PageRank algorithm */
    auto const pr_start_time = std::chrono::steady_clock::now();

    update_entries( transition_matrix, visited_matrix, outgoing_table );

    pagerank( score_table, transition_matrix );

    auto const pr_end_time = std::chrono::steady_clock::now();
    auto const pr_time = std::chrono::duration_cast< std::chrono::microseconds >\
    ( pr_end_time - pr_start_time ).count();

    auto const total_end_time = std::chrono::steady_clock::now();
    auto const total_time = std::chrono::duration_cast< std::chrono::microseconds >\
    ( total_end_time - total_start_time ).count();

    print_top_5( score_table );
    print_total( score_table );

    std::cout << "pagerank_time = "
              << pr_time
              << " us" 
              << "\ntotal_time = "
              << total_time
              << " us"
              << std::endl;
    
    return 0;
}