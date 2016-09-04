#include "dlx.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

using std::vector;
using std::size_t;
using std::cout;
using std::endl;
using std::stack;

namespace {
static void link_row(vector<dlx_node>& v)
{
    if (v.empty())
        return;

    // Link along the row
    for (size_t i = 0; i < v.size()-1; ++i) {
        v[i].right = &v[i+1];
        v[i+1].left = &v[i];
    }
    v[0].left = &v.back();
    v.back().right = &v[0];
}

static void link_columns(vector<dlx_node>& v, vector<dlx_node>& col_heads)
{
    // Link columns
    for (auto& x : v) {
        x.head     = &col_heads[x.column];
        auto head  = x.head;
        x.down     = head;
        x.up       = head->up;
        x.down->up = &x;
        x.up->down = &x;
        ++head->count;
    }
}
} // anonymous namespace

void DLX::build_nodes(vector<vector<size_t>>& def)
{
    nodes_.resize(def.size());
    for (size_t r = 0; r < nodes_.size(); ++r) {

        auto& v = nodes_[r];
        v.reserve(def[r].size());

        for (size_t c : def[r]) {
            v.emplace_back(r, c);
        }

        // Setup the links
        link_row(v);
    }

}

DLX::DLX(vector<vector<size_t>> definition, size_t req_columns)
    : definition_(definition),
      required_(req_columns),
      solution_count(0)
{
    // Find out how many columns we actually have
    size_t columns = 0;
    for (auto& v : definition_) {
        auto x = *std::max_element(v.begin(), v.end());
        if (x > columns)
            columns = x;
    }

    col_headers.resize(columns + 1);

    for (size_t c = 0; c < col_headers.size(); ++c) {
        col_headers[c].column = c;
    }
    link_row(col_headers);

    // Create all the nodes from the given input
    build_nodes(definition);

    for (auto& v : nodes_)
        link_columns(v, col_headers);

    // Construct the matrix
    matrix = new dlx_node();

    matrix->right = &col_headers.front();
    matrix->left = &col_headers.back();
    col_headers.front().left = matrix;
    col_headers.back().right = matrix;
}

DLX::~DLX()
{
    delete matrix;
}

/**
 * Specify a row that must be included in the solution.
 * Used for additional constraints, such as specifying a unique sudoku problem
 *
 * @param   row     Row ID to include
 */
void DLX::require_row(size_t row)
{
    if (row > definition_.size())
        throw std::invalid_argument("Must provide valid row ID.");

    // Get a pointer to a place in the row
    dlx_node* ptr = matrix->right;
    auto column = definition_[row][0];
    while (ptr->column != column && ptr != matrix)
        ptr = ptr->right;

    if (ptr == matrix)
        throw std::invalid_argument("Incompatible requirements.");

    auto rp = ptr->down;
    while (rp->row != row && rp != ptr)
        rp = rp->down;

    if (rp == ptr)
        throw std::invalid_argument("Incompatible requirements.");

    solutions.push(row);

    ptr = rp;

    do {
        cover_column(ptr->head);
        ptr = ptr->right;
    } while (ptr != rp);
}

/**
 * Returns a vector of the row indices that solve the cover
 * Use this to only search for the first solution
 *
 * @return  Vector of indices indication the first solution
 */
vector<size_t> DLX::find_solution()
{
    search(true);

    stack<size_t> tmp(solutions);
    vector<size_t> ret;

    while (!tmp.empty()) {
        ret.push_back(tmp.top());
        tmp.pop();
    }
    return ret;
}

/**
 * Counts all the solutions to the given problem
 *
 * @return  Number of possible solutions
 */
size_t DLX::count_solutions()
{
    search(false);

    return solution_count;
}

/**
 * Search for solutions. Optionally bail out after finding the first
 *
 * @param   all     Setting for finding all solutions or only one
 * @return  True if solution has been found
 */
bool DLX::search(bool all)
{
    if (is_solved()) {
        solution_count++;
        return all;
    }

    auto col_head = smallest_column();

    cover_column(col_head);

    // Loop through rows
    for (auto tmp = col_head->down; tmp != col_head; tmp = tmp->down) {

        // Set current row as a solution
        solutions.push(tmp->row);

        // Cover all columns this row is in
        for (auto rp = tmp->right; rp != tmp; rp = rp->right)
            cover_column(rp->head);

        // Run recursively
        if (search(all))
            return true;

        // Back up
        solutions.pop();
        for (auto rp = tmp->right; rp != tmp; rp = rp->right)
            uncover_column(rp->head);
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
bool DLX::is_solved() const
{
    // Check only for required columns
    return matrix->right->column >= required_;
}

/**
 * Cover a column
 *
 * @param   col_header  Pointer to column head
 */
void DLX::cover_column(dlx_node* col_header)
{
    // Cover header first
    col_header->right->left = col_header->left;
    col_header->left->right = col_header->right;

    // Go down and right for cover
    for (auto tmp_col = col_header->down; tmp_col != col_header; tmp_col = tmp_col->down) {
        // Cover this row
        for (auto rp = tmp_col->right; rp != tmp_col; rp = rp->right) {
            rp->up->down = rp->down;
            rp->down->up = rp->up;
            --rp->head->count;
        }
    }
}

/**
 * Uncover a column
 *
 * @param   col_header  Pointer to column head
 */
void DLX::uncover_column(dlx_node* col_header)
{
    // Loop 'up' over the column
    for (auto tmp_col = col_header->up; tmp_col != col_header; tmp_col = tmp_col->up) {
        // Uncover this row
        for (auto rp = tmp_col->right; rp != tmp_col; rp = rp->right) {
            rp->up->down = rp;
            rp->down->up = rp;
            ++rp->head->count;
        }
    }

    // Uncover header last
    col_header->right->left = col_header;
    col_header->left->right = col_header;
}

/**
 * Find the smallest column
 *
 * @return  Pointer to head of smallest column
 */
dlx_node* DLX::smallest_column() const
{
    size_t min_count = SIZE_MAX;
    dlx_node* smallest_column = nullptr;

    for (auto col = matrix->right; col != matrix; col = col->right) {
        // Skip non-required columns
        if (col->column >= required_)
            break;

        if (col->count < min_count) {
            min_count = col->count;
            smallest_column = col;
        }
    }
    return smallest_column;
}

