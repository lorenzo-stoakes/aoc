#include <cstdint>
#include <iostream>
#include <vector>

std::vector<uint8_t> parse_input()
{
	std::string line;
	std::cin >> line;

	std::vector<uint8_t> ret;
	ret.reserve(line.size());

	for (int i = 0; i < line.size(); i += 2) {
		uint8_t val = 0;

		char chr = line[i];
		val = chr >= 'A' ? 10 + chr - 'A' : chr - '0';
		val <<= 4;
		chr = line[i + 1];
		val |= chr >= 'A' ? 10 + chr - 'A' : chr - '0';

		ret.push_back(val);
	}

	return ret;
}

struct packet {
	int version = -1;
	int type = -1; // 4 = literal, otherwise operator.
	// ASSUME: Max 64-bit value.
	uint64_t value = 0; // length if operator, value if literal.
	std::vector<packet *> sub_packets;
};

uint64_t get_next_bits(const std::vector<uint8_t>& bytes,
		       int num_bits, int& bit_offset)
{
	if (num_bits > 64)
		throw std::runtime_error("too many bits!");

	const int total_bits = num_bits;

	uint64_t ret = 0;

	// ...#####|########|###.....
	int overhang = bit_offset % 8;

	if (overhang > 0) {
		// We offset high bit -> low bit.
		// ..#.....
		// ...#####
		const int remains = 8 - overhang;
		const uint8_t mask = (1 << remains) - 1;
		int num = bytes[bit_offset / 8] & mask;

		if (remains > num_bits) {
			num >>= remains - num_bits;
			bit_offset += num_bits;
			num_bits = 0;
		} else {
			bit_offset += remains;
			num_bits -= remains;
		}

		ret = num;
	}

	// Get full bytes.
	const int byte_count = num_bits / 8;
	for (int i = 0; i < byte_count; i++) {
		ret <<= 8;
		ret |= bytes[bit_offset / 8];
		bit_offset += 8;
		num_bits -= 8;
	}

	// Get last remaining bits.
	// |###.....
	overhang = num_bits;
	if (overhang > 0) {
		uint8_t num = bytes[bit_offset / 8];

		num >>= (8 - overhang);
		ret <<= overhang;
		ret |= num;
		bit_offset += overhang;
	}

	ret &= ((1UL << total_bits) - 1);
	return ret;
}

packet parse_packets(const std::vector<uint8_t>& bytes,
		     int& bit_offset)
{
	packet ret;

	ret.version = (int)get_next_bits(bytes, 3, bit_offset);
	ret.type = (int)get_next_bits(bytes, 3, bit_offset);

	// Literal.
	if (ret.type == 4) {
		ret.value = 0;

		int read;
		do {
			read = get_next_bits(bytes, 1, bit_offset);

			ret.value <<= 4;
			int next_block = get_next_bits(bytes, 4, bit_offset);
			ret.value |= next_block;
		} while (read);

		return ret;
	}

	// OK operator packet.

	int length_type = get_next_bits(bytes, 1, bit_offset);

	if (length_type == 0) {
		// Read a 15-bit number indicating remaining subpacket bits.
		int length = get_next_bits(bytes, 15, bit_offset);

		for (int curr = bit_offset; bit_offset < curr + length;) {
			packet* sub = new packet();
			*sub = parse_packets(bytes, bit_offset);
			ret.sub_packets.push_back(sub);
		}

		return ret;
	}

	// Otherwise length_type is 1.

	// Read an 11-bit number indicating remaining _number_ of subpackets.
	int num_packets = get_next_bits(bytes, 11, bit_offset);
	for (int i = 0; i < num_packets; i++) {
		packet* sub = new packet();
		*sub = parse_packets(bytes, bit_offset);
		ret.sub_packets.push_back(sub);
	}


	return ret;
}

packet parse_packets(const std::vector<uint8_t>& bytes)
{
	int bit_offset = 0;
	return parse_packets(bytes, bit_offset);
}

int sum_versions(const packet* p)
{
	int ret = p->version;

	for (const packet* sub : p->sub_packets) {
		ret += sum_versions(sub);
	}

	return ret;
}

int main()
{
	std::vector<uint8_t> bytes = parse_input();
	packet p = parse_packets(bytes);

	std::cout << sum_versions(&p) << std::endl;

	return 0;
}
