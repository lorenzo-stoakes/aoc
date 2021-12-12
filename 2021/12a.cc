#include <cstddef>
#include <deque>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using map_t = std::unordered_map<std::string, std::vector<std::string>>;

map_t parse_map()
{
	map_t ret;

	std::string line;
	while (std::cin >> line) {
		size_t offset = line.find("-");
		std::string from = line.substr(0, offset);
		std::string to = line.substr(offset + 1);

		ret[from].push_back(to);

		if (from != "start" && to != "end")
			ret[to].push_back(from);
	}

	return ret;
}

int calc_num_paths(map_t& map, const std::string& curr,
		   // We intentionally copy this each time.
		   std::unordered_set<std::string> seen)
{
	if (curr == "end")
		return 1;

	if (std::islower(curr[0]))
		seen.insert(curr);

	int count = 0;
	for (const std::string& child : map[curr]) {
		if (seen.find(child) != seen.end())
			continue;

		count += calc_num_paths(map, child, seen);
	}

	return count;
}

int calc_num_paths(map_t& map)
{
	std::unordered_set<std::string> seen;
	return calc_num_paths(map, "start", seen);
}

void print_map(const map_t& map)
{
	for (auto [ from, to ] : map) {
		std::cout << from << ": ";

		for (const std::string& dest : to) {
			std::cout << dest << " ";
		}
		std::cout << std::endl;
	}
}

int main()
{
	map_t map = parse_map();
	//print_map(map);
	std::cout << calc_num_paths(map) << std::endl;

	return 0;
}
