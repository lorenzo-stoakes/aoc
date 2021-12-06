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

void update_fish(std::vector<int>& fish)
{
	int new_fish = 0;

	for (int i = 0; i < fish.size(); i++) {
		int f = fish[i];

		if (f == 0) {
			fish[i] = 6;
			new_fish++;
		} else {
			fish[i]--;
		}
	}

	for (int i = 0; i < new_fish; i++) {
		fish.push_back(8);
	}
}

int main()
{
	std::string line;
	std::cin >> line;

	std::vector<int> fish = split_comma(line);

	for (int i = 0; i < 80; i++) {
		update_fish(fish);
	}

	std::cout << fish.size() << std::endl;

	return 0;
}
