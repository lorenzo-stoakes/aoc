#include <algorithm>
#include <cstdint>
#include <deque>
#include <iostream>
#include <unordered_set>
#include <vector>

using matrix_t = std::vector<std::vector<int>>;

matrix_t read_matrix()
{
	matrix_t ret;

	std::string line;
	while ((std::cin >> line)) {
		std::vector<int> ns(line.size());
		for (int i = 0; i < ns.size(); i++) {
			ns[i] = line[i] - '0';
		}
		ret.push_back(ns);
	}

	return ret;
}

void print_matrix(const matrix_t& matrix)
{
	for (const auto& row : matrix) {
		for (const int n : row) {
			std::cout << n;
		}
		std::cout << std::endl;
	}
}

// Use BFS to count adjacent higher elements.
int count_basin(const matrix_t& matrix, int row, int col)
{
	const int num_rows = matrix.size();
	const int num_cols = matrix[0].size();

	std::deque<std::pair<int, int>> to_check;
	std::unordered_set<int> seen;

	auto add_neighbours = [&] () {
		const int num = matrix[row][col];

		// Above.
		if (row > 0 && matrix[row - 1][col] >= num)
			to_check.push_back({row - 1, col});

		// Below.
		if (row < num_rows - 1 && matrix[row + 1][col] >= num)
			to_check.push_back({row + 1, col});

		// Left.
		if (col > 0 && matrix[row][col - 1] >= num)
			to_check.push_back({row, col - 1});

		// Right.
		if (col < num_cols - 1 && matrix[row][col + 1] >= num)
			to_check.push_back({row, col + 1});
	};

	// Add initial neighbours.
	add_neighbours();

	int count = 1; // Include source node in count.
	while (!to_check.empty()) {
		std::tie(row, col) = to_check.front();
		to_check.pop_front();

		// Discard 9's.
		if (matrix[row][col] == 9)
			continue;

		int seen_index = row * num_cols + col;
		if (seen.find(seen_index) != seen.end())
			continue;
		seen.insert(seen_index);

		count++;
		add_neighbours();
	}

	return count;
}

static constexpr int NOT_THERE = 10;

int main()
{
	const matrix_t matrix = read_matrix();
	const int num_rows = matrix.size();
	const int num_cols = matrix[0].size();
	int expected_total_basin_count = num_rows * num_cols;

	// Find low points then count the number of basin elements for each.
	std::vector<int> basin_counts;
	for (int row_num = 0; row_num < num_rows; row_num++) {
		for (int col_num = 0; col_num < num_cols; col_num++) {
			const int num = matrix[row_num][col_num];

			// We don't count 9's.
			if (num == 9) {
				expected_total_basin_count--;
				continue;
			}

			const int above = row_num > 0 ? matrix[row_num - 1][col_num] : NOT_THERE;
			const int below = row_num < num_rows - 1 ? matrix[row_num + 1][col_num] : NOT_THERE;
			const int left = col_num > 0 ? matrix[row_num][col_num - 1] : NOT_THERE;
			const int right = col_num < num_cols - 1 ? matrix[row_num][col_num + 1] : NOT_THERE;

			if (num < above && num < below && num < left && num < right) {
				// If 8 nothing can flow into it as we discard 9's.
				if (num == 8) {
					basin_counts.push_back(1);
					continue;
				}

				basin_counts.push_back(count_basin(matrix, row_num, col_num));
			}
		}
	}

	int actual_basin_count = 0;
	for (const int n : basin_counts) {
		actual_basin_count += n;
	}

	if (actual_basin_count != expected_total_basin_count) {
		std::cerr << "ERROR: elements in basin = " << actual_basin_count <<
			" expected = " << expected_total_basin_count << std::endl;
		return 1;
	}

	// Multiply top 3.
	std::sort(basin_counts.begin(), basin_counts.end(), std::greater<>());
	int product = basin_counts[0] * basin_counts[1] * basin_counts[2];
	std::cout << product << std::endl;

	return 0;
}
