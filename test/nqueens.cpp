#include "dlx_containers.h"
#include "dlx.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

void print_vector(vector<size_t> inp, size_t width = 2)
{
    for (auto i : inp)
        cout << setw(width) << i << ", ";
    cout << endl;
}

void print_nqueens(vector<vector<size_t>> def, vector<size_t> idx)
{
    int N = idx.size();
    string line(N, '-');

    vector<string> out(N,line);

    for (auto i : idx) {
        // def[i][0]   == row
        // def[i][1]-N == column
        out[def[i][0]][def[i][1] - N] = 'Q';
    }
    for (auto str : out)
        cout << str << endl;
}

int main(int argc, char* argv[])
{
    // Construct vector of vectors for nqueens problem
    int N = 8; // Default to 8 queens
    if (argc > 1)
        N = atoi(argv[1]);

    vector<vector<size_t>> definition;

    // One entry per square
    for (size_t r = 0; r < N; ++r) {
        for (size_t c = 0; c < N; ++c) {
            vector<size_t> tmp;

            // Row is simply row value
            tmp.push_back(r);

            // Column is N + column
            tmp.push_back(N + c);

            // Diags are more complicated (and optional)
            tmp.push_back(N + N + (N - 1 ) + r - c);
            tmp.push_back(N + N + N + (N - 1) + c + r);

            definition.push_back(tmp);
        }
    }

    DLX tmp(definition, 2 * N);
    DLX_Containers tmp_cont(definition, 2 * N);
    
    cout << "DLX found " << tmp.count_solutions() << " solutions." << endl;
    cout << "DLX_Containers found " << tmp_cont.count_solutions() << " solutions." << endl;

    cout << "First solution found by DLX:" << endl;
    print_nqueens(definition, tmp.find_solution());

    return 0;
}