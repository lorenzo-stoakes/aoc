#include <algorithm>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

// THOUGHTS: This could be done more neatly & vastly more efficiently by mapping
// a - g segments to bits in an integer perhaps using std::bitset<>.

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

void print_mappings(const std::unordered_set<char>* mappings)
{
	for (int i = 0; i < 7; i++) {
		std::cout << "abcdefg"[i] << ": ";
		for (const char chr : mappings[i]) {
			std::cout << chr << " ";
		}
		std::cout << std::endl;
	}
}

int decode_mapping(const std::string& str)
{
	if (str == "cf")
		return 1;
	if (str == "acdeg")
		return 2;
	if (str == "acdfg")
		return 3;
	if (str == "bcdf")
		return 4;
	if (str == "abdfg")
		return 5;
	if (str == "abdefg")
		return 6;
	if (str == "acf")
		return 7;
	if (str == "abcdefg")
		return 8;
	if (str == "abcdfg")
		return 9;
	if (str == "abcefg")
		return 0;

	throw std::runtime_error("what?");
}

int get_digits(const readout& r)
{
	// Determine possible mappings. a - g -> 0 - 6.
	std::unordered_set<char> mappings[7];

	// Everything could map to everything else to start.
	for (int i = 0; i < 7; i++) {
		for (int j = 0 ; j < 7; j++) {
			mappings[i].insert('a' + j);
		}
	}

	auto remove_from_mapping = [&mappings] (const std::string& str, const std::string& remove) {
		for (const char from : str) {
			std::unordered_set<char>& set = mappings[from - 'a'];
			for (const char chr : remove) {
				set.erase(chr);
			}
		}
	};

	auto remove_from_other_mappings = [&mappings] (const std::string& str, const std::string& remove) {
		for (int i = 0; i < 7; i++) {
			const char chr = 'a' + i;
			// Inefficient...
			if (str.find(chr) != std::string::npos)
				continue;

			std::unordered_set<char>& set = mappings[i];
			for (const char chr : remove) {
				set.erase(chr);
			}
		}
	};

	for (const std::string& pattern : r.patterns) {
		switch (pattern.size()) {
		case 2:
			// Only 1 has 2 digits -> [ c, f ].
			remove_from_mapping(pattern, "abdeg");
			remove_from_other_mappings(pattern, "cf");
			break;
		case 3:
			// Only 7 has 3 digits -> [ a, c, f ].
			remove_from_mapping(pattern, "bdeg");
			remove_from_other_mappings(pattern, "acf");
			break;
		case 4:
			// Only 4 has 4 digits -> [ b, c, d, f ].
			remove_from_mapping(pattern, "aeg");
			remove_from_other_mappings(pattern, "bcdf");
			break;
		case 5:
			// Common elements are [ a, d, g ] for [ 2, 3, 5 ].
			remove_from_other_mappings(pattern, "adg");
			break;
		case 6:
			// Common elements are [ a, b, f, g ] for [ 0, 6, 9 ].
			remove_from_other_mappings(pattern, "abfg");
			break;
		case 7:
			// No new information, anything can map to anything.
			break;
		}
	}

	// Use single mappings to eliminate any entries in multi mappings.
	// Inefficient.
	std::string to_remove;
	for (int i = 0; i < 7; i++) {
		const auto& mapping = mappings[i];
		if (mapping.size() > 1)
			continue;

		to_remove += *mapping.begin();
	}
	for (int i = 0; i < 7; i++) {
		auto& mapping = mappings[i];
		if (mapping.size() == 1)
			continue;

		for (const char chr : to_remove) {
			mapping.erase(chr);
		}
	}

	// Now double check nothing is broken...
	for (int i = 0; i < 7; i++) {
		const auto& mapping = mappings[i];
		if (mapping.size() != 1)
			throw std::runtime_error("WTF?");
	}

	// Finally use our mappings to decode.

	int ret = 0;
	int mult = 1000;
	// Now decode.
	for (int i = 0; i < 4; i++) {
		// Copy.
		std::string str = r.digit_patterns[i];

		for (int j = 0; j < str.size(); j++) {
			const char to = *mappings[str[j] - 'a'].begin();
			str[j] = to;
		}
		std::sort(str.begin(), str.end());
		ret += decode_mapping(str) * mult;
		mult /= 10;
	}

	//print_mappings(mappings);
	return ret;
}

int main()
{
	std::vector<readout> readouts = parse_input();

	int sum = 0;
	for (const readout& r : readouts) {
		sum += get_digits(r);
	}

	std::cout << sum << std::endl;

	return 0;
}
