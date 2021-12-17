#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>

#define TRY_STEPS (600)
#define VELOCITY_X_FROM (-500)
#define VELOCITY_X_TO (500)

#define VELOCITY_Y_FROM (-400)
#define VELOCITY_Y_TO (400)

struct target {
	int x_from, x_to;
	int y_from, y_to;
};

target parse_input()
{
	target ret;

	std::string dummy;
	std::cin >> dummy; // "target"
	std::cin >> dummy; // "area:"

	std::string xstr;
	std::cin >> xstr;
	xstr = xstr.substr(2); // Skip "x="
	xstr = xstr.substr(0, xstr.size() - 1); // Remove trailing ,
	size_t separator = xstr.find("..");
	ret.x_from = std::atoi(xstr.substr(0, separator).c_str());
	ret.x_to = std::atoi(xstr.substr(separator + 2).c_str());

	std::string ystr;
	std::cin >> ystr;
	ystr = ystr.substr(2); // Skip "y="
	size_t yseparator = ystr.find("..");
	ret.y_from = std::atoi(ystr.substr(0, yseparator).c_str());
	ret.y_to = std::atoi(ystr.substr(yseparator + 2).c_str());

	return ret;
}

void advance(int& x, int& y, int& velocity_x, int& velocity_y)
{
	x += velocity_x;
	y += velocity_y;
	if (velocity_x != 0)
		velocity_x += velocity_x < 0 ? 1 : -1;
	velocity_y--;
}

bool try_velocity(const target& t, int velocity_x, int velocity_y)
{
	int x = 0, y = 0;

	for (int i = 0; i < TRY_STEPS; i++) {
		if (x >= t.x_from && x <= t.x_to && y >= t.y_from && y <= t.y_to)
			return true;

		advance(x, y, velocity_x, velocity_y);
	}

	return false;
}

int get_distinct_count(const target& t)
{
	int count = 0;

	for (int velocity_x = VELOCITY_X_FROM; velocity_x <= VELOCITY_X_TO; velocity_x++) {
		for (int velocity_y = VELOCITY_Y_FROM; velocity_y <= VELOCITY_Y_TO; velocity_y++) {
			if(try_velocity(t, velocity_x, velocity_y))
				count++;
		}
	}

	return count;
}

int main()
{
	target t = parse_input();

	std::cout << get_distinct_count(t) << std::endl;

	return 0;
}
