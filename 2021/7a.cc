#include <algorithm>
#include <cmath>
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

int min_dist(const std::vector<int>& ns)
{
	// First get median.

	std::vector<int> copy = ns;
	std::sort(copy.begin(), copy.end());

	int mid = ns.size() / 2;

	int av = (ns.size() % 2 == 1)
		? copy[mid]
		: (copy[mid - 1] + copy[mid]) / 2;

	// Now calculate each movement to average.
	int dist = 0;
	for (const int n : ns) {
		dist += abs(n - av);
	}

	return dist;
}

int main()
{
	std::string line;
	std::cin >> line;
	std::vector<int> ns = split_comma(line);

	std::cout << min_dist(ns) << std::endl;

	return 0;
}
