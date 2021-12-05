#include <cstddef>
#include <cstdio>
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>

#define MAX_ROWS (1000)
#define MAX_COLS (1000)

struct coords {
	int x, y;
};

struct line {
	coords from, to;
};

// Dirty as hell but works.
std::pair<line, bool> read_coord_pair()
{
	line ret;

	std::string str;
	if (!(std::cin >> str))
		return { ret, false };

	// Read from.
	size_t comma_pos = str.find(",");
	ret.from.x = std::atoi(str.substr(0, comma_pos).c_str());
	ret.from.y = std::atoi(str.substr(comma_pos + 1).c_str());
	// Read "->".
	std::cin >> str;
	// Read to.
	std::cin >> str;
	comma_pos = str.find(",");
	ret.to.x = std::atoi(str.substr(0, comma_pos).c_str());
	ret.to.y = std::atoi(str.substr(comma_pos + 1).c_str());

	return { ret, true };
}

int main()
{
	std::vector<std::vector<int>> grid(MAX_ROWS);
	for (auto& row : grid) {
		row = std::vector<int>(MAX_COLS);
	}

	int count = 0;
	while (true) {
		auto [ line, ok ] = read_coord_pair();
		if (!ok)
			break;

		//std::cout << line.from.x << "," << line.from.y << " -> " << line.to.x << "," << line.to.y << std::endl;

		if (line.from.y == line.to.y) {
			// Horizontal

			int sign = (line.to.x - line.from.x) / std::abs(line.to.x - line.from.x);

			for (int x = line.from.x; x != line.to.x + sign; x += sign) {
				// NOTE: I got _very_ stuck on this because I
				// was saying > 1 here. Which meant we double
				// counted...
				if (++grid[x][line.from.y] == 2)
					count++;
			}
		} else if (line.from.x == line.to.x) {
			// Vertical

			int sign = (line.to.y - line.from.y) / std::abs(line.to.y - line.from.y);

			for (int y = line.from.y; y != line.to.y + sign; y += sign) {
				if (++grid[line.from.x][y] == 2)
					count++;
			}
		} else {
			// Diagonal so ignore.
			continue;
		}
	}

	std::cout << count << std::endl;

	return 0;
}
