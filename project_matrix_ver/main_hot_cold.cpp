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
const uint32_t N = 10000; // number of nodes
const uint32_t num_iter = 10; // number of pagerank iterations
const std::string filename = "../test/erdos-10000.txt";

void print_scores( Tables_Hot *table )
{
    /* print score matrix */
    float sum = 0;
    for ( int i = 0; i < N; ++i )
    {
        sum += table->scores[ i ];
        std::cout << i << "=" << table->scores[ i ] << std::endl;
    }
    std::cout << "s=" << sum << std::endl;
}

void print_score_sum( Tables_Hot *table )
{
    float sum =  std::accumulate( table->scores.begin(), table->scores.end(), 0.0 );
    std::cout << "s=" << sum << std::endl;
}

void print_table( Tables_Hot *tableh, Tables_Cold *tablec )
{
    /* print visited matrix */
    std::cout << "Entry Visited Table: " << std::endl;
    for ( int i = 0; i < N; ++i )
    {
        for ( int j = 0; j < N; ++j )
        {
            std::cout << tablec->visited_matrix[ i ][ j ] << " ";
        }
        std::cout << std::endl;
    }
    /* print entry matrix */
    std::cout << "Entry Matrix Table: " << std::endl;
    for ( int i = 0; i < N; ++i )
    {
        for ( int j = 0; j < N; ++j )
        {
            std::cout << tableh->ij_entries_matrix[ i ][ j ] << " ";
        }
        std::cout << std::endl;
    }
    /* print score matrix */
    std::cout << "Score Matrix Table: " << std::endl;
    for ( int i = 0; i < N; ++i )
    {
        std::cout << tableh->scores[ i ] << std::endl;
    }
    /* print num entries matrix */
    std::cout << "Entries Matrix Table: " << std::endl;
    for ( int i = 0; i < N; ++i )
    {
        std::cout << tablec->num_entries[ i ] << std::endl;
    }
}

void read_inputfile( Tables_Cold *tablec )
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
		
        tablec->visited_matrix[ a ][ b ] = 1;
        tablec->num_entries[ a ] += 1;
	}

	infile.close();
}

void update_entries( Tables_Hot *tableh, Tables_Cold *tablec )
{
    for ( int i = 0; i < N; ++i )
    {
        for ( int j = 0; j < N; ++j )
        {
            if ( tablec->num_entries[ j ] == 0 )
            {
                // dangling node: 1 / N
                tableh->ij_entries_matrix[ i ][ j ] = 1.0 / N;
            }
            else if ( tablec->visited_matrix[ j ][ i ] == 1 )
            {
                // if v(j, i) is visited then a(ij) = 1/L(j)
                tableh->ij_entries_matrix[ i ][ j ] = 1.0 / tablec->num_entries[ j ];
            }
            // else{ table->ij_entries_matrix[ i ][ j ] = 0.0; }
        }
    }
}

void cal_pagerank( Tables_Hot *table )
{
    for ( int i = 0; i < num_iter-1; ++i )
    {
        /* scores from previous iteration */
        std::vector< Score > old_scores = {};
        for ( int j = 0; j < N; ++j )
        {
            old_scores.push_back( table->scores[ j ] );
        }
        /* update pagerank scores */
        for ( int j = 0; j < N; ++j )
        {
            double sum = 0.0;
            for ( int k = 0; k < N; ++k )
            {
                sum += old_scores[ k ] * table->ij_entries_matrix[ j ][ k ];
            }
            table->scores[ j ] = sum;
        }
    }
}

int main ()
{
    /* initialize matrix table */
    Tables_Hot* th = new struct Tables_Hot( { std::vector< std::vector< Entry > > {}, std::vector< Score > {} } );
    Tables_Cold* tc = new struct Tables_Cold( { std::vector< std::vector< Count > > {}, std::vector< Count > {} } );
    
    for ( int i = 0; i < N; ++i )
    {
        tc->visited_matrix.push_back( std::vector< Count > {} );
        th->ij_entries_matrix.push_back( std::vector< Entry > {} );
        for ( int j = 0; j < N; ++j )
        {
            tc->visited_matrix[ i ].push_back( 0 ); // 0: unvisited, 1: visited
            th->ij_entries_matrix[ i ].push_back( 0.0 );
        }
        th->scores.push_back( 1.0 / N );
        tc->num_entries.push_back( 0 );
    }

    read_inputfile( tc );
    update_entries( th, tc );
    /* timing the pagerank algorithm */
    auto const start_time = std::chrono::steady_clock::now();
    cal_pagerank( th );
    auto const end_time = std::chrono::steady_clock::now();
    // print_scores( th );
    print_score_sum( th );
    std::cout << "Calculation time = "
		      << std::chrono::duration_cast<std::chrono::microseconds>( end_time - start_time ).count()
		      << " us" << std::endl;
    return 0;
}