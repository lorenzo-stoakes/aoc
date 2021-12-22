// This is the one question where I needed to look for hints on reddit. The
// shame! 3D geometry is not my favourite subject.

#include <iostream>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

struct cuboid {
	bool on = true;
	int x_from = 0, x_to = 0;
	int y_from = 0, y_to = 0;
	int z_from = 0, z_to = 0;

	int64_t volume() const
	{
		int64_t x_len = x_to - x_from + 1;
		int64_t y_len = y_to - y_from + 1;
		int64_t z_len = z_to - z_from + 1;

		return x_len * y_len * z_len;
	}

	bool overlaps(const cuboid& other) const
	{
		return other.x_from <= x_to &&
			other.x_to >= x_from &&
			other.y_from <= y_to &&
			other.y_to >= y_from &&
			other.z_from <= z_to &&
			other.z_to >= z_from;
	}

	cuboid get_overlap(const cuboid& other) const
	{
		if (!overlaps(other))
			throw std::runtime_error("Doesn't overlap?!");

		cuboid ret;

		ret.x_from = std::max(x_from, other.x_from);
		ret.x_to = std::min(x_to, other.x_to);

		ret.y_from = std::max(y_from, other.y_from);
		ret.y_to = std::min(y_to, other.y_to);

		ret.z_from = std::max(z_from, other.z_from);
		ret.z_to = std::min(z_to, other.z_to);

		return ret;
	}

	void print() const
	{
		if (on)
			std::cout << "on ";
		else
			std::cout << "off ";

		std::cout << "x=" << x_from << ".." << x_to << ",";
		std::cout << "y=" << y_from << ".." << y_to << ",";
		std::cout << "z=" << z_from << ".." << z_to << std::endl;
	}
};

// Pretty dire but just getting the job done ugly or otherwise!
std::vector<cuboid> parse_input()
{
	std::vector<cuboid> ret;

	std::string line;
	while (std::getline(std::cin, line)) {
		cuboid in;

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

		ret.push_back(in);
	}

	return ret;
}

void print_cuboids(const std::vector<cuboid>& cuboids)
{
	for (const cuboid& in : cuboids) {
		in.print();
	}
}

// Get volume of cuboids[offset] removing any overlap with cuboids[offset+1:].
int64_t get_non_overlap_volume(const std::vector<cuboid>& cuboids, int offset)
{
	const cuboid& in = cuboids[offset];

	// Firstly determine our overlaps with everything ahead of us.
	std::vector<cuboid> overlaps;
	for (int i = offset + 1; i < cuboids.size(); i++) {
		const cuboid& other = cuboids[i];
		if (!in.overlaps(other))
			continue;

		overlaps.push_back(in.get_overlap(other));
	}

	// Calculate our volume...
	int64_t ret = in.volume();
	// ...Then recursively determine the non-overlapped volume of our
	// overlap (!) and subtract it.
	for (int i = 0; i < overlaps.size(); i++) {
		ret -= get_non_overlap_volume(overlaps, i);
	}

	return ret;
}

int64_t get_volume(const std::vector<cuboid>& cuboids)
{
	int64_t total = 0;

	for (int i = 0 ; i < cuboids.size(); i++) {
		const cuboid& in = cuboids[i];
		//in.print();

		// If off it doesn't count towards ons.
		if (!in.on)
			continue;

		// Determine the volume of this element _excluding_ any
		// forward overlap.
		total += get_non_overlap_volume(cuboids, i);
	}

	return total;
}

int main()
{
	std::vector<cuboid> cuboids = parse_input();
	std::cout << get_volume(cuboids) << std::endl;

	return 0;
}
