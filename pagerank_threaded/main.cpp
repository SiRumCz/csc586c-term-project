/*
 * reference: http://home.ie.cuhk.edu.hk/~wkshum/papers/pagerank.pdf 
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono> // timing
#include <numeric> // accumulate

#include "pagerank.hpp"

using namespace csc586_matrix;
using namespace csc586_matrix::soa_matrix;

/* global variables */
const int N = 10000; // number of nodes
const int num_iter = 10; // number of pagerank iterations
const std::string filename = "../test/erdos-10000.txt";

void print_scores( Tables *table )
{
    /* print score matrix */
    double sum = 0;
    for ( auto i = 0; i < N; ++i )
    {
        sum += table->scores[ i ];
        std::cout << i << "=" << table->scores[ i ] << std::endl;
    }
    std::cout << "s=" << sum << std::endl;
}

void print_score_sum( Tables *table )
{
    double sum =  std::accumulate( table->scores.begin(), table->scores.end(), 0.0 );
    std::cout << "s=" << sum << std::endl;
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
                table->ij_entries_matrix[ i ][ j ] = 1.0 / N;
            }
            else if ( table->visited_matrix[ j ][ i ] == 1 )
            {
                // if v(j, i) is visited then a(ij) = 1/L(j)
                table->ij_entries_matrix[ i ][ j ] = 1.0 / table->num_entries[ j ];
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
        //#pragma omp parallel for
        for ( auto j = 0; j < N; ++j )
        {
            old_scores.push_back( table->scores[ j ] );
        }
        /* update pagerank scores */
        //#pragma omp parallel for
        for ( auto j = 0; j < N; ++j )
        {
            double sum = 0.0;
            for ( auto k = 0; k < N; ++k )
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
    Tables* t = new struct Tables( { std::vector< Score > {},
                                     std::vector< Count > {},
                                     std::vector< std::vector< Count > > {}, 
                                     std::vector< std::vector< Entry > > {} } );
    for ( auto i = 0; i < N; ++i )
    {
        t->visited_matrix.push_back( std::vector< Count > {} );
        t->ij_entries_matrix.push_back( std::vector< Entry > {} );
        for ( auto j = 0; j < N; ++j )
        {
            t->visited_matrix[ i ].push_back( 0 ); // 0: unvisited, 1: visited
            t->ij_entries_matrix[ i ].push_back( 0.0 );
        }
        t->scores.push_back( 1.0 / N );
        t->num_entries.push_back( 0 );
    }

    read_inputfile( t );
    auto const start_time = std::chrono::steady_clock::now();
    update_entries( t );
    /* timing the pagerank algorithm */
    
    cal_pagerank( t );
    auto const end_time = std::chrono::steady_clock::now();
    //print_scores( t );
    print_score_sum( t );
    
    std::cout << "Calculation time = "
		      << std::chrono::duration_cast<std::chrono::microseconds>( end_time - start_time ).count()
		      << " us" << std::endl;
    
    return 0;
}