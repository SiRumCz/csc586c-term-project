/*
 * reference: http://home.ie.cuhk.edu.hk/~wkshum/papers/pagerank.pdf 
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono> // timing
#include <numeric> // accumulate

#include "pagerank_hot_cold.hpp"

using namespace csc586_matrix;
using namespace csc586_matrix::soa_matrix;

/* global variables */
const int N = 10000; // number of nodes
const int num_iter = 10; // number of pagerank iterations
const std::string filename = "../test/erdos-10000.txt";

void print_scores( Matrix_soa *table )
{
    /* print score matrix */
    double sum = 0;
    for ( auto i = 0; i < N; ++i )
    {
        sum += table->hot[ i ].score;
        std::cout << i << "=" << table->hot[ i ].score << std::endl;
    }
    std::cout << "s=" << sum << std::endl;
}

void print_score_sum( Matrix_soa *table )
{
    double sum = 0;
    for ( auto i = 0; i < N; ++i )
    {
        sum += table->hot[ i ].score;
    }
    std::cout << "s=" << sum << std::endl;
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
    for ( auto i = 0; i < N; ++i )
    {
        for ( auto j = 0; j < N; ++j )
        {
            if ( table->cold[ j ].num_entry == 0 )
            {
                // dangling node: 1 / N
                table->hot[ i ].entries_col[ j ] = 1.0 / N;
            }
            else if ( table->cold[ j ].visited_col[ i ] == 1 )
            {
                // if v(j, i) is visited then a(ij) = 1/L(j)
                table->hot[ i ].entries_col[ j ] = 1.0 / table->cold[ j ].num_entry;
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
        for ( auto j = 0; j < N; ++j )
        {
            old_scores.push_back( table->hot[ j ].score );
        }
        /* update pagerank scores */
        for ( auto j = 0; j < N; ++j )
        {
            double sum = 0.0;
            for ( auto k = 0; k < N; ++k )
            {
                sum += old_scores[ k ] * table->hot[ j ].entries_col[ k ];
            }
            table->hot[ j ].score = sum;
        }
    }
}

int main ()
{
    /* initialize matrix table */
    Matrix_soa* t = new Matrix_soa( { 
        std::vector< Tables_Hot > {}, 
        std::vector< Tables_Cold > {} 
    } );
    
    /* initialize matrix */
    for ( auto i = 0; i < N; ++i )
    {
        t->cold.push_back( Tables_Cold( { 0, std::vector< Count > {} } ) );
        t->hot.push_back( Tables_Hot( { 0.0, std::vector< Entry > {} } ) );
        for ( auto j = 0; j < N; ++j )
        {
            t->cold[ i ].visited_col.push_back( 0 );
            t->hot[ i ].entries_col.push_back( 0.0 );
        }
        t->hot[ i ].score = 1.0 / N;
        t->cold[ i ].num_entry = 0;
    }

    read_inputfile( t );
    update_entries( t );
    /* timing the pagerank algorithm */
    auto const start_time = std::chrono::steady_clock::now();
    cal_pagerank( t );
    auto const end_time = std::chrono::steady_clock::now();
    // print_scores( t );
    print_score_sum( t );
    std::cout << "Calculation time = "
		      << std::chrono::duration_cast< std::chrono::microseconds >( end_time - start_time ).count()
		      << " us" << std::endl;
    return 0;
}