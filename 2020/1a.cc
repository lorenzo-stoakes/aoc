#include <cstdint>
#include <iostream>
#include <unordered_set>

int main()
{
	std::unordered_set<int64_t> nums;

	int64_t num;
	while (std::cin >> num) {
		nums.insert(num);
	}

	for (const int64_t n : nums) {
		if (nums.find(2020 - n) != nums.end()) {
			std::cout << n * (2020 - n) << std::endl;

			break;
		}
	}

	return 0;
}
