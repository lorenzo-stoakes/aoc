#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using grid_t = std::vector<std::vector<int>>;

struct fold {
	bool is_x = true;
	int val = 0;
};

std::vector<int> split_comma(const std::string& str)
{
	std::vector<int> ret;

	size_t from = 0;
	size_t to;
	while ((to = str.find(",", from)) != std::string::npos) {
		// Potentially costly, though SSO might make this OK.
		int n = std::atoi(str.substr(from, to).c_str());
		ret.push_back(n);
		from = to + 1;
	}
	// Pick up last number.
	int n = std::atoi(str.substr(from, str.size()).c_str());
	ret.push_back(n);

	return ret;
}

grid_t parse_grid()
{
	std::vector<std::pair<int, int>> coord_list;
	int max_x = 0;
	int max_y = 0;

	std::string line;
	std::cin >> line;
	while (line != "fold") {
		std::vector<int> coords = split_comma(line);
		coord_list.push_back({ coords[0], coords[1] });

		max_x = std::max(max_x, coords[0]);
		max_y = std::max(max_y, coords[1]);

		std::cin >> line;
	}

	grid_t ret(max_y + 1);
	for (auto& row : ret) {
		row = std::vector<int>(max_x + 1);
	}

	for (const auto [ x, y ] : coord_list) {
		ret[y][x] = 1;
	}

	return ret;
}

std::vector<fold> parse_folds()
{
	std::vector<fold> ret;

	// First 'fold' already gobbled.
	bool skip_fold = true;

	std::string word;
	while (true) {
		// "fold"
		if (!(std::cin >> word))
			break;
		// "along"
		if (skip_fold) {
			skip_fold = false;
		} else {
			std::cin >> word;
		}
		// [xy]=n
		std::cin >> word;

		fold f = {
 			.is_x = word[0] == 'x',
			.val = std::atoi(&word.c_str()[2]),
		};
		ret.push_back(f);
	}

	return ret;
}

void print_grid(const grid_t& grid)
{
	for (const auto& row : grid) {
		for (const int val : row) {
			if (val == 0)
				std::cout << ".";
			else
				std::cout << "#";
		}
		std::cout << std::endl;
	}
}

void print_folds(const std::vector<fold>& folds)
{
	for (const fold& f : folds) {
		if (f.is_x) {
			std::cout << "fold x at ";
		} else {
			std::cout << "fold y at ";
		}

		std::cout << f.val << std::endl;
	}
}

void do_fold_y(grid_t& grid, int pivot)
{
	const int num_cols = grid[0].size();
	int from_row = pivot + 1;
	int to_row = pivot - 1;

	while (to_row >= 0 && from_row < grid.size()) {
		for (int i = 0; i < num_cols; i++) {
			grid[to_row][i] += grid[from_row][i];
		}

		from_row++;
		to_row--;
	}

	grid.resize(pivot);
}

void do_fold_x(grid_t& grid, int pivot)
{
	const int num_rows = grid.size();
	const int num_cols = grid[0].size();
	int from_col = pivot + 1;
	int to_col = pivot - 1;

	while (to_col >= 0 && from_col < num_cols) {
		for (int i = 0; i < num_rows; i++) {
			grid[i][to_col] += grid[i][from_col];
		}

		from_col++;
		to_col--;
	}

	for (auto& row : grid) {
		row.resize(pivot);
	}
}

void do_fold(grid_t& grid, const fold& fold)
{
	if (fold.is_x) {
		do_fold_x(grid, fold.val);
	} else {
		do_fold_y(grid, fold.val);
	}
}

int count_dots(const grid_t& grid)
{
	int ret = 0;

	for (const auto& row : grid) {
		for (const int val : row) {
			if (val != 0)
				ret++;
		}
	}

	return ret;
}

int main()
{
	grid_t grid = parse_grid();
	std::vector<fold> folds = parse_folds();

	//print_folds(folds);

	for (const fold& f : folds) {
		do_fold(grid, f);
	}

	print_grid(grid);

	return 0;
}
