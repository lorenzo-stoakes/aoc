#include <iostream>

int main()
{
	int prev = -1;
	int count = 0;

	int depth;
	while (std::cin >> depth) {
		if (prev > -1 && depth > prev)
			count++;
		prev = depth;
	}

	std::cout << count << std::endl;

	return 0;
}
