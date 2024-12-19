#dlx - cpp
A couple implementations of Knuth's Algorithm X, AKA Dancing Links.

Algorithm X is useful for solving [exact cover](https://en.wikipedia.org/wiki/Exact_cover) problems. The most well-known examples are probably N-Queens and Sudoku.

[Knuth's Algorithm X](https://en.wikipedia.org/wiki/Knuth%27s_Algorithm_X) is an algorithm for efficiently solving the exact cover problem. Dancing Links (or DLX) specifically refers to a particular implementation involving a doubly-linked torus.

For details of the algorithm see wikipedia or [Donald Knuth's original paper](https://www.ocf.berkeley.edu/~jchu/publicportal/sudoku/0011047.pdf).

#Details

This library contains 2 separate implementations of Algorithm X.

1. **DLX** - Defined in dlx.h. A clean implementation of Dancing Links.
2. **DLX_Containers** - Defined in dlx_containers.h. Uses stdlib containers to get the same functionality of DLX without any pointers.

Both have identical interfaces, but **DLX** is 3-4x more efficient at the moment. There is essentially no reason to use DLX_Containers in production, but it can be useful to read through if you'd like to understand the algorithm.

#Examples

See test/nqueens.cpp for an example of using the library to solve the N-Queens problem
