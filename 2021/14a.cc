
#include <iostream>
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

void substitute(poly_state& state)
{
	// We are quite reliant on SSO now!
	// We assume we _always_ get a hit.

	const std::string& base = state.base;
	const auto& mappings = state.mappings;

	std::string insertions(base.size() - 1, ' ');

	for (int i = 0; i < base.size() - 1; i++) {
		const std::string pair = base.substr(i, 2);
		auto iter = mappings.find(pair);
		if (iter == mappings.end())
			throw std::runtime_error(std::string("Can't find mapping ") + pair);
		insertions[i] = iter->second;
	}

	size_t new_size = base.size() + insertions.size();
	std::string new_base(new_size, ' ');

	int i = 0, j = 0, k = 0;
	for (; i < base.size() || j < insertions.size(); i++, j++) {
		new_base[k++] = base[i];
		if (j < insertions.size())
			new_base[k++] = insertions[j];
	}

	state.base = new_base;
}

int calc_delta(const std::string& base)
{
	std::unordered_map<char, int> counts;

	for (const char chr : base) {
		int count = ++counts[chr];
	}

	int max = 0;
	int min = base.size() + 1;
	for (auto [ chr, count ] : counts) {
		max = std::max(max, count);
		min = std::min(min, count);
	}

	return max - min;
}

int main()
{
	poly_state state = parse_input();

	for (int i = 0; i < 10; i++) {
		substitute(state);
	}

	std::cout << calc_delta(state.base) << std::endl;

	return 0;
}
