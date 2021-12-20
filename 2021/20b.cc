#include <deque>
#include <iostream>
#include <string>
#include <vector>

// If the input mask indicates that we transform a pixel that has all mask
// inputs set to 0 to 1 then after the first iteration we default everything to
// binary 1 as the infinite set of pixels will have been transformed.
static bool default_to_one = false;

using grid_t = std::deque<std::deque<bool>>;

struct trench {
	std::vector<bool> mask;
	grid_t grid;

	int count_lit()
	{
		int ret = 0;
		for (const auto& row : grid) {
			for (const bool set : row) {
				if (set)
					ret++;
			}
		}

		return ret;
	}
};

trench parse_input()
{
	std::string mask;
	std::cin >> mask;

	trench ret;
	for (const char chr : mask) {
		ret.mask.push_back(chr == '#');
	}

	std::string line;
	while ((std::cin >> line)) {
		std::deque<bool> row;

		for (const char chr : line) {
			row.push_back(chr == '#');
		}
		ret.grid.push_back(row);
	}

	return ret;
}

void print_trench(const trench& tr)
{
	for (const bool set : tr.mask) {
		std::cout << (set ? "#" : ".");
	}
	std::cout << std::endl << std::endl;

	for (const auto& row : tr.grid) {
		for (const bool set : row) {
			std::cout << (set ? "#" : ".");
		}
		std::cout << std::endl;
	}
}

void add_border(grid_t& grid)
{
	int num_cols = grid[0].size();

	grid.push_front(std::deque<bool>(num_cols, default_to_one));
	grid.push_back(std::deque<bool>(num_cols, default_to_one));

	for (auto& row : grid) {
		row.push_front(default_to_one);
		row.push_back(default_to_one);
	}
}

void apply_mask(trench& tr, const grid_t& input, int source_row, int source_col)
{
	int num_rows = input.size();
	int num_cols = input[0].size();

	int index = 0;
	for (int row = source_row - 1; row <= source_row + 1; row++) {
		for (int col = source_col - 1; col <= source_col + 1; col++) {
			// These count as zeroes therefore do not change index.
			const bool out_of_bounds =
				row < 0 || col < 0 || row >= num_rows || col >= num_cols;

			if (!out_of_bounds)
				index |= input[row][col] ? 1 : 0;
			else
				index |= default_to_one ? 1 : 0;

			index <<= 1;
		}
	}
	// Correct for trailing shift.
	index >>= 1;

	tr.grid[source_row][source_col] = tr.mask[index];
}

void iterate(trench& tr)
{
	// We need to consider outer pixels too so we need to add an empty
	// border.
	add_border(tr.grid);

	int num_rows = tr.grid.size();
	int num_cols = tr.grid[0].size();

	// We take a copy so we can use it as input to modify output in order to
	// achieve a 'simultaneous' change in the grid.
	grid_t input = tr.grid;

	for (int row = 0; row < num_rows; row++) {
		for (int col = 0; col < num_cols; col++) {
			apply_mask(tr, input, row, col);
		}
	}
}

int main()
{
	trench tr = parse_input();

	for (int i = 0; i < 50; i++) {
		iterate(tr);
		// If we set zero values by default, then on next iteration we
		// might clear them, so check whether we need to alternate.
		if (tr.mask[0])
			default_to_one = i % 2 == 0 ? tr.mask[0] : tr.mask[511];
	}

	//print_trench(tr);
	std::cout << tr.count_lit() << std::endl;

	return 0;
}
