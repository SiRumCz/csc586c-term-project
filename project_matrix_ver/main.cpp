/*
 * reference: http://home.ie.cuhk.edu.hk/~wkshum/papers/pagerank.pdf 
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono> // timing
#include <algorithm> // sort
#include <numeric> // accumulate

#include "pagerank.hpp"

using namespace csc586_matrix;
using namespace csc586_matrix::soa_matrix;

/* global variables */
const int N = 10000; // number of nodes
const int num_iter = 10; // number of pagerank iterations
const std::string filename = "../test/erdos-10000.txt";
const float d = 0.85; // damping factor

void print_scores( Tables *table )
{
    /* print score matrix */
    float sum = 0;
    for ( auto i = 0; i < N; ++i )
    {
        sum += table->scores[ i ];
        std::cout << i << "=" << table->scores[ i ] << std::endl;
    }
    std::cout << "s=" << sum << std::endl;
}

int comp( std::tuple< int, Score > const &i, std::tuple< int, Score > const &j )
{
    return std::get< 1 >( i ) > std::get< 1 >( j );
}

void print_score_sum( Tables *table )
{
    float sum =  std::accumulate( table->scores.begin(), table->scores.end(), 0.0 );
    std::cout << "s=" << sum << std::endl;
}

void print_top_5( Tables *table )
{
    std::vector< std::tuple< int, Score > > sorted = {};
    for ( auto i = 0; i < N; ++i )
    {
        sorted.push_back( std::tuple< int, Score >{ i, table->scores[ i ] } );
    }
    std::sort( sorted.begin(), sorted.end(), comp );
    for ( auto i = 0; i < std::min( 5, N); ++i )
    {
        std::cout << std::get< 0 >( sorted[ i ] ) << "(" << std::get< 1 >( sorted[ i ] ) << ") ";
    }
    std::cout << std::endl;
}

void print_table( Tables *table )
{
    /* print visited matrix */
    std::cout << "Entry Visited Table: " << std::endl;
    for ( auto i = 0; i < N; ++i )
    {
        for ( auto j = 0; j < N; ++j )
        {
            std::cout << table->visited_matrix[ i ][ j ] << " ";
        }
        std::cout << std::endl;
    }
    /* print entry matrix */
    std::cout << "Entry Matrix Table: " << std::endl;
    for ( auto i = 0; i < N; ++i )
    {
        for ( auto j = 0; j < N; ++j )
        {
            std::cout << table->ij_entries_matrix[ i ][ j ] << " ";
        }
        std::cout << std::endl;
    }
    /* print score matrix */
    std::cout << "Score Matrix Table: " << std::endl;
    for ( auto i = 0; i < N; ++i )
    {
        std::cout << table->scores[ i ] << std::endl;
    }
    /* print num entries matrix */
    std::cout << "Entries Matrix Table: " << std::endl;
    for ( auto i = 0; i < N; ++i )
    {
        std::cout << table->num_entries[ i ] << std::endl;
    }
}

void read_inputfile( Tables *table )
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
		
        table->visited_matrix[ a ][ b ] = 1;
        table->num_entries[ a ] += 1;
	}

	infile.close();
}

void update_entries( Tables *table )
{
    for ( auto i = 0; i < N; ++i )
    {
        for ( auto j = 0; j < N; ++j )
        {
            if ( table->num_entries[ j ] == 0 )
            {
                // dangling node: 1 / N
                table->ij_entries_matrix[ i ][ j ] = 1.0f / N;
            }
            else if ( table->visited_matrix[ j ][ i ] == 1 )
            {
                // if v(j, i) is visited then a(ij) = 1/L(j)
                table->ij_entries_matrix[ i ][ j ] = 1.0f / table->num_entries[ j ];
            }
            // else{ table->ij_entries_matrix[ i ][ j ] = 0.0; }
        }
    }
}

void cal_pagerank( Tables *table )
{
    for ( auto i = 0; i < num_iter-1; ++i )
    {
        /* scores from previous iteration */
        std::vector< Score > old_scores = {};
        for ( auto j = 0; j < N; ++j )
        {
            old_scores.push_back( table->scores[ j ] );
        }
        /* update pagerank scores */
        for ( auto j = 0; j < N; ++j )
        {
            float sum = 0.0f;
            for ( auto k = 0; k < N; ++k )
            {
                sum += old_scores[ k ] * table->ij_entries_matrix[ j ][ k ];
            }
            table->scores[ j ] = d * old_scores[ j ] + ( 1.0f - d ) * sum;
        }
    }
}

int main ()
{
    /* initialize matrix table */
    Tables* t = new struct Tables( { std::vector< Score > ( N, 1.0f / N ),
                                     std::vector< Count > ( N, 0 ),
                                     std::vector< std::vector< Count > > ( N, std::vector< Count > ( N, 0 ) ), 
                                     std::vector< std::vector< Entry > > ( N, std::vector< Entry > ( N, 0.0f ) ) } );

    read_inputfile( t );
    /* timing the pre processing */
    auto start_time = std::chrono::steady_clock::now();
    update_entries( t );
    auto end_time = std::chrono::steady_clock::now();
    auto const update_duration = std::chrono::duration_cast<std::chrono::microseconds>( end_time - start_time ).count();
    /* timing the pagerank algorithm */
    start_time = std::chrono::steady_clock::now();
    cal_pagerank( t );
    end_time = std::chrono::steady_clock::now();
    auto const pr_duration = std::chrono::duration_cast<std::chrono::microseconds>( end_time - start_time ).count();
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