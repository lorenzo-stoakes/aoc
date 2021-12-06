#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

std::vector<int64_t> split_comma(const std::string& str)
{
	std::vector<int64_t> ret;

	size_t from = 0;
	size_t to;
	while ((to = str.find(",", from)) != std::string::npos) {
		// Potentially costly, though SSO might make this OK.
		int64_t n = std::atoi(str.substr(from, to).c_str());
		ret.push_back(n);
		from = to + 1;
	}
	// Pick up last number.
	int64_t n = std::atoi(str.substr(from, str.size()).c_str());
	ret.push_back(n);

	return ret;
}

// Trick: Store number of fish on each day rather than individual fish.
void update_fish(std::vector<int64_t>& fish)
{
	int64_t next_day = fish[8];
	for (int i = 7; i >= 1; i--) {
		std::swap(next_day, fish[i]);
	}
	fish[8] = fish[0];
	fish[6] += fish[0];
	fish[0] = next_day;
}

int main()
{
	std::string line;
	std::cin >> line;

	std::vector<int64_t> fish(9);

	for (const int f : split_comma(line)) {
		fish[f]++;
	}

	for (int64_t i = 0; i < 256; i++) {
		update_fish(fish);
	}

	int64_t sum = 0;
	for (int64_t count : fish) {
		sum += count;
	}

	std::cout << sum << std::endl;

	return 0;
}
