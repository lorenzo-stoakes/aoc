#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

std::vector<int> split_comma(const std::string& str)
{
	std::vector<int> ret;

	size_t from = 0;
	size_t to;
	while ((to = str.find(",", from)) != std::string::npos) {
		// Potentially costly, though SSO might make this OK.
		int n = std::atoi(str.substr(from, to).c_str());
		ret.push_back(n);
		from = to + 1;
	}
	// Pick up last number.
	int n = std::atoi(str.substr(from, str.size()).c_str());
	ret.push_back(n);

	return ret;
}

int64_t calc_fuel(const std::vector<int>& ns, int av)
{
	int64_t fuel = 0;
	for (const int n : ns) {
		int dist = abs(n - av);
		fuel += dist * (dist + 1) / 2;
	}

	return fuel;
}

int64_t min_dist(const std::vector<int>& ns)
{
	// First get average (better to minimise maximum distance).
	int sum = 0;
	for (const int n : ns) {
		sum += n;
	}

	// Now calculate each movement to average, try both rounding and not
	// rounding.
	sum += ns.size() / 2;
	int av = sum / ns.size();
	return std::min(calc_fuel(ns, av), calc_fuel(ns, av - 1));
}

int main()
{
	std::string line;
	std::cin >> line;
	std::vector<int> ns = split_comma(line);

	std::cout << min_dist(ns) << std::endl;

	return 0;
}
