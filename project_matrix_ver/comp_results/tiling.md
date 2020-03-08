### Tiling

Since we are trying to achieve spatial locality improvement by applying tiling, we look into our basline implementation for PageRank and examine the data access pattern used in each matrix operation. We start by comparing baseline data SoA vs hot and cold data SoAoS. 
