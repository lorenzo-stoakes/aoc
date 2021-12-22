#include <iostream>
#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

struct input {
	bool on = false;
	int x_from = 0, x_to = 0;
	int y_from = 0, y_to = 0;
	int z_from = 0, z_to = 0;
};

struct cuboid {
	int x, y, z;

	cuboid() : x{0}, y{0}, z{0}
	{}

	explicit cuboid(int x, int y, int z) : x{x}, y{y}, z{z}
	{}

	bool operator==(const cuboid& other) const
	{
		return x == other.x &&
			y == other.y &&
			z == other.z;
	}
};

namespace std
{
template<>
struct hash<cuboid>
{
	std::size_t operator()(const cuboid& c) const
	{
		return c.x ^ (c.y << 1) ^ (c.z << 2);
	}
};
} // namespace std

bool out_of_bounds(const input& in)
{
	auto check = [] (const int& param) {
		return param < -50 || param > 50;
	};

	return check(in.x_from) &&
		check(in.x_to) &&
		check(in.y_from) &&
		check(in.y_to) &&
		check(in.z_from) &&
		check(in.z_to);
}

// Pretty dire but just getting the job done ugly or otherwise!
std::vector<input> parse_input()
{
	std::vector<input> ret;

	std::string line;
	while (std::getline(std::cin, line)) {
		input in;

		in.on = line.find("on") != std::string::npos;

		if (in.on)
			line = line.substr(sizeof("on x=") - 1);
		else
			line = line.substr(sizeof("off x=") - 1);

		size_t next = line.find("..");

		auto read_next = [&line, &next] {
			return std::atoi(line.substr(0, next).c_str());
		};

		in.x_from = read_next();

		line = line.substr(next + sizeof("..") - 1);
		next = line.find(",");
		in.x_to = read_next();

		line = line.substr(next + sizeof(",y=") - 1);
		next = line.find("..");
		in.y_from = read_next();

		line = line.substr(next + sizeof("..") - 1);
		next = line.find(",");
		in.y_to = read_next();

		line = line.substr(next + sizeof(",z=") - 1);
		next = line.find("..");
		in.z_from = read_next();

		line = line.substr(next + sizeof("..") - 1);
		next = line.size();
		in.z_to = read_next();

		if (!out_of_bounds(in))
			ret.push_back(in);
	}

	return ret;
}

void print_input(const input& in)
{
	if (in.on)
		std::cout << "on ";
	else
		std::cout << "off ";

	std::cout << "x=" << in.x_from << ".." << in.x_to << ",";
	std::cout << "y=" << in.y_from << ".." << in.y_to << ",";
	std::cout << "z=" << in.z_from << ".." << in.z_to << std::endl;
}

void print_inputs(const std::vector<input>& inputs)
{
	for (const input& in : inputs) {
		print_input(in);
	}
}

void add_cuboid(std::unordered_set<cuboid>& set, const input& in)
{
	print_input(in);

	for (int x = in.x_from; x <= in.x_to; x++) {
		for (int y = in.y_from; y <= in.y_to; y++) {
			for (int z = in.z_from; z <= in.z_to; z++) {
				cuboid c(x, y, z);
				if (in.on)
					set.insert(c);
				else
					set.erase(c);
			}
		}
	}
}

std::unordered_set<cuboid> gen_cuboids(const std::vector<input>& inputs)
{
	std::unordered_set<cuboid> ret;

	for (const input& in : inputs) {
		add_cuboid(ret, in);
	}

	std::cout << ret.size() << std::endl;

	return ret;
}

int main()
{
	std::vector<input> inputs = parse_input();

	std::unordered_set<cuboid> set = gen_cuboids(inputs);

	return 0;
}
