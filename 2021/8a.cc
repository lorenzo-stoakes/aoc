#include <algorithm>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

/*
 *   0:      1:      2:      3:      4:
 *  aaaa    ....    aaaa    aaaa    ....
 * b    c  .    c  .    c  .    c  b    c
 * b    c  .    c  .    c  .    c  b    c
 *  ....    ....    dddd    dddd    dddd
 * e    f  .    f  e    .  .    f  .    f
 * e    f  .    f  e    .  .    f  .    f
 *  gggg    ....    gggg    gggg    ....
 *
 *   5:      6:      7:      8:      9:
 *  aaaa    aaaa    aaaa    aaaa    aaaa
 * b    .  b    .  .    c  b    c  b    c
 * b    .  b    .  .    c  b    c  b    c
 *  dddd    dddd    ....    dddd    dddd
 * .    f  e    f  .    f  e    f  .    f
 * .    f  e    f  .    f  e    f  .    f
 *  gggg    gggg    ....    gggg    gggg
 */

// 2 -> [ 1 ]
// 3 -> [ 7 ]
// 4 -> [ 4 ]
// 5 -> [ 2, 3, 5 ]
// 6 -> [ 0, 6, 9 ]
// 7 -> [ 8 ]

struct readout {
	std::vector<std::string> patterns;
	std::vector<std::string> digit_patterns;
};

std::optional<readout> parse_readout()
{
	readout ret;

	for (int i = 0; i < 10; i++) {
		std::string pattern;
		if (!(std::cin >> pattern))
			return std::nullopt;
		std::sort(pattern.begin(), pattern.end());

		ret.patterns.push_back(pattern);
	}

	std::string dummy;
	std::cin >> dummy;
	if (dummy != "|")
		throw std::runtime_error(dummy);

	for (int i = 0; i < 4; i++) {
		std::string pattern;
		std::cin >> pattern;
		std::sort(pattern.begin(), pattern.end());

		ret.digit_patterns.push_back(pattern);
	}

	return ret;
}

std::vector<readout> parse_input()
{
	std::vector<readout> ret;

	while (true) {
		if (auto r = parse_readout(); r) {
			ret.push_back(*r);
		} else {
			break;
		}
	}
	return ret;
}

int main()
{
	std::vector<readout> readouts = parse_input();

	// Count 1, 4, 7, 8 occurrence.
	int count = 0;

	for (const readout& r : readouts) {
		std::unordered_set<std::string> unique_patterns;
		for (const std::string& pattern : r.patterns) {
			int size = pattern.size();

			if ((size >= 2 && size <= 4) || size == 7)
				unique_patterns.insert(pattern);
		}

		for (const std::string& digit_pattern : r.digit_patterns) {
			if (unique_patterns.find(digit_pattern) != unique_patterns.end())
				count++;
		}
	}

	std::cout << count << std::endl;

	return 0;
}
