#pragma once

#include <vector>
#include <stack>
#include <set>

struct row_node {
    std::vector<std::size_t> cols;
};

struct col_node {
    std::vector<std::size_t> rows;
    bool covered = false;
};

class DLX_Containers {
    public:

        DLX_Containers(std::vector<std::vector<std::size_t>> definition,
            std::size_t req_columns);

        // Return vector of row indices satisfying the cover
        std::vector<std::size_t> find_solution();

        // Return the number of solutions
        std::size_t count_solutions();

    private:
        // Functions
        bool is_solved() const;
        bool search();

        void cover_column(std::size_t col);
        void uncover_column(std::size_t col);

        std::size_t smallest_column() const;

        // Data
        std::vector<row_node> rows_;
        std::vector<col_node> cols_;

        std::vector<std::vector<std::size_t>> definition_;
        std::size_t required_;
        std::stack<std::size_t> solutions;
        std::size_t solution_count;
        std::size_t covered_cols;
};

