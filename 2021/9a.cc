#include <iostream>
#include <vector>

// Seems graph-y. Also seems possibly bit-y. Mmmm bitty!

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

static constexpr int NOT_THERE = 10;

int main()
{
	matrix_t matrix = read_matrix();

	int sum = 0;

	//print_matrix(matrix);

	int num_rows = matrix.size();
	int num_cols = matrix[0].size();
	for (int row_num = 0; row_num < num_rows; row_num++) {
		for (int col_num = 0; col_num < num_cols; col_num++) {
			int num = matrix[row_num][col_num];

			int above = row_num > 0 ? matrix[row_num - 1][col_num] : NOT_THERE;
			int below = row_num < num_rows - 1 ? matrix[row_num + 1][col_num] : NOT_THERE;
			int left = col_num > 0 ? matrix[row_num][col_num - 1] : NOT_THERE;
			int right = col_num < num_cols - 1 ? matrix[row_num][col_num + 1] : NOT_THERE;

			if (num < above && num < below && num < left && num < right)
				sum += num + 1;
		}
	}

	std::cout << sum << std::endl;

	return 0;
}
