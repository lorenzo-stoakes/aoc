#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <vector>

using grid_t = std::vector<std::vector<int>>;

grid_t parse_grid()
{
	grid_t ret;

	std::string line;
	while ((std::cin >> line)) {
		std::vector<int> ns(line.size());
		for (size_t i = 0; i < line.size(); i++) {
			ns[i] = line[i] - '0';
		}

		ret.push_back(ns);
	}

	return ret;
}

void print_grid(const grid_t& grid)
{
	for (const auto& row : grid) {
		for (const int val : row) {
			std::cout << val;
		}
		std::cout << std::endl;
	}
}

struct node {
	node(int row, int col, int risk)
		: row{row}
		, col{col}
		, risk{risk}
	{}

	int row, col;
	int risk; // Cumulative
};

// Dijkstra baby!
int find_shortest_path(const grid_t& grid)
{
	const int num_rows = grid.size();
	const int num_cols = grid[0].size();

	auto compare = [] (const node& left, const node& right) {
		return left.risk > right.risk;
	};

	std::vector<node> queue { node(0, 0, 0) };

	// Use num_cols * row + col to determine.
	std::unordered_set<int> seen;

	auto push_node = [&queue, &grid, &compare] (int row, int col, int risk) {
		queue.emplace_back(row, col, risk + grid[row][col]);
		std::push_heap(queue.begin(), queue.end(), compare);
	};

	while (!queue.empty()) {
		std::pop_heap(queue.begin(), queue.end(), compare);
		node curr = queue.back();
		queue.pop_back();

		const int row = curr.row;
		const int col = curr.col;
		const int risk = curr.risk;

		const int coord_index = row * num_cols + col;
		if (seen.find(coord_index) != seen.end())
			continue;

		if (row == num_rows - 1 && col == num_cols - 1)
			return risk;

		// Up.
		if (row > 0)
			push_node(row - 1, col, risk);

		// Down.
		if (row < num_rows - 1)
			push_node(row + 1, col, risk);

		// Left.
		if (col > 0)
			push_node(row, col - 1, risk);

		// Right.
		if (col < num_cols - 1)
			push_node(row, col + 1, risk);

		seen.insert(coord_index);
	}

	throw std::runtime_error("Did not complete??");
}

int main()
{
	grid_t grid = parse_grid();
	//print_grid(grid);

	std::cout << find_shortest_path(grid) << std::endl;

	return 0;
}
