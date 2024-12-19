#include <iomanip>
#include <iostream>
#include <string>

#include "dlx.h"

using namespace std;

static void print_nqueens(vector<vector<size_t>> def, vector<size_t> idx) {
  size_t N = idx.size();
  string line(N, '-');

  vector<string> out(N, line);

  for (auto i : idx) {
    // def[i][0]   == row
    // def[i][1]-N == column
    out[def[i][0]][def[i][1] - N] = 'Q';
  }
  for (auto str : out) cout << str << endl;
}

int main(int argc, char *argv[]) {
  // Construct vector of vectors for nqueens problem
  size_t N = 8;  // Default to 8 queens
  if (argc > 1) N = static_cast<size_t>(atoi(argv[1]));

  vector<vector<size_t>> definition;
  definition.reserve(N * N);

  // One entry per square
  for (size_t r = 0; r < N; ++r) {
    for (size_t c = 0; c < N; ++c) {
      definition.emplace_back();
      auto &tmp = definition.back();
      tmp.reserve(4);

      // Row is simply row value
      tmp.push_back(r);

      // Column is N + column
      tmp.push_back(N + c);

      // Diags are more complicated (and optional)
      tmp.push_back(N + N + (N - 1) + r - c);
      tmp.push_back(N + N + N + (N - 1) + c + r);
    }
  }

  DLX tmp(definition, 2 * N);
  cout << "DLX found " << tmp.count_solutions() << " solutions." << endl;

  cout << "First solution found by DLX:" << endl;
  print_nqueens(definition, tmp.find_solution());

  return 0;
}
