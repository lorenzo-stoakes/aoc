#include <iostream>
#include <string>
#include <vector>

int main()
{
	std::string line;

	int count = 0;
	std::vector<int> counts;

	while (std::cin >> line) {
		if (counts.empty()) {
			counts.resize(line.size());
		}
		for (int i = 0; i < line.size(); i++) {
			const char chr = line[i];
			if (chr == '0')
				counts[i]--;
			else
				counts[i]++;
		}
	}

	uint64_t gamma = 0;
	for (int i = 0; i < counts.size(); i++) {
		if (counts[counts.size() - 1 - i] > 0)
			gamma |= 1UL << i;
	}

	uint64_t epsilon = ~gamma & ((1UL << counts.size()) - 1);

	std::cout << gamma * epsilon << std::endl;
}
