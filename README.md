# dlx-cpp
A couple implementations of Knuth's Algorithm X, AKA Dancing Links.

Algorithm X is useful for solving [exact cover](https://en.wikipedia.org/wiki/Exact_cover) problems. The most well-known examples are probably N-Queens and Sudoku.

[Knuth's Algorithm X](https://en.wikipedia.org/wiki/Knuth%27s_Algorithm_X) is an algorithm for efficiently solving the exact cover problem. Dancing Links (or DLX) specifically refers to a particular implementation involving a doubly-linked torus.

For details of the algorithm see wikipedia or [Donald Knuth's original paper](https://www.ocf.berkeley.edu/~jchu/publicportal/sudoku/0011047.pdf).

# Details

This library contains 2 separate implementations of Algorithm X.

1. **DLX** - Defined in dlx.h. A clean implementation of Dancing Links.
2. **DLX_Containers** - Defined in dlx_containers.h. 

Both have identical interfaces, but **DLX** is more efficient at the moment. The advantage of **DLX_Containers** is that it's much easier to read (and it's the only one without a memory leak, see Issue [#1](https://github.com/patricksjackson/dlx-cpp/issues/1)).

# Examples

See test/nqueens.cpp for an example of using the library to solve the N-Queens problem
