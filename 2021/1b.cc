#include <deque>
#include <iostream>
#include <string>


int main()
{
	std::deque<int> prev;
	int prev_sum = 0;
	int sum = 0;
	int increase_count = 0;

	int depth;
	while (std::cin >> depth) {
		sum += depth;

		prev.push_back(depth);

		if (prev.size() < 3)
			continue;

		if (prev.size() == 3) {
			prev_sum = sum;
			continue;
		}

		sum -= prev.front();
		prev.pop_front();
		if (sum > prev_sum)
			increase_count++;
		prev_sum = sum;
	}

	std::cout << increase_count << std::endl;

	return 0;
}
