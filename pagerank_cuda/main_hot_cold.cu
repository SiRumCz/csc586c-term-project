/*
 * reference: http://home.ie.cuhk.edu.hk/~wkshum/papers/pagerank.pdf 
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <chrono> // timing
#include <numeric> // accumulate
#include <algorithm> // sort

#include "pagerank_hot_cold.hpp"

using namespace csc586_matrix;
using namespace csc586_matrix::soa_matrix;

/* global variables */
const std::string filename = "p2p-Gnutella04.txt";
const int N = 10876; // number of nodes
const int num_iter = 10; // number of pagerank iterations
const float d = 0.85f; // damping factor. 0.85 as defined by Google
const int blocksize = 512;

void print_scores( Matrix_soa *table )
{
    /* Print score matrix. */
    float sum = 0;
    for ( auto i = 0; i < N; ++i )
    {
        sum += table->hot[ i ].score;
        std::cout << i << "=" << table->hot[ i ].score << std::endl;
    }
    std::cout << "s=" << sum << std::endl;
}

void print_score_sum( Matrix_soa *table )
{
    float sum = 0;
    for ( auto i = 0; i < N; ++i )
    {
        sum += table->hot[ i ].score;
    }
    std::cout << "s=" << sum << std::endl;
}

int comp( std::tuple< int, Score > const &i, std::tuple< int, Score > const &j )
{
    return std::get< 1 >( i ) > std::get< 1 >( j );
}

void print_top_5( Matrix_soa *table )
{
    std::vector< std::tuple< int, Score > > sorted = {};
    for ( auto i = 0; i < N; ++i )
    {
        sorted.push_back( std::tuple< int, Score >{ i, table->hot[ i ].score } );
    }
    std::sort( sorted.begin(), sorted.end(), comp );
    for ( auto i = 0; i < std::min( 5, N); ++i )
    {
        std::cout << std::get< 0 >( sorted[ i ] ) << "(" << std::get< 1 >( sorted[ i ] ) << ") ";
    }
    std::cout << std::endl;
}

void print_table( Matrix_soa *table )
{
    /* Print visited matrix. */
    std::cout << "Entry Visited Table: " << std::endl;
    for ( auto i = 0; i < N; ++i )
    {
        for ( auto j = 0; j < N; ++j )
        {
            std::cout << table->cold[ i ].visited_col[ j ] << " ";
        }
        std::cout << std::endl;
    }
    /* Print entry matrix. */
    std::cout << "Entry Matrix Table: " << std::endl;
    for ( auto i = 0; i < N; ++i )
    {
        for ( auto j = 0; j < N; ++j )
        {
            std::cout << table->hot[ i ].entries_col[ j ] << " ";
        }
        std::cout << std::endl;
    }
    /* Print score matrix. */
    std::cout << "Score Matrix Table: " << std::endl;
    for ( auto i = 0; i < N; ++i )
    {
        std::cout << table->hot[ i ].score << std::endl;
    }
    /* Print num entries matrix. */
    std::cout << "Entries Matrix Table: " << std::endl;
    for ( auto i = 0; i < N; ++i )
    {
        std::cout << table->cold[ i ].num_entry << std::endl;
    }
}

void read_inputfile( Matrix_soa *table )
{
    /* Read file and build node. */
    std::ifstream infile;
    infile.open( filename );

    if (infile.fail()) {
        std::cerr << "Error opeing a file" << std::endl;
        infile.close();
        exit( 1 );
    }
	
    std::string line;
    uint32_t a, b;
    while ( getline( infile, line ) )
    {
		std::istringstream iss( line );
		if ( ! ( iss >> a >> b ) ) { break; } // Format error.

    	/* Process pair (a, b). */
        // std::cout << a << " " << b << std::endl;
		
        table->cold[ a ].visited_col[ b ] = 1;
        table->cold[ a ].num_entry += 1;
	}

	infile.close();
}

__global__
void update_entries( Matrix_soa *table, int N )
{
    int const i = threadIdx.x + blockIdx.x * blockDim.x;

    for ( auto j = 0; j < N; ++j )
    {
        table->hot[ i ].entries_col[ j ] = 1.0f / N;
    //     if ( table->cold[ j ].num_entry == 0 )
    //     {
    //         /* Dangling node: 1 / N */
    //         table->hot[ i ].entries_col[ j ] = 1.0f / N;
    //     }
    //     else if ( table->cold[ j ].visited_col[ i ] == 1 )
    //     {
    //         /* If v(j, i) is visited then a(ij) = 1/L(j). */
    //         table->hot[ i ].entries_col[ j ] = 1.0f / table->cold[ j ].num_entry;
    //     }
    //     // else{ table->ij_entries_matrix[ i ][ j ] = 0.0; }
    }
}

// __global__
// void cal_pagerank( Matrix_soa *table, std::vector< Score > *old_scores, int N )
// {
//     int const i = threadIdx.x + blockIdx.x * blockDim.x;
    
//     if( i < N )
//     {
//         old_scores[ i ] = table->hot[ i ].score;

//          Update pagerank scores. 
//         float sum = 0.0f;
//         for ( auto j = 0; j < N; ++j )
//         {
//             sum += old_scores[ j ] * table->hot[ i ].entries_col[ j ];
//         }
//         table->hot[ i ].score = d * old_scores[ i ] + ( 1.0f - d ) * sum;
//     }
// }

int main ()
{
    /* Initialize matrix table. */
    Matrix_soa* t = new Matrix_soa( { 
        std::vector< Tables_Hot > ( N, Tables_Hot( { 
            ( 1.0f / N ), 
            std::vector< Entry > ( N, 0.0f ) } ) ), 
        std::vector< Tables_Cold > ( N, Tables_Cold( { 
            0, 
            std::vector< Count > ( N, 0 ) } ) )
    } );

    /* Initialize Scores from previous iteration. */
    std::vector< Score > old_scores = {};
    old_scores.reserve( N );

    read_inputfile( t );

    auto const num_blocks = ceil( N / static_cast< float >( blocksize ) );

    /* Initialize pointers to memory on GPU. */
    std::vector< Score > *dev_old_scores;
    Matrix_soa *dev_table;

    /* Allocate GPU memory and copy t to there. */
    cudaMalloc( (void **) &dev_table, sizeof( t ) );
    cudaMalloc( (void **) &dev_old_scores, sizeof( old_scores ) );
    cudaMemcpy( dev_table, t, sizeof( t ), cudaMemcpyHostToDevice );
    cudaMemcpy( dev_old_scores, old_scores, sizeof( old_scores ), cudaMemcpyHostToDevice );
    
    update_entries<<< num_blocks, blocksize >>>( dev_table, N );
    // /* Time the pre-processing. */
    // auto start_time = std::chrono::steady_clock::now();
    // update_entries( t );
    // auto end_time = std::chrono::steady_clock::now();
    // auto const update_duration = std::chrono::duration_cast< std::chrono::microseconds >( end_time - start_time ).count();
    
    // /* Time the pagerank algorithm. */
    // start_time = std::chrono::steady_clock::now();

    // for ( auto i = 0; i < num_iter-1; ++i )
    // {
    //     cal_pagerank<<< num_blocks, blocksize >>>( dev_table, dev_old_scores, N );
    // }

    // end_time = std::chrono::steady_clock::now();
    // auto const pr_duration = std::chrono::duration_cast< std::chrono::microseconds >( end_time - start_time ).count();
    
    // /* Free GPU memory */
    cudaFree( dev_table );
    cudaFree( dev_old_scores);

    // // print_scores( t );
    // print_top_5( t );
    // print_score_sum( t );
    // std::cout << "Entries update time = "
    //           << update_duration
    //           << " us"
    //           << "\nCalculation time = "
		  //     << pr_duration
		  //     << " us" 
    //           << "\nTotal time = "
    //           << update_duration + pr_duration
    //           << " us"
    //           << std::endl;
    // return 0;
}