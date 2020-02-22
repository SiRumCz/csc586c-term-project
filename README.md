# csc586c-term-project

## Problem Description

[PageRank](https://en.wikipedia.org/wiki/PageRank) was the original idea that differentiated the Google search engine. It still forms a major component of web search rankings. The basic principle is to determine, for each webpage, the probability that a random walk over web links will arrive at that webpage. The principle extends to any sort of graph and has also been used to evaluate the "importance" of a person in a social network.

At web-scale, this is typically solved with distributed computing, but in this project we will take a smaller graph (such as a modest-sized social network of a few hundred thousand people) and use our optimisation and shared-memory parallelism skills to determine the PageRank of every node.

## PageRank

PageRank (PR) is an algorithm used by Google Search to rank websites in their search engine results. PageRank was named after Larry Page, one of the founders of Google. PageRank is a way of measuring the importance of website pages[1]. Also as Google has described, PageRank works by counting the number and quality of links to a page to determine a rough estimate of how important the website is. The underlying assumption is that more important websites are likely to receive more links from other websites.

## Description of PageRank Calculation[3]

Academic citation literature has been applied to the web, largely by counting citations or backlinks to a given page. This gives some approximation of a page's importance or quality. PageRank extends this idea by not counting links from all pages equally, and by normalizing by the number of links on a page. PageRank is defined as follows:

We assume page A has pages T1...Tn which point to it (i.e., are citations). The parameter d is a damping factor which can be set between 0 and 1. We usually set d to 0.85. There are more details about d in the next section. Also C(A) is defined as the number of links going out of page A. The PageRank of a page A is given as follows:

PR(A) = (1-d) + d (PR(T1)/C(T1) + ... + PR(Tn)/C(Tn))

Note that the PageRanks form a probability distribution over web pages, so the sum of all web pages' PageRanks will be one.

PageRank or PR(A) can be calculated using a simple iterative algorithm, and corresponds to the principal eigenvector of the normalized link matrix of the web. Also, a PageRank for 26 million web pages can be computed in a few hours on a medium size workstation.

## Result Comparisons

## References
[1] GeeksforGeeks: https://www.geeksforgeeks.org/page-rank-algorithm-implementation/. (last visited on 02/22/2020).<br>
[2] "Facts about Google and Competition". Archived from the original on 4 November 2011. Retrieved 12 July 2014. https://en.wikipedia.org/wiki/PageRank. (last visited on 02/19/2020)<br>
[3] S. Brin, and L. Page. The Anatomy of a Large-Scale Hypertextual Web Search Engine. http://infolab.stanford.edu/~backrub/google.html. (last visited on 02/22/2020).<br>