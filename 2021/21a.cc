#include <cstdint>
#include <iostream>
#include <string>
#include <utility>

struct game_state {
	int num_rolled = 0;
	int player1_score = 0;
	int player2_score = 0;
	int player1_position = 0; // We subtract 1 from actual position.
	int player2_position = 0; // for both players.

	int get_losing_player_score()
	{
		return player1_score > player2_score ? player2_score : player1_score;
	}
};

class die {
public:
	int roll()
	{
		int ret = (m_score % 100) + 1;
		m_score++;
		m_num_rolls++;
		return ret;
	}

	int num_rolls() const
	{
		return m_num_rolls;
	}

private:
	int m_score = 0; // We subtract 1 from actual score.
	int m_num_rolls = 0;
};

std::pair<int, int> parse_input()
{
	auto get = [] {
		std::string line;
		std::getline(std::cin, line);
		size_t start = line.rfind(" ") + 1;
		return std::atoi(line.substr(start).c_str());
	};

	return { get(), get() };
}

game_state play(int player1_start_pos, int player2_start_pos)
{
	die d;
	game_state state;
	state.player1_position = player1_start_pos;
	state.player2_position = player2_start_pos;

	while (true) {
		// Player 1.
		int num = d.roll() + d.roll() + d.roll();
		state.player1_position += num;
		state.player1_position %= 10;
		state.player1_score += state.player1_position + 1;
		if (state.player1_score >= 1000)
			break;

		// Player 2.
		num = d.roll() + d.roll() + d.roll();
		state.player2_position += num;
		state.player2_position %= 10;
		state.player2_score += state.player2_position + 1;
		if (state.player2_score >= 1000)
			break;
	}

	state.num_rolled = d.num_rolls();

	return state;
}

int main()
{
	auto [ player1, player2 ] = parse_input();
	game_state final_state = play(player1 - 1, player2 - 1);

	int64_t result = final_state.get_losing_player_score();
	result *= final_state.num_rolled;
	std::cout << result << std::endl;

	return 0;
}
