/*
 * reference: http://home.ie.cuhk.edu.hk/~wkshum/papers/pagerank.pdf 
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <chrono> // timing
#include <algorithm> // sort
#include <omp.h>    // for multi-core parallelism

#include "pagerank_hot_cold.hpp"

using namespace csc586_matrix;
using namespace csc586_matrix::soa_matrix;

/* global variables */
const int N = 100000; // number of nodes
const int num_iter = 10; // number of pagerank iterations
const std::string filename = "../test/erdos-100000.txt";
const float d = 0.85f; // damping factor. 0.85 as defined by Google

void print_scores( Matrix_soa *table )
{
    /* print score matrix */
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
    /* print visited matrix */
    std::cout << "Entry Visited Table: " << std::endl;
    for ( auto i = 0; i < N; ++i )
    {
        for ( auto j = 0; j < N; ++j )
        {
            std::cout << table->cold[ i ].visited_col[ j ] << " ";
        }
        std::cout << std::endl;
    }
    /* print entry matrix */
    std::cout << "Entry Matrix Table: " << std::endl;
    for ( auto i = 0; i < N; ++i )
    {
        for ( auto j = 0; j < N; ++j )
        {
            std::cout << table->hot[ i ].entries_col[ j ] << " ";
        }
        std::cout << std::endl;
    }
    /* print score matrix */
    std::cout << "Score Matrix Table: " << std::endl;
    for ( auto i = 0; i < N; ++i )
    {
        std::cout << table->hot[ i ].score << std::endl;
    }
    /* print num entries matrix */
    std::cout << "Entries Matrix Table: " << std::endl;
    for ( auto i = 0; i < N; ++i )
    {
        std::cout << table->cold[ i ].num_entry << std::endl;
    }
}

void read_inputfile( Matrix_soa *table )
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
    uint32_t a, b;
    while ( getline( infile, line ) )
    {
		std::istringstream iss( line );
		if ( ! ( iss >> a >> b ) ) { break; } // Format error.

    	// Process pair (a, b).
        // std::cout << a << " " << b << std::endl;
		
        table->cold[ a ].visited_col[ b ] = 1;
        table->cold[ a ].num_entry += 1;
	}

	infile.close();
}

void update_entries( Matrix_soa *table )
{
    #pragma omp parallel for
    for ( auto i = 0; i < N; ++i )
    {
        for ( auto j = 0; j < N; ++j )
        {
            if ( table->cold[ j ].num_entry == 0 )
            {
                // dangling node: 1 / N
                table->hot[ i ].entries_col[ j ] = 1.0f / N;
            }
            else if ( table->cold[ j ].visited_col[ i ] == 1 )
            {
                // if v(j, i) is visited then a(ij) = 1/L(j)
                table->hot[ i ].entries_col[ j ] = 1.0f / table->cold[ j ].num_entry;
            }
            // else{ table->ij_entries_matrix[ i ][ j ] = 0.0; }
        }
    }
}

void cal_pagerank( Matrix_soa *table )
{
    for ( auto i = 0; i < num_iter-1; ++i )
    {
        /* scores from previous iteration */
        std::vector< Score > old_scores = {};
        old_scores.reserve( N );
        #pragma omp parallel for reduction( +:sum )
        for ( auto j = 0; j < N; ++j )
        {
            old_scores[ j ] = table->hot[ j ].score;
        }
        /* update pagerank scores */
        float sum = 0.0f;
        /* handling critical section with omp reduction */
        // #pragma omp parallel for reduction( +:sum ) num_threads( 1 )
        for ( auto j = 0; j < N; ++j )
        {
            sum = 0.0f;
            for ( auto k = 0; k < N; ++k )
            {
                sum += old_scores[ k ] * table->hot[ j ].entries_col[ k ];
            }
            table->hot[ j ].score = d * old_scores[ j ] + ( 1.0f - d ) * sum;
        }
    }
}

int main ()
{
    /* initialize matrix table */
    Matrix_soa* t = new Matrix_soa( { 
        std::vector< Tables_Hot > ( N, Tables_Hot( { 
            ( 1.0f / N ), 
            std::vector< Entry > ( N, 0.0f ) } ) ), 
        std::vector< Tables_Cold > ( N, Tables_Cold( { 
            0, 
            std::vector< Count > ( N, 0 ) } ) )
    } );

    read_inputfile( t );
    /* timing the pre processing */
    auto start_time = std::chrono::steady_clock::now();
    update_entries( t );
    auto end_time = std::chrono::steady_clock::now();
    auto const update_duration = std::chrono::duration_cast< std::chrono::microseconds >( end_time - start_time ).count();
    /* timing the pagerank algorithm */
    start_time = std::chrono::steady_clock::now();
    cal_pagerank( t );
    end_time = std::chrono::steady_clock::now();
    auto const pr_duration = std::chrono::duration_cast< std::chrono::microseconds >( end_time - start_time ).count();
    // print_scores( t );
    print_top_5( t );
    print_score_sum( t );
    std::cout << "Entries update time = "
              << update_duration
              << " us"
              << "\nCalculation time = "
		      << pr_duration
		      << " us" 
              << "\nTotal time = "
              << update_duration + pr_duration
              << " us"
              << std::endl;
    return 0;
}