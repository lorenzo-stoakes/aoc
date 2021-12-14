#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>

struct poly_state {
	std::string base;
	std::unordered_map<std::string, char> mappings;
};

poly_state parse_input()
{
	poly_state ret;

	// Get base.
	std::cin >> ret.base;

	std::string key;
	while ((std::cin >> key)) {
		std::string dummy; // The " -> "
		std::cin >> dummy;
		char val;
		std::cin >> val;
		ret.mappings[key] = val;
	}

	return ret;
}

void print_state(const poly_state& state)
{
	std::cout << state.base << std::endl << std::endl;
	for (auto& [ k, v ] : state.mappings) {
		std::cout << k << " -> " << v << std::endl;
	}
}

std::unordered_map<std::string, int64_t> init_pair_counts(const poly_state& state)
{
	std::unordered_map<std::string, int64_t> ret;

	const std::string& base = state.base;

	for (int i = 0; i < base.size() - 1; i++) {
		ret[base.substr(i, 2)]++;
	}

	return ret;
}

void substitute(poly_state& state,
		std::unordered_map<std::string, int64_t>& pair_counts)
{
	// TODO: Not efficient.
	std::unordered_map<std::string, int64_t> prev = pair_counts;

	for (const auto& [ pair, count ] : prev) {
		const char chr = state.mappings[pair];

		std::string pair1(2, ' ');
		std::string pair2(2, ' ');

		pair1[0] = pair[0];
		pair1[1] = chr;

		pair2[0] = chr;
		pair2[1] = pair[1];

		pair_counts[pair] -= count;
		pair_counts[pair1] += count;
		pair_counts[pair2] += count;
	}
}

int64_t calc_delta(const char start_letter,
		   const std::unordered_map<std::string, int64_t>& pair_counts)
{
	std::unordered_map<char, int64_t> counts;

	counts[start_letter]++;
	for (const auto& [ pair, count ] : pair_counts) {
		counts[pair[1]] += count;
	}

	int64_t min = std::numeric_limits<int64_t>::max();
	int64_t max = std::numeric_limits<int64_t>::min();
	for (const auto& [ chr, count ] : counts) {
		//std::cout << std::string(1, chr) << " -> " << count << std::endl;

		min = std::min(min, count);
		max = std::max(max, count);
	}

	return max - min;
}

int main()
{
	poly_state state = parse_input();

	std::unordered_map<std::string, int64_t> pair_counts = init_pair_counts(state);

	for (int i = 0; i < 40; i++) {
		substitute(state, pair_counts);
	}

	std::cout << calc_delta(state.base[0], pair_counts) << std::endl;

	return 0;
}
