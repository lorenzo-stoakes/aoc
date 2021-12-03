#include <cstdint>
#include <iostream>
#include <unordered_set>
#include <vector>

int main()
{
	std::unordered_set<int64_t> nums;
	std::vector<int64_t> targets;

	int64_t num;
	while (std::cin >> num) {
		nums.insert(num);
		targets.push_back(2020 - num);
	}

	// O(n^2)!
	for (const int64_t target : targets) {
		for (const int64_t n : nums) {
			if (nums.find(target - n) != nums.end()) {
				int64_t one = n;
				int64_t two = 2020 - target;
				int64_t three = target - n;

				if (one != two && one != three && two != three) {
					int64_t mult = one * two * three;
					std::cout << mult << std::endl;
					return 0;
				}
			}
		}
	}

	return 1;
}
