#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using grid_t = std::vector<std::vector<int>>;

grid_t read_grid()
{
	grid_t ret;

	std::string line;
	while (std::cin >> line) {
		std::vector<int> nums(line.size());

		for (int i = 0; i < line.size(); i++) {
			nums[i] = line[i] - '0';
		}

		ret.push_back(nums);
	}

	return ret;
}

void increase_neighbours(grid_t& grid, int row, int col)
{
	int num_rows = grid.size();
	int num_cols = grid[0].size();

	// 3 above.
	if (row > 0) {
		grid[row - 1][col]++;
		if (col > 0)
			grid[row - 1][col - 1]++;
		if (col < num_cols - 1)
			grid[row - 1][col + 1]++;
	}

	// 3 below.
	if (row < num_rows - 1) {
		grid[row + 1][col]++;
		if (col > 0)
			grid[row + 1][col - 1]++;
		if (col < num_cols - 1)
			grid[row + 1][col + 1]++;
	}

	// 2 either side.
	if (col > 0)
		grid[row][col - 1]++;
	if (col < num_cols - 1)
		grid[row][col + 1]++;
}

int do_flash(grid_t& grid)
{
	int flashes = 0;

	int num_rows = grid.size();
	int num_cols = grid[0].size();

	// Get and clear grid marking what flashed or not.
	static std::vector<std::vector<bool>> has_flashed(num_rows);
	for (auto& row : has_flashed) {
		if (row.size() == 0)
			row = std::vector<bool>(num_cols);
		else
			std::fill_n(row.begin(), num_cols, false);
	}

	bool did_flash;
	do {
		did_flash = false;

		for (int row = 0; row < num_rows; row++) {
			for (int col = 0; col < num_cols; col++) {
				if (has_flashed[row][col] || grid[row][col] < 10)
					continue;

				increase_neighbours(grid, row, col);
				flashes++;
				has_flashed[row][col] = true;
				did_flash = true;
			}
		}

	} while (did_flash);

	return flashes;
}

void clear_flashed(grid_t& grid)
{
	for (auto& row : grid) {
		for (int& num : row) {
			if (num > 9)
				num = 0;
		}
	}
}

int loop(grid_t& grid)
{
	// Increment all.
	for (auto& row : grid) {
		for (int& num : row) {
			num++;
		}
	}

	int flashes = do_flash(grid);
	clear_flashed(grid);
	return flashes;
}

int main()
{
	grid_t grid = read_grid();

	int flashes = 0;
	for (int i = 0; i < 100; i++) {
		flashes += loop(grid);
	}

	std::cout << flashes << std::endl;

	return 0;
}
