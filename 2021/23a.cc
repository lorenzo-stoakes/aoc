#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

// ### # # # ###
//   # # # # #
//   #########
// ^^         ^^
static constexpr size_t BURROW_OFFSET = 2;
static constexpr size_t NUM_BURROWS = 4;
static constexpr size_t BURROW_DEPTH = 2;
static constexpr size_t NUM_ROWS = 1 + BURROW_DEPTH;
static constexpr size_t NUM_COLS = (NUM_BURROWS * 2 - 1) + 2*BURROW_OFFSET;
static constexpr size_t CORRIDOR_WIDTH = NUM_COLS;
static constexpr size_t TOTAL_SPACES = CORRIDOR_WIDTH + NUM_BURROWS * BURROW_DEPTH;
static constexpr size_t NUM_SPACE_STATES = 5;
static constexpr size_t NUM_SPACE_STATES_BITS = 3;

static constexpr uint64_t AMPHIPOD_MASK = (1UL << NUM_SPACE_STATES_BITS) - 1;

static constexpr uint64_t BURROW_BITS = NUM_SPACE_STATES_BITS * BURROW_DEPTH;
static constexpr uint64_t BURROW_MASK = (1UL << BURROW_BITS) - 1;

static constexpr uint64_t CORRIDOR_MASK = (1UL << CORRIDOR_WIDTH) - 1;
static constexpr uint64_t CORRIDOR_BIT_OFFSET = NUM_BURROWS * BURROW_BITS;

static constexpr size_t MAX_MOVES = 20;

// This is the final state represented as a uint64_t.
static uint64_t DONE_ARENA = 0x91b489;

static_assert(TOTAL_SPACES * NUM_SPACE_STATES_BITS < 8*sizeof(uint64_t));

enum amphipod
{
	EMPTY = 0,
	A = 1,
	B = 2,
	C = 3,
	D = 4,
};

uint64_t energy(amphipod a)
{
	switch (a) {
	case A:
		return 1;
	case B:
		return 10;
	case C:
		return 100;
	case D:
		return 1000;
	default:
		throw std::runtime_error("Impossible? " + std::to_string((int)a));
	};
}

amphipod next_amphipod(amphipod a)
{
	int val = static_cast<int>(a);
	val++;
	return static_cast<amphipod>(val);
}

char amphipod_to_char(amphipod a)
{
	if (a == EMPTY)
		return '.';

	return 'A' + static_cast<char>(a) - 1;
}

std::string amphipod_to_string(amphipod a)
{
	return std::string(1, amphipod_to_char(a));
}

// We represent an arena as a 64-bit bitset value.
struct arena {
	arena() : as_bitset{0}
	{}

	// Implicitly convertible from uint64_t.
	arena(uint64_t num) : as_bitset{num}
	{}

	// Implicitly convertible to uint64_t.
	operator uint64_t() const { return as_bitset; }

	explicit arena(const std::vector<amphipod>& amps)
	{
		if (amps.size() != NUM_BURROWS * BURROW_DEPTH)
			throw std::runtime_error("Invalid init!");

		size_t idx = 0;
		for (size_t burrow_idx = 0; burrow_idx < NUM_BURROWS; burrow_idx++) {
			for (size_t depth = 0; depth < BURROW_DEPTH; depth++) {
				set_burrow(burrow_idx, depth, amps[idx++]);
			}
		}
	}

	amphipod get_corridor(size_t idx) const
	{
		uint64_t corridor = as_bitset >> CORRIDOR_BIT_OFFSET;
		const uint64_t offset = idx * NUM_SPACE_STATES_BITS;
		corridor >>= offset;

		return static_cast<amphipod>(corridor & AMPHIPOD_MASK);
	}

	void set_corridor(size_t idx, amphipod amp)
	{
		const uint64_t offset = CORRIDOR_BIT_OFFSET + idx * NUM_SPACE_STATES_BITS;
		const uint64_t clear_mask = ~(AMPHIPOD_MASK << offset);
		const uint64_t val = static_cast<uint64_t>(amp) << offset;

		as_bitset &= clear_mask;
		as_bitset |= val;
	}

	amphipod get_burrow(size_t burrow_idx, size_t depth) const
	{
		const uint64_t offset = burrow_idx * BURROW_BITS +
			depth * NUM_SPACE_STATES_BITS;
		return static_cast<amphipod>((as_bitset >> offset) & AMPHIPOD_MASK);
	}

	void set_burrow(size_t burrow_idx, size_t depth, amphipod amp)
	{
		const uint64_t offset = burrow_idx * BURROW_BITS +
			depth * NUM_SPACE_STATES_BITS;
		const uint64_t clear_mask = ~(AMPHIPOD_MASK << offset);
		const uint64_t val = static_cast<uint64_t>(amp) << offset;

		as_bitset &= clear_mask;
		as_bitset |= val;
	}

	void print() const
	{
		auto put = [](size_t num, const char chr) {
			return std::string(num, chr);
		};

		auto wall = [&put] (size_t num) {
			return put(num, '#');
		};

		auto overhang = [&put] (size_t num) {
			return put(num, ' ');
		};

		std::cout << wall(NUM_COLS + 2) << std::endl;
		std::cout << wall(1);
		for (size_t i = 0; i < CORRIDOR_WIDTH; i++) {
			std::cout << amphipod_to_string(get_corridor(i));
		}
		std::cout << wall(1) << std::endl;

		for (size_t depth = 0; depth < BURROW_DEPTH; depth++) {
			if (depth == 0)
				std::cout << wall(BURROW_OFFSET);
			else
				std::cout << overhang(BURROW_OFFSET);
			std::cout << wall(1);

			for (size_t burrow_idx = 0; burrow_idx < NUM_BURROWS; burrow_idx++) {
				std::cout << amphipod_to_string(get_burrow(burrow_idx, depth));
				std::cout << "#";
			}

			if (depth == 0)
				std::cout << wall(BURROW_OFFSET);
			std::cout << std::endl;
		}
		std::cout << overhang(BURROW_OFFSET)
			  << wall(NUM_COLS - BURROW_OFFSET) << std::endl;

	}

	// Are we in sorted state yet?
	bool is_done() const
	{
		return as_bitset == DONE_ARENA;
	}

	// Calculate moves from burrow to corridor checking if we can actually
	// move there, returning 0 if we can't.
	uint64_t moves_to_corridor(size_t burrow_idx, size_t depth, size_t corridor_idx) const
	{
		// Do we have something to move and are we unblocked in the
		// burrow & finally is the target corridor location empty?
		if (get_burrow(burrow_idx, depth) == EMPTY ||
		    (depth == 1 && get_burrow(burrow_idx, 0) != EMPTY) ||
		    get_corridor(corridor_idx) != EMPTY)
			return 0;

		// TODO: We assume burrow depth, etc.

		// Moves to get into the corridor.
		const uint64_t to_corridor = depth == 0 ? 1 : 2;

		// Our starting position once in corridor.
		const uint64_t pos = BURROW_OFFSET + 2 * burrow_idx;

		// Check each position to make sure we can move there & count
		// moves. We have already checked target corridor spot is empty.
		uint64_t moves = 1;
		const ssize_t delta = corridor_idx < pos ? -1 : 1;
		for (ssize_t idx = (ssize_t)pos + delta; idx != (ssize_t)corridor_idx; idx += delta) {
			if (get_corridor(idx) != EMPTY)
				return 0;
			moves++;
		}

		return to_corridor + moves;
	}

	void append_moves_from_burrow(std::vector<std::pair<uint64_t, arena>>& ret,
				      const size_t burrow_idx,
				      const size_t depth) const
	{
		const amphipod amp = get_burrow(burrow_idx, depth);
		if (amp == EMPTY)
			return;

		const uint64_t cost_per_move = energy(amp);

		// Create a template with this entry removed.
		arena from = *this;
		from.set_burrow(burrow_idx, depth, EMPTY);

		// TODO: Obviously assuming things here!
		// TODO: Efficient?
		for (size_t idx : { 0, 1, 3, 5, 7, 9, 10 }) {
			const uint64_t moves = moves_to_corridor(burrow_idx, depth, idx);
			if (moves == 0)
				continue;

			arena to = from;
			to.set_corridor(idx, amp);
			const uint64_t cost = cost_per_move * moves;
			ret.push_back({ cost, to });
		}
	}

	// Calculate moves from corridor to a burrow position checking if we can
	// actually move there, returning 0 if we can't.
	uint64_t moves_to_burrow(size_t corridor_idx, size_t burrow_idx, size_t depth) const
	{
		// TODO: Assumptions.
		// TODO: Duplication.

		// Do we have something to move and is target burrow location
		// blocked?
		const amphipod amp = get_corridor(corridor_idx);
		if (amp == EMPTY ||
		    get_burrow(burrow_idx, 0) != EMPTY ||
		    (depth == 1 && get_burrow(burrow_idx, 1) != EMPTY))
			return 0;

		// Target burrow not our eventual target?
		const amphipod correct_amp = (amphipod)(burrow_idx + 1);
		if (amp != correct_amp)
			return 0;

		// No point in going above an empty slot, and can't go if other
		// slot contains different amphipod.
		if (depth == 0 && (get_burrow(burrow_idx, 1) == EMPTY ||
				   get_burrow(burrow_idx, 1) != amp))
			return 0;

		// Firstly we need to move to the position _above_ the burrow.
		const size_t target_corridor_idx = BURROW_OFFSET + 2 * burrow_idx;

		// Check each position to make sure we can move there & count
		// moves. We have already checked target burrow spot is empty +
		// unblocked.
		uint64_t moves = 1;
		const ssize_t delta = target_corridor_idx < corridor_idx ? -1 : 1;
		for (ssize_t idx = (ssize_t)corridor_idx + delta; idx != (ssize_t)target_corridor_idx; idx += delta) {
			if (get_corridor(idx) != EMPTY)
				return 0;
			moves++;
		}

		// The number of moves from position above burrow into position.
		const uint64_t to_burrow = depth == 0 ? 1 : 2;
		return to_burrow + moves;
	}

	void append_moves_from_corridor(std::vector<std::pair<uint64_t, arena>>& ret,
					const size_t corridor_idx) const
	{
		// TODO: Duplication.

		const amphipod amp = get_corridor(corridor_idx);
		if (amp == EMPTY)
			return;

		const uint64_t cost_per_move = energy(amp);

		// Create a template with this entry removed.
		arena from = *this;
		from.set_corridor(corridor_idx, EMPTY);

		// We can only move into burrows now.
		for (size_t burrow_idx = 0; burrow_idx < NUM_BURROWS; burrow_idx++) {
			for (size_t depth = 0; depth < BURROW_DEPTH; depth++) {
				const uint64_t moves = moves_to_burrow(corridor_idx, burrow_idx, depth);
				if (moves == 0)
					continue;

				arena to = from;
				to.set_burrow(burrow_idx, depth, amp);
				const uint64_t cost = cost_per_move * moves;
				ret.push_back({ cost, to });
			}
		}
	}

	// Generate valid moves from this position. Expressed as cost, resultant arena pairs.
	void get_moves(std::vector<std::pair<uint64_t, arena>>& moves) const
	{
		// TODO: Be more efficient.
		// TODO: Making assumptions about number of burrows, etc. etc.

		// Moves from burrows:
		for (size_t burrow_idx = 0; burrow_idx < NUM_BURROWS; burrow_idx++) {
			// TODO: Assumptions.

			const amphipod correct_amp = (amphipod)(burrow_idx + 1);
			const bool bottom_correct = get_burrow(burrow_idx, 1) == correct_amp;
			const bool top_correct = get_burrow(burrow_idx, 0) == correct_amp;

			// If both elements are already in place, or the bottom
			// one is, no point in moving.

			if (!bottom_correct || !top_correct)
				append_moves_from_burrow(moves, burrow_idx, 0);

			if (!bottom_correct)
				append_moves_from_burrow(moves, burrow_idx, 1);
		}

		for (size_t idx = 0; idx < CORRIDOR_WIDTH; idx++) {
			append_moves_from_corridor(moves, idx);
		}
	}

	// We store values as a bit set comprising corridor then each burrow
	// using 3 bits to represent state in each position.
	uint64_t as_bitset;
};

arena parse_input()
{
	std::string line;

	std::cin >> line; // #############

	arena ret;

	std::cin >> line; // #...........#

	for (size_t i = 1; i < line.size() - 1; i++) {
		const char chr = line[i];
		if (chr == '.')
			continue;

		const amphipod amp = static_cast<amphipod>(chr - 'A' + 1);
		ret.set_corridor(i - 1, amp);
	}

	for (size_t depth = 0; depth < BURROW_DEPTH; depth++) {
		std::cin >> line; // ###x#x#x#x###

		for (size_t burrow_idx = 0; burrow_idx < NUM_BURROWS; burrow_idx++) {
			size_t offset = 2 * burrow_idx + BURROW_OFFSET + 1;
			// cin will squash space characters.
			const char chr = depth == 0
				? line[offset]
				: line[offset - BURROW_OFFSET];

			if (chr == '.')
				continue;

			const amphipod amp = static_cast<amphipod>(chr - 'A' + 1);

			ret.set_burrow(burrow_idx, depth, amp);
		}
	}

	std::cin >> line; //   #########

	return ret;
}

// Dijkstra baby!
uint64_t get_minimum_cost(arena head)
{
	std::unordered_set<uint64_t> seen;
	// First is cost, second is arena.
	std::vector<std::pair<uint64_t, uint64_t>> queue { { 0, head } };

	auto compare = [] (const std::pair<uint64_t, uint64_t>& pair1,
			   const std::pair<uint64_t, uint64_t>& pair2) {
		return pair1.first > pair2.first;
	};

	std::vector<std::pair<uint64_t, arena>> moves;
	moves.reserve(MAX_MOVES);

	while (!queue.empty()) {
		// Min-heap.
		std::pop_heap(queue.begin(), queue.end(), compare);
		auto [ cost_so_far, arnum ] = queue.back();
		queue.pop_back();

		arena ar = (arena)arnum;

		if (ar.is_done())
			return cost_so_far;

		if (seen.find(arnum) != seen.end())
			continue;

		moves.clear();
		ar.get_moves(moves);
		for (auto [ cost, next ] : moves) {
			queue.push_back({ cost_so_far + cost, next });
			std::push_heap(queue.begin(), queue.end(), compare);
		}

		seen.insert(arnum);
	}

	throw std::runtime_error("Did not complete??");
}

int main()
{
	arena ar = parse_input();
	//ar.print();

	std::cout << get_minimum_cost(ar) << std::endl;

	return 0;
}
