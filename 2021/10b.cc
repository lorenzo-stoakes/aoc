#include <algorithm>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int64_t score_syntax_error(const std::string& line)
{
	std::vector<char> stack;

	// ASUME: We never close when we haven't opened.

	for (const char chr : line) {
		const char prev = stack.empty() ? '\0' : stack.back();

		switch (chr) {
		case '(':
		case '[':
		case '{':
		case '<':
			stack.push_back(chr);
			break;
		case ')':
			if (prev != '(')
				return -1;
			stack.pop_back();
			break;
		case ']':
			if (prev != '[')
				return -1;
			stack.pop_back();
			break;
		case '}':
			if (prev != '{')
				return -1;
			stack.pop_back();
			break;
		case '>':
			if (prev != '<')
				return -1;
			stack.pop_back();
			break;
		default:
			throw std::runtime_error("impossible!");
		}
	}

	// Now we are left only with incomplete. Score 'em.
	int64_t score = 0;

	while (!stack.empty()) {
		score *= 5;
		const char chr = stack.back();
		stack.pop_back();
		switch (chr) {
		case '(':
			score++;
			break;
		case '[':
			score += 2;
			break;
		case '{':
			score += 3;
			break;
		case '<':
			score += 4;
			break;
		}
	}

	return score;
}

int main()
{
	std::vector<char> stack;

	std::string line;

	std::vector<int64_t> scores;
	while (std::cin >> line) {
		int64_t score = score_syntax_error(line);
		if (score == -1)
			continue;
		scores.push_back(score);
	}

	// Find median.
	std::sort(scores.begin(), scores.end());
	size_t mid = scores.size() / 2;
	int64_t median = scores.size() % 2 == 1 ? scores[mid] : (scores[mid - 1] + scores[mid]) / 2;

	std::cout << median << std::endl;

	return 0;
}
