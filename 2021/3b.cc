#include <iostream>
#include <string>
#include <vector>

struct node {
	int count = 0;
	int num = 0;
	struct node *zeroes = nullptr, *ones = nullptr;
};

int main()
{
	node head;

	std::string line;
	while (std::cin >> line) {
		node *n = &head;
		for (const char chr : line) {
			n->num++;

			if (chr == '0') {
				n->count--;
				if (n->zeroes == nullptr)
					n->zeroes = new node;
				n = n->zeroes;
			} else {
				n->count++;
				if (n->ones == nullptr)
					n->ones = new node;
				n = n->ones;
			}
		}
	}

	uint64_t oxygen = 0;
	node *curr = &head;
	while (curr != nullptr && curr->num > 0) {
		if (curr->count >= 0) {
			oxygen |= 1;
			curr = curr->ones;
		} else {
			curr = curr->zeroes;
		}
		oxygen <<= 1;
	}
	// Discard final unnecessary <<=.
	oxygen >>= 1;

	uint64_t scrubber = 0;
	curr = &head;
	while (curr != nullptr && curr->num > 0) {
		if (curr->count >= 0) {
			// Fewer zero bits
			if (curr->num == 1) {
				scrubber |= 1;
				curr = curr->ones;
			} else {
				curr = curr->zeroes;
			}
		} else {
			// Fewer one bits
			if (curr->num == 1) {
				curr = curr->zeroes;
			} else {
				scrubber |= 1;
				curr = curr->ones;
			}
		}
		scrubber <<= 1;
	}
	scrubber >>= 1;

	std::cout << oxygen * scrubber << std::endl;
}
