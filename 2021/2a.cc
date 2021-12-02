#include <algorithm>
#include <iostream>

int main()
{
	std::string direction;
	int magnitude;

	int horizontal = 0;
	int depth = 0;

	while (std::cin >> direction && std::cin >> magnitude) {
		if (direction == "forward") {
			horizontal += magnitude;
		} else if (direction == "down") {
			depth += magnitude;
		} else if (direction == "up") {
			depth = std::max(0, depth - magnitude);
		}
	}

	std::cout << horizontal * depth << std::endl;

	return 0;
}
