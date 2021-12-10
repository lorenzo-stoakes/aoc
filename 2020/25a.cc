#include <cstdint>
#include <iostream>
#include <vector>

int64_t get_loop_size(int64_t encryption_key)
{
	int64_t val = 1;

	int64_t loop_number = 0;

	while (val != encryption_key) {
		val *= 7;
		val %= 20201227;
		loop_number++;
	}

	return loop_number;
}

int64_t transform(int64_t subject_number, int64_t loop_number)
{
	int64_t val = 1;
	for (int64_t i = 0; i < loop_number; i++) {
		val *= subject_number;
		val %= 20201227;
	}

	return val;
}

int main()
{
	int64_t card_public_key;
	std::cin >> card_public_key;
	int64_t door_public_key;
	std::cin >> door_public_key;

	int64_t card_loop_num = get_loop_size(card_public_key);
	int64_t door_loop_num = get_loop_size(door_public_key);

	std::cout << transform(card_public_key, door_loop_num) << std::endl;
	std::cout << transform(door_public_key, card_loop_num) << std::endl;


	return 0;
}
