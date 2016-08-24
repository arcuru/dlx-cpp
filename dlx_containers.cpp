#include "dlx_containers.h"
#include <cstdint>
#include <iostream>
#include <set>
#include <algorithm>

using std::set;
using std::vector;
using std::size_t;
using std::cout;
using std::endl;
using std::stack;

DLX_Containers::DLX_Containers(vector<vector<size_t>> definition, size_t req_columns)
    : definition_(definition),
      required_(req_columns)
{
    solution_count = 0;
    covered_cols = 0;

    // Construct the matrix
    for (size_t r = 0; r < definition.size(); ++r) {
        for (size_t c : definition[r]) {
            if (r >= rows_.size())
                rows_.resize(r + 1);
            if (c >= cols_.size())
                cols_.resize(c + 1);
            rows_[r].cols.push_back(c);
            cols_[c].rows.push_back(r);
        }
    }
}

vector<size_t> DLX_Containers::find_solution()
{
    search();

    cout << "Solutions: " << solution_count << endl;
    // Print the stack

    stack<size_t> tmp(solutions);
    vector<size_t> ret;

    while (!tmp.empty()) {
        //cout << tmp.top() << endl;
        ret.push_back(tmp.top());
        tmp.pop();
    }
    return ret;
}

size_t DLX_Containers::count_solutions()
{
    search();

    return solution_count;
}

bool DLX_Containers::search()
{
    if (is_solved()) {
        solution_count++;
        return true;
    }

    // Find smallest column
    auto col_head = smallest_column();
    if (cols_[col_head].rows.size() == 0)
        return false;

    // Cover column
    cover_column(col_head);

    // Loop through rows
    for (auto row : cols_[col_head].rows) {

        // Set current row as a solution
        solutions.push(row);

        // Cover all columns this row is in
        for (auto tmp : rows_[row].cols) {
            if (tmp == col_head)
                continue;
            cover_column(tmp);
        }
        
        // Run recursively
        search();

        // Back up
        solutions.pop();
        for (auto tmp : rows_[row].cols) {
            if (tmp == col_head)
                continue;
            uncover_column(tmp);
        }
    }

    // Uncover this column
    uncover_column(col_head);
    return false;
}

/**
 * Checks to see if the matrix is currently solved
 *
 * @return  True if solution has been found
 */
bool DLX_Containers::is_solved() const
{
    return covered_cols == required_;
}

/**
 * Cover a column
 *
 * @param   col     Column index
 */
void DLX_Containers::cover_column(size_t col)
{
    // Cover header first
    cols_[col].covered = true;
    if (col < required_)
        ++covered_cols;

    for (auto tmp : cols_[col].rows) {
        // Cover this row
        for (auto j : rows_[tmp].cols) {
            if (j == col)
                continue;

            // Because we are guaranteed only one, this is more
            // efficient than std::remove()
            auto& x = cols_[j].rows;
            for (auto& i : x) {
                if (i == tmp) {
                    std::swap(i, x.back());
                    x.pop_back();
                    break;
                }
            }
        }
    }
}

/**
 * Uncover a column
 *
 * @param   col     Column index
 */
void DLX_Containers::uncover_column(size_t col)
{
    // Uncover column
    cols_[col].covered = false;
    if (col < required_)
        --covered_cols;

    for (auto tmp : cols_[col].rows) {
        // Uncover this row
        for (auto j : rows_[tmp].cols) {
            if (j == col)
                continue;
            cols_[j].rows.push_back(tmp);
        }
    }
}

/**
 * Find the smallest column
 *
 * @return  Index of smallest column
 */
size_t DLX_Containers::smallest_column() const
{
    size_t min_count = SIZE_MAX;
    size_t smallest_column = 100;

    for (size_t i = 0; i < cols_.size(); ++i) {
        // Skip non-required columns
        if (i >= required_ || cols_[i].covered)
            continue;

        // Count rows left in column
        size_t count = cols_[i].rows.size();
        if (count < min_count) {
            min_count = count;
            smallest_column = i;
        }
    }
    return smallest_column;
}

