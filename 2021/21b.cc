#include <cstddef>
#include <cstdint>
#include <deque>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// The key to this one is using a DP-like grid structure tracking score and
// position for each player.

// Another key part is that player 2's grid is PER each state in player 1's grid
// or vice-versa. So to get actual universe size we count wins * size of other
// grid.

using grid_t = std::vector<std::vector<int64_t>>;

static constexpr int64_t MAX_SCORE = 21;
static constexpr int64_t MAX_POS = 10;
static constexpr int64_t SPLIT_UNIVERSES = 3;
static constexpr int64_t NUM_ROLLS = 3;

static constexpr int64_t get_split_count()
{
	int64_t ret = 1;
	for (int64_t i = 0; i < NUM_ROLLS; i++) {
		ret *= SPLIT_UNIVERSES;
	}
	return ret;
}

// Determine the counts of each summed NUM_ROLLS of SPLIT_UNIVERSES-sided dice.
std::unordered_map<int64_t, int64_t> get_position_map()
{
	static std::unordered_map<int64_t, int64_t> map;

	if (!map.empty())
		return map;

	std::deque<int64_t> queue;

	// We roll up to the number of split universes.
	for (int64_t roll = 1; roll <= SPLIT_UNIVERSES; roll++) {
		queue.push_back(roll);
	}

	for (int i = 0; i < NUM_ROLLS - 1; i++) {
		size_t num_rolls = queue.size();

		for (size_t j = 0; j < num_rolls; j++) {
			int64_t prev = queue.front();
			queue.pop_front();

			for (int64_t roll = 1; roll <= SPLIT_UNIVERSES; roll++) {
				queue.push_back(prev + roll);
			}
		}
	}

	for (const int64_t roll : queue) {
		map[roll]++;
	}

	return map;
}

struct player {
	player(int64_t start_pos) : grid{MAX_SCORE}
	{
		for (auto& row : grid) {
			// We store position as 1 to 10 _inclusive_.
			row = std::vector<int64_t>(MAX_POS + 1);
		}

		grid[0][start_pos] = 1;
	}

	// A grid of score and position that stores number of universes in this
	// state.
	grid_t grid;

	// What are the total number of universes that are yet to see wins?
	int64_t count() const
	{
		int64_t ret = 0;

		for (auto& row : grid) {
			for (int64_t val : row) {
				ret += val;
			}
		}

		return ret;
	}

	// Are we done yet?
	bool done() const
	{
		return count() == 0;
	}

	// Clear grid of all values.
	void clear_grid()
	{
		for (auto& row : grid) {
			for (int64_t& val : row) {
				val = 0;
			}
		}
	}

	// Do a move, returns the raw number of wins.
	int64_t move()
	{
		// Take a copy of the grid to read from.
		grid_t from_grid = grid;
		// Then clear it ready for the new values.
		clear_grid();

		int64_t wins = 0;

		for (int64_t score = 0; score < MAX_SCORE; score++) {
			for (int64_t pos = 1; pos <= MAX_POS; pos++) {
				int64_t prev = from_grid[score][pos];
				if (prev == 0)
					continue;

				for (const auto [ spaces, count ] : get_position_map()) {
					int64_t new_count = prev * count;
					int64_t new_pos = pos + spaces;
					new_pos = new_pos > 10 ? new_pos - 10 : new_pos;
					int64_t new_score = score + new_pos;

					if (new_score >= MAX_SCORE) {
						wins += new_count;
					} else {
						grid[new_score][new_pos] += new_count;
					}
				}
			}
		}

		return wins;
	}

	void print(int player_num) const
	{
		std::cout << std::endl << "### PLAYER " << player_num << " ###" << std::endl << std::endl;

		for (auto& row : grid) {
			for (size_t pos = 1; pos <= MAX_POS; pos++) {
				std::cout << row[pos] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
		std::cout << "count=" << count() << std::endl;
		std::cout << "---" << std::endl;
	}
};

std::pair<int64_t, int64_t> parse_input()
{
	auto get = [] {
		std::string line;
		std::getline(std::cin, line);
		size_t start = line.rfind(" ") + 1;
		return std::atoi(line.substr(start).c_str());
	};

	return { get(), get() };
}

int main()
{
	const auto [ score1, score2 ] = parse_input();
	player player1(score1), player2(score2);

	// Each player state is expressed in terms of the other's e.g. player
	// 2's number of states is PER each state of player 1. So to reach
	// actual universe size, we must multiply by the size of the other grid.

	int64_t total_wins1 = 0;
	int64_t total_wins2 = 0;
	while (true) {
		const int64_t wins1 = player1.move();
		total_wins1 += wins1 * player2.count();
		if (player1.done())
			break;

		const int64_t wins2 = player2.move();
		total_wins2 += wins2 * player1.count();
		if (player2.done())
			break;
	}

	std::cout << total_wins1 << std::endl;
	std::cout << total_wins2 << std::endl;

	return 0;
}
