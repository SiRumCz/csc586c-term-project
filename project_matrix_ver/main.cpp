/*
 * reference: http://home.ie.cuhk.edu.hk/~wkshum/papers/pagerank.pdf 
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "pagerank.hpp"

using namespace csc586_matrix;
using namespace csc586_matrix::soa_matrix;

/* global variables */
const short int N = 12; // number of nodes
const short int num_iter = 100; // number of pagerank iterations
const std::string filename = "../test/chvatal.txt";

void print_scores( Tables *table )
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

void print_table( Tables *table )
{
    /* print visited matrix */
    std::cout << "Entry Visited Table: " << std::endl;
    for ( int i = 0; i < N; ++i )
    {
        for ( int j = 0; j < N; ++j )
        {
            std::cout << table->visited_matrix[ i ][ j ] << " ";
        }
        std::cout << std::endl;
    }
    /* print entry matrix */
    std::cout << "Entry Matrix Table: " << std::endl;
    for ( int i = 0; i < N; ++i )
    {
        for ( int j = 0; j < N; ++j )
        {
            std::cout << table->ij_entries_matrix[ i ][ j ] << " ";
        }
        std::cout << std::endl;
    }
    /* print score matrix */
    std::cout << "Score Matrix Table: " << std::endl;
    for ( int i = 0; i < N; ++i )
    {
        std::cout << table->scores[ i ] << std::endl;
    }
    /* print num entries matrix */
    std::cout << "Entries Matrix Table: " << std::endl;
    for ( int i = 0; i < N; ++i )
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
    short int a, b;
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
    for ( int i = 0; i < N; ++i )
    {
        for ( int j = 0; j < N; ++j )
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
    Tables* t = new struct Tables( { std::vector< std::vector< Count > > {}, std::vector< std::vector< Entry > > {}, std::vector< Score > {}, std::vector< Count > {} } );
    for ( int i = 0; i < N; ++i )
    {
        t->visited_matrix.push_back( std::vector< Count > {} );
        t->ij_entries_matrix.push_back( std::vector< Entry > {} );
        for ( int j = 0; j < N; ++j )
        {
            t->visited_matrix[ i ].push_back( 0 ); // 0: unvisited, 1: visited
            t->ij_entries_matrix[ i ].push_back( 0.0 );
        }
        t->scores.push_back( 1.0 / N );
        t->num_entries.push_back( 0 );
    }

    read_inputfile( t );
    update_entries( t );
    cal_pagerank( t );
    print_scores( t );

    return 0;
}