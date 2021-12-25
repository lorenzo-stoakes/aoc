#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

enum direction {
	EMPTY,
	RIGHT,
	DOWN,
};

using grid_t = std::vector<std::vector<direction>>;
using decision_grid_t = std::vector<std::vector<bool>>;

grid_t parse_input()
{
	grid_t ret;

	std::string line;
	while ((std::cin >> line)) {
		std::vector<direction> row(line.size());

		for (size_t i = 0; i < line.size(); i++) {
			const char chr = line[i];
			switch (chr) {
			case '.':
				row[i] = EMPTY;
				break;
			case '>':
				row[i] = RIGHT;
				break;
			case 'v':
				row[i] = DOWN;
				break;
			default:
				throw std::runtime_error("Impossible!");
			}
		}

		ret.push_back(row);
	}

	return ret;
}

void print_grid(const grid_t& grid)
{
	for (auto& row : grid) {
		for (direction dir : row) {
			switch (dir) {
			case EMPTY:
				std::cout << ".";
				break;
			case RIGHT:
				std::cout << ">";
				break;
			case DOWN:
				std::cout << "v";
				break;
			}
		}
		std::cout << std::endl;
	}
}

size_t iterate(grid_t& grid, decision_grid_t& dgrid)
{
	const size_t num_rows = grid.size();
	const size_t num_cols = grid[0].size();

	size_t num_moves = 0;

	// Do right herd pass.

	// Decide ahead of time what to move ('simultaneous').
	for (size_t row = 0; row < num_rows; row++) {
		for (size_t col = 0; col < num_cols; col++) {
			const direction dir = grid[row][col];

			dgrid[row][col] = dir == RIGHT && (
				(col != num_cols - 1 && grid[row][col + 1] == EMPTY) ||
				(col == num_cols - 1 && grid[row][0] == EMPTY));
		}
	}

	for (size_t row = 0; row < num_rows; row++) {
		for (size_t col = 0; col < num_cols; col++) {
			const direction dir = grid[row][col];
			if (dir != RIGHT || !dgrid[row][col])
				continue;

			if (col != num_cols - 1 && grid[row][col + 1] == EMPTY) {
				grid[row][col] = EMPTY;
				grid[row][col + 1] = RIGHT;
				col++;
				num_moves++;
			} else if (col == num_cols - 1 && grid[row][0] == EMPTY) { // Far right side.
				grid[row][col] = EMPTY;
				grid[row][0] = RIGHT;
				num_moves++;
			}
		}
	}

	// Decide ahead of time what to move ('simultaneous').
	for (size_t col = 0; col < num_cols; col++) {
		for (size_t row = 0; row < num_rows; row++) {
			const direction dir = grid[row][col];

			dgrid[row][col] = dir == DOWN && (
				(row != num_rows - 1 && grid[row + 1][col] == EMPTY) ||
				(row == num_rows - 1 && grid[0][col] == EMPTY));
		}
	}

	// Do down herd pass. We will go up -> down left -> right.
	for (size_t col = 0; col < num_cols; col++) {
		for (size_t row = 0; row < num_rows; row++) {
			const direction dir = grid[row][col];
			if (dir != DOWN || !dgrid[row][col])
				continue;

			if (row != num_rows - 1 && grid[row + 1][col] == EMPTY) {
				grid[row][col] = EMPTY;
				grid[row + 1][col] = DOWN;
				row++;
				num_moves++;
			} else if (row == num_rows - 1 && grid[0][col] == EMPTY) { // Far bottom side.
				grid[row][col] = EMPTY;
				grid[0][col] = DOWN;
				num_moves++;
			}
		}
	}

	return num_moves;
}

decision_grid_t make_decision_grid(const grid_t& grid)
{
	const size_t num_rows = grid.size();
	const size_t num_cols = grid[0].size();

	decision_grid_t ret(num_rows);
	for (auto & row : ret) {
		row = std::vector<bool>(num_cols);
	}

	return ret;
}

int main()
{
	grid_t grid = parse_input();
	decision_grid_t dgrid = make_decision_grid(grid);
	//print_grid(grid);

	int num_iterations = 1;
	while (iterate(grid, dgrid) > 0) {
		num_iterations++;
	}

	std::cout << num_iterations << std::endl;

	return 0;
}
