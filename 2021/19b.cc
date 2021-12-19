#include <algorithm>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct coords {
	int x, y, z;

	coords() : x{0}, y{0}, z{0}
	{}

	explicit coords(int x, int y, int z) : x{x}, y{y}, z{z}
	{}

	bool operator==(const coords& other) const
	{
		return x == other.x &&
			y == other.y &&
			z == other.z;
	}
};

namespace std
{
template<>
struct hash<coords>
{
	std::size_t operator()(const coords& c) const
	{
		return c.x ^ (c.y << 1) ^ (c.z << 2);
	}
};
} // namespace std

struct scanner {
	std::vector<coords> scans;

	std::vector<coords> scanner_locations;
};

std::vector<scanner> parse_input()
{
	std::vector<scanner> ret;
	std::string line;

	auto skip_header = [&line] {
		for (int i = 0; i < 3; i++) {
			std::cin >> line;
		}
	};

	// Skip first "--- scanner X ---".
	std::cin >> line;
	skip_header();

	scanner s;
	while ((std::cin >> line)) {
		// --- scanner X ---
		if (line[0] == '-' && line[1] == '-') {
			ret.push_back(s);
			s = scanner();
			skip_header();
			continue;
		}

		coords c;

		size_t comma_pos = line.find(",");
		size_t comma2_pos = line.find(",", comma_pos + 1);

		c.x = std::atoi(line.substr(0, comma_pos).c_str());
		c.y = std::atoi(line.substr(comma_pos + 1, comma2_pos - comma_pos - 1).c_str());
		c.z = std::atoi(line.substr(comma2_pos + 1).c_str());
		s.scans.push_back(c);
	}
	ret.push_back(s);

	return ret;
}

void print_scanner(const scanner& s)
{
	for (const auto& c : s.scans) {
		std::cout << c.x << "," << c.y << "," << c.z << std::endl;
	}
}

void print_scanners(const std::vector<scanner>& scanners)
{
	bool first = true;
	int idx = 0;
	for (const auto& s : scanners) {
		if (first) {
			first = false;
		} else {
			std::cout << std::endl;
		}

		std::cout << "--- scanner ";
		std::cout << idx++ << " ---" << std::endl;

		print_scanner(s);
	}
}

/*
 * Orientations (24):
 *
 *  x  y  z
 *  x -z  y
 *  x -y -z
 *  x  z -y
 * -x -y  z
 * -x -z -y
 * -x  y -z
 * -x  z  y
 *
 *  y -x  z
 *  y -z -x
 *  y  x -z
 *  y  z  x
 * -y -x  z
 * -y  z -x
 * -y -x -z
 * -y -z  x
 *
 *  z  y -x
 *  z  x  y
 *  z -y  x
 *  z -x -y
 * -z  y  x
 * -z -x  y
 * -z -y -x
 * -z  x -y
 */

coords get_permute(const coords& c, int permute_idx)
{
	switch (permute_idx) {
	case 0:
		// x  y  z
		return coords(c.x, c.y, c.z);
	case 1:
		//  x -z  y
		return coords(c.x, -c.z, c.y);
	case 2:
		//  x -y -z
		return coords(c.x, -c.y, -c.z);
	case 3:
		//  x  z -y
		return coords(c.x, c.z, -c.y);
	case 4:
		// -x -y  z
		return coords(-c.x, -c.y, c.z);
	case 5:
		// -x -z -y
		return coords(-c.x, -c.z, -c.y);
	case 6:
		// -x  y -z
		return coords(-c.x, c.y, -c.z);
	case 7:
		// -x  z  y
		return coords(-c.x, c.z, c.y);

	case 8:
		//  y -x  z
		return coords(c.y, -c.x, c.z);
	case 9:
		//  y -z -x
		return coords(c.y, -c.z, -c.x);
	case 10:
		//  y  x -z
		return coords(c.y, c.x, -c.z);
	case 11:
		//  y  z  x
		return coords(c.y, c.z, c.x);
	case 12:
		// -y -x  z
		return coords(-c.y, -c.x, c.z);
	case 13:
		// -y  z -x
		return coords(-c.y, c.z, -c.x);
	case 14:
		// -y -x -z
		return coords(-c.y, -c.x, -c.z);
	case 15:
		// -y -z  x
		return coords(-c.y, -c.z, c.x);

	case 16:
		//  z  y -x
		return coords(c.z, c.y, -c.x);
	case 17:
		//  z  x  y
		return coords(c.z, c.x, c.y);
	case 18:
		//  z -y  x
		return coords(c.z, -c.y, c.x);
	case 19:
		//  z  -x  -y
		return coords(c.z, -c.x, -c.y);
	case 20:
		// -z  y  x
		return coords(-c.z, c.y, c.x);
	case 21:
		// -z -x  y
		return coords(-c.z, -c.x, c.y);
	case 22:
		// -z -y -x
		return coords(-c.z, -c.y, -c.x);
	case 23:
		// -z  x -y
		return coords(-c.z, c.x, -c.y);
	default:
		throw std::runtime_error("wtf?");
	}
}

std::vector<std::vector<coords>> get_permutations(const scanner& s)
{
	std::vector<std::vector<coords>> ret(24);

	for (int i = 0; i < s.scans.size(); i++) {
		const coords& c = s.scans[i];

		for (int i = 0; i < 24; i++) {
			ret[i].emplace_back(get_permute(c, i));
		}
	}

	return ret;
}
std::vector<coords> get_offsets(const std::vector<coords>& coords_base,
				const std::vector<coords>& coords_target)
{
	std::vector<coords> ret;

	for (const coords& c_base : coords_base) {
		for (const coords& c_target : coords_target) {
			ret.emplace_back(c_base.x - c_target.x, c_base.y - c_target.y, c_base.z - c_target.z);
		}
	}

	return ret;
}

std::tuple<int, int, coords> find_scanner(const scanner& base_scanner, const scanner& target_scanner)
{
	// Store known coords.
	std::unordered_set<coords> known;
	for (const coords& c : base_scanner.scans) {
		known.emplace(c.x, c.y, c.z);
	}

	int best_perm = -1;
	int best_offset = -1;
	int best_count = -1;

	std::vector<std::vector<coords>> perms = get_permutations(target_scanner);
	for (int j = 0; j < perms.size(); j++) {
		const std::vector<coords>& perm = perms[j];
		const std::vector<coords> offsets = get_offsets(base_scanner.scans, perm);
		for (int k = 0; k < offsets.size(); k++) {
			const coords& c_offset = offsets[k];

			int count = 0;
			for (const coords& c : perm) {
				coords c_candidate(c.x + c_offset.x, c.y + c_offset.y, c.z + c_offset.z);
				if (known.find(c_candidate) != known.end())
					count++;
			}

			if (count > best_count) {
				best_count = count;
				best_perm = j;
				best_offset = k;
			}
		}
	}

	const std::vector<coords>& perm = perms[best_perm];
	const std::vector<coords> offsets = get_offsets(base_scanner.scans, perm);

	return { best_count, best_perm, offsets[best_offset] };
}

void combine_scanners(std::vector<scanner>& scanners, int i, int j, int perm_idx, coords offset)
{
	scanner& base_scanner = scanners[i];
	scanner& target_scanner = scanners[j];

	std::unordered_set<coords> set(base_scanner.scans.begin(), base_scanner.scans.end());

	// Seriously inefficient...
	std::vector<std::vector<coords>> perms = get_permutations(target_scanner);
	const std::vector<coords>& perm = perms[perm_idx];

	for (const coords& c : perm) {
		set.emplace(c.x + offset.x, c.y + offset.y, c.z + offset.z);
	}

	for (coords loc : target_scanner.scanner_locations) {
		loc = get_permute(loc, perm_idx);
		loc.x += offset.x;
		loc.y += offset.y;
		loc.z += offset.z;

		base_scanner.scanner_locations.push_back(loc); //get_permute(loc, perm_idx));
	}
	base_scanner.scanner_locations.push_back(offset);

	base_scanner.scans.clear();
	for (const coords& c : set) {
		base_scanner.scans.push_back(c);
	}

	scanners.erase(scanners.begin() + j);
}

void combine_scanners(std::vector<scanner>& scanners)
{
	const int target_overlap = 12;

  loop:
	std::cout << "size=" << scanners.size() << std::endl;
	for (int i = 0; i < scanners.size(); i++) {
		scanner& base_scanner = scanners[i];

		for (int j = i + 1; j < scanners.size(); j++) {
			const scanner& target_scanner = scanners[j];

			auto [ count, perm_idx, offset ] = find_scanner(base_scanner, target_scanner);
			if (count >= target_overlap) {
				std::cout << i << ", " << j << ": " << count << std::endl;
				// Put into base and delete target.
				combine_scanners(scanners, i, j, perm_idx, offset);
				goto loop;
			}
		}
	}
}

int man_distance(int a, int b)
{
	return abs(a - b);
}

int man_distance(const coords& a, const coords& b)
{
	return man_distance(a.x, b.x) + man_distance(a.y, b.y) + man_distance(a.z, b.z);
}

int main()
{
	std::vector<scanner> scanners = parse_input();
	//print_scanners(scanners);

	combine_scanners(scanners);

	if (scanners.size() != 1)
		throw std::runtime_error("wtf?");

	int max_dist = 0;
	std::vector<coords>& locations = scanners[0].scanner_locations;

	for (const coords& c : locations) {
		std::cout << c.x << "," << c.y << "," << c.z << std::endl;
	}

	std::cout << "---" << std::endl;

	locations.emplace_back(0,0,0);

	for (int i = 0; i < locations.size(); i++) {
		for (int j = i + 1; j < locations.size(); j++) {
			int dist = man_distance(locations[i], locations[j]);
			if (dist > max_dist) {
				max_dist = dist;
				std::cout << i << ", " << j << std::endl;
			}
		}
	}

	std::cout << max_dist << std::endl;

	return 0;
}
