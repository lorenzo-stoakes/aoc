#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int score_syntax_error(const std::string& line)
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
				return 3;
			stack.pop_back();
			break;
		case ']':
			if (prev != '[')
				return 57;
			stack.pop_back();
			break;
		case '}':
			if (prev != '{')
				return 1197;
			stack.pop_back();
			break;
		case '>':
			if (prev != '<')
				return 25137;
			stack.pop_back();
			break;
		default:
			throw std::runtime_error("impossible!");
		}
	}

	return 0;
}

int main()
{
	std::vector<char> stack;

	int score = 0;
	std::string line;
	while (std::cin >> line) {
		score += score_syntax_error(line);
	}

	std::cout << score << std::endl;

	return 0;
}
