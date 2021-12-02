#include <cstdint>
#include <iostream>

int main()
{
	std::string direction;
	int64_t magnitude;

	int64_t horizontal = 0;
	int64_t depth = 0;
	int64_t aim = 0;

	while (std::cin >> direction && std::cin >> magnitude) {
		if (direction == "forward") {
			horizontal += magnitude;
			depth += aim * magnitude;
		} else if (direction == "down") {
			aim += magnitude;
		} else if (direction == "up") {
			aim -= magnitude;
		}
	}

	std::cout << horizontal * depth << std::endl;

	return 0;
}
