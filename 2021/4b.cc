#include <cstddef>
#include <iostream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#define GRID_ROWS (5)
#define GRID_COLS (5)

#define SEEN_VALUE (-1)

std::unordered_set<int> grid_numbers;

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

std::vector<int> read_numbers()
{
	std::string line;
	std::cin >> line;
	return split_comma(line);
}

// Sucks but C++ struggles with multidimensional arrays.
using bingo_grid_t = std::vector<std::vector<int>>;

std::pair<bingo_grid_t, bool> read_grid()
{
	bingo_grid_t ret(GRID_ROWS);

	for (int i = 0; i < GRID_ROWS; i++) {
		ret[i] = std::vector<int>(GRID_COLS);
		for (int j = 0; j < GRID_COLS; j++) {
			if (!(std::cin >> ret[i][j]))
				return { ret, false };
			grid_numbers.insert(ret[i][j]);
		}
	}

	return { ret, true };
}

std::vector<bingo_grid_t> read_grids()
{
	std::vector<bingo_grid_t> ret;

	while (true) {
		auto [ grid, ok ] = read_grid();
		if (!ok)
			break;

		ret.push_back(grid);
	}

	return ret;
}

bool check_bingo(bingo_grid_t& grid, int n)
{
	// Mark seen.
	bool hit = false;
	for (int i = 0; i < GRID_ROWS; i++) {
		for (int j = 0; j < GRID_COLS; j++) {
			if (grid[i][j] == n) {
				hit = true;
				grid[i][j] = SEEN_VALUE;
			}
		}
	}

	if (!hit)
		return false;

	// Got a hit, check for bingo.

	bool invalid_rows[GRID_ROWS] = { false };
	bool invalid_cols[GRID_COLS] = { false };

	// Horrifically inefficient.
	for (int i = 0; i < GRID_ROWS; i++) {
		for (int j = 0; j < GRID_COLS; j++) {
			if (grid[i][j] != SEEN_VALUE) {
				invalid_rows[i] = true;
				invalid_cols[j] = true;
			}
		}
	}

	for (int i = 0; i < GRID_ROWS; i++) {
		if (!invalid_rows[i])
			return true;
	}

	for (int i = 0; i < GRID_COLS; i++) {
		if (!invalid_cols[i])
			return true;
	}

	return false;
}

int calc_value(bingo_grid_t& grid, int n)
{
	int sum = 0;
	for (int i = 0; i < GRID_ROWS; i++) {
		for (int j = 0; j < GRID_COLS; j++) {
			int val = grid[i][j];

			if (val != SEEN_VALUE)
				sum += val;
		}
	}

	return sum * n;
}

int main()
{
	auto ns = read_numbers();
	auto grids = read_grids();

	int last_score = 0;

	std::vector<bool> done_grids(grids.size());

	for (const int n : ns) {
		if (grid_numbers.find(n) == grid_numbers.end())
			continue;

		for (int i = 0; i < grids.size(); i++) {
			if (done_grids[i])
				continue;

			auto& grid = grids[i];

			if (check_bingo(grid, n)) {
				done_grids[i] = true;
				last_score = calc_value(grid, n);
			}
		}
	}

	std::cout << last_score << std::endl;

	return 0;
}
