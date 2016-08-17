#include "dlx.h"
#include <cstdint>
#include <iostream>
#include <stdexcept>

using std::vector;
using std::size_t;
using std::cout;
using std::endl;
using std::stack;

/**
 * Common node for every entry (and column head) in the matrix
 * It contains extra info for most nodes but makes the code simpler
 */
struct dlx_node {
    dlx_node* up;
    dlx_node* down;
    dlx_node* left;
    dlx_node* right;
    int row;
    int column;
    size_t count;
    dlx_node* head;

    dlx_node()
        : up(this), down(this),
        left(this), right(this),
        row(-1), column(-1), count(0), head(this)
    {}
};


DLX::DLX(vector<vector<size_t>> definition, size_t req_columns)
    : definition_(definition),
      required_(req_columns),
      solution_count(0)
{
    // Construct the matrix
    matrix = new dlx_node();
    for (size_t r = 0; r < definition.size(); ++r)
        for (size_t c : definition[r])
            insert_node(r, c);

    // Remove the unnecessary row headers
    dlx_node* tmp = matrix->down;
    for (auto tmp = matrix->down; tmp != matrix; tmp = tmp->down) {
        tmp->right->left = tmp->left;
        tmp->left->right = tmp->right;
    }
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
    // No guarantee of what order they are in
    for (auto tmp = matrix->right; tmp != matrix; tmp = tmp->right)
        if (tmp->column < required_)
            return false;
    return true;
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
        // Cover this row
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
            continue;

        if (col->count < min_count) {
            min_count = col->count;
            smallest_column = col;
        }
    }
    return smallest_column;
}

/**
 * Insert a new node into the matrix. Expand the matrix if necessary.
 *
 * @param   row     Row value
 * @param   col     Column value
 */
void DLX::insert_node(size_t row, size_t col)
{
    dlx_node* row_header = matrix->down;
    while (row_header != matrix) {
        if (row_header->row == row)
            break;
        row_header = row_header->down;
    }
    if (row_header == matrix) {
        // Row doesn't exist. Insert it
        // Insert above matrix
        row_header = new dlx_node();
        row_header->row = row;
        row_header->down = matrix;
        row_header->up = matrix->up;
        row_header->up->down = row_header;
        matrix->up = row_header;
    }
    dlx_node* col_header = matrix->right;
    while (col_header != matrix) {
        if (col_header->column == col)
            break;
        col_header = col_header->right;
    }
    if (col_header == matrix) {
        // Column doesnt exist. Insert it
        // Insert left of matrix
        col_header = new dlx_node();
        col_header->column = col;
        col_header->right = matrix;
        col_header->left = matrix->left;
        col_header->left->right = col_header;
        matrix->left = col_header;
    }
    // Just stick it above col_header and left of row_header
    dlx_node* tmp = new dlx_node();
    tmp->column = col;
    tmp->row = row;
    
    // Left of row_header
    tmp->right = row_header;
    tmp->left = row_header->left;
    tmp->right->left = tmp;
    tmp->left->right = tmp;

    // Above col_header
    tmp->down = col_header;
    tmp->up = col_header->up;
    tmp->down->up = tmp;
    tmp->up->down = tmp;

    // Add one to col_header
    col_header->count++;
    tmp->head = col_header;
}

