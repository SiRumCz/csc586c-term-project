/*
* reference: http://home.ie.cuhk.edu.hk/~wkshum/papers/pagerank.pdf 
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <chrono> // timing
#include <algorithm> // sort

/* global variables */
const int N = 62586; // number of nodes
const int num_iter = 10; // number of pagerank iterations
const std::string filename = "p2p-Gnutella31.txt";
const float d = 0.85f; // damping factor. 0.85 as defined by Google
int const blocksize = 512;

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
__global__
void update_entries( trans_m_col *transition_matrix, vis_m_col *visited_matrix, int *outgoing_table, int N )
{
    int const idx = threadIdx.x + blockIdx.x * blockDim.x;
    int const i = idx / N;
    int const j = idx % N;

    if (i < N && j < N)
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

__global__
void pagerank( float *score_table, float *old_score_table, trans_m_col *transition_matrix, float d, int N )
{
    int const j = threadIdx.x + blockIdx.x * blockDim.x;

    if (j < N)
    {
        /* update pagerank scores */
        float sum = 0.0f;

        for ( auto k = 0; k < N; ++k )
        {
            sum += old_score_table[ k ] * transition_matrix[ j ][ k ];
        }
        score_table[ j ] = d * old_score_table[ j ] + ( 1.0f - d ) * sum;
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
    for ( auto i = 0; i < std::min( 5, N); ++i )
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

int main()
{
    size_t score_t_size = N * sizeof(float);
    size_t out_t_size = N * sizeof(int);
    size_t vis_m_size = N * N * sizeof(int);
    size_t trans_m_size = N * N * sizeof(float);

    vis_m_col *visited_matrix;
    visited_matrix = ( vis_m_col * )malloc( vis_m_size );
    memset(visited_matrix, 0, vis_m_size);

    trans_m_col *transition_matrix;
    transition_matrix = ( trans_m_col * )malloc( trans_m_size );
    memset(transition_matrix, 0, trans_m_size);

    float score_table[ N ] = { 0 };
    std::fill_n(score_table, N, 1.0f / N );
    int outgoing_table[ N ] = { 0 };

    read_inputfile( visited_matrix, outgoing_table );
    
    float *dev_score_table, *dev_old_score_table;
    int *dev_outgoing_table;
    vis_m_col *dev_visited_matrix;
    trans_m_col *dev_transition_matrix;

    cudaMalloc( &dev_score_table, score_t_size );
    cudaMalloc( &dev_old_score_table, score_t_size );
    cudaMalloc( &dev_outgoing_table, out_t_size );
    cudaMalloc( &dev_visited_matrix, vis_m_size );
    cudaMalloc( &dev_transition_matrix, trans_m_size );
    
    cudaMemcpy( dev_score_table, score_table, score_t_size, cudaMemcpyHostToDevice );
    cudaMemcpy( dev_outgoing_table, outgoing_table, out_t_size, cudaMemcpyHostToDevice );
    cudaMemcpy( dev_visited_matrix, visited_matrix, vis_m_size, cudaMemcpyHostToDevice );
    cudaMemcpy( dev_transition_matrix, transition_matrix, trans_m_size, cudaMemcpyHostToDevice );

    auto num_blocks = ceil( N * N / static_cast< float >( blocksize ) );
    /* timing the pre processing */
    auto start_time = std::chrono::steady_clock::now();
    update_entries<<< num_blocks, blocksize >>>( dev_transition_matrix, dev_visited_matrix, dev_outgoing_table, N );
    auto end_time = std::chrono::steady_clock::now();
    auto const update_duration = std::chrono::duration_cast< std::chrono::microseconds >( end_time - start_time ).count();
    
    num_blocks = ceil( N / static_cast< float >( blocksize ) );
    /* timing the pagerank algorithm */
    start_time = std::chrono::steady_clock::now();
    /* iterations must be serial */
    for ( auto i = 0; i < num_iter - 1; ++i )
    {
        /* scores from previous iteration */
        cudaMemcpy( dev_old_score_table, dev_score_table, score_t_size, cudaMemcpyDeviceToDevice );
        pagerank<<< num_blocks, blocksize >>>( dev_score_table, dev_old_score_table, dev_transition_matrix, d, N );
    }
    cudaMemcpy(score_table, dev_score_table, score_t_size, cudaMemcpyDeviceToHost);
    end_time = std::chrono::steady_clock::now();
    auto const pr_duration = std::chrono::duration_cast< std::chrono::microseconds >( end_time - start_time ).count();

    cudaFree( dev_score_table );
    cudaFree( dev_old_score_table );
    cudaFree( dev_outgoing_table );
    cudaFree( dev_visited_matrix );
    cudaFree( dev_transition_matrix );

    print_top_5( score_table );
    print_total( score_table );

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