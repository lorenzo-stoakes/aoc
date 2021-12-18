#include <algorithm>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

struct node {
	int val = 0xdeadbeef;
	bool destroyed = false;

	struct node *left = nullptr;
	struct node *right = nullptr;
	struct node *parent = nullptr;

	bool is_leaf() const
	{
		return left == nullptr && right == nullptr;
	}

	bool is_number_pair() const
	{
		return left != nullptr && left->is_leaf() &&
			right != nullptr && right->is_leaf();
	}

	bool is_left_child() const
	{
		return parent != nullptr && parent->left == this;
	}

	bool is_right_child() const
	{
		return parent != nullptr && parent->right == this;
	}

	bool is_head() const
	{
		return parent == nullptr;
	}
};

enum class action_type
{
	EXPLODE,
	SPLIT,
};

struct action {
	action(action_type type, node *n)
		: type{type}
		, n{n}
	{}

	action_type type;
	node *n;
};

node* parse_single_input(const std::string& line)
{
	node *head = new node();
	node *curr = head;

	std::vector<bool> side_stack { true };

	bool first = true;

	for (const char chr : line) {
		const bool is_left_side = side_stack.back();
		switch (chr) {
		case '[':
		{
			if (first) {
				first = false;
				break;
			}

			node *new_node = new node();
			if (is_left_side)
				curr->left = new_node;
			else
				curr->right = new_node;
			new_node->parent = curr;
			curr = new_node;
			side_stack.push_back(true);

			break;
		}
		case ']':
			curr = curr->parent;
			side_stack.pop_back();

			break;
		case ',':
			side_stack.pop_back();
			side_stack.push_back(false);

			break;
		default: // 0 - 9
		{
			int val = chr - '0';
			node *leaf = new node();
			leaf->val = val;
			leaf->parent = curr;
			if (is_left_side)
				curr->left = leaf;
			else
				curr->right = leaf;

			break;
		}
		}
	}

	return head;
}

bool check(node *node)
{
	if (node == nullptr)
		return true;

	if (node->left != nullptr && node->left->parent != node)
		return false;

	if (node->right != nullptr && node->right->parent != node)
		return false;

	if (!node->is_leaf() && node->left == node->right)
		return false;

	return check(node->left) && check(node->right);
}

std::vector<node*> parse_input()
{
	std::vector<node*> ret;

	std::string line;
	while ((std::cin >> line)) {
		node *n = parse_single_input(line);
		if (!check(n))
			std::cout << "CORRUPT!" << std::endl;

		ret.push_back(n);
	}

	return ret;
}

void print_inner_node(node *curr)
{
	if (curr == nullptr)
		return;

	if (curr->is_leaf()) {
		std::cout << curr->val;
		return;
	}

	std::cout << "[";
	print_inner_node(curr->left);
	if (curr->right != nullptr) {
		std::cout << ",";
		print_inner_node(curr->right);
	}

	std::cout << "]";
}

void print_node(node *head)
{
	print_inner_node(head);
	std::cout << std::endl;
}

std::deque<action> get_actions(node *head)
{
	std::deque<action> ret;

	std::vector<node *> stack { head };
	std::vector<int> depth_stack { 0 };

	while (!stack.empty()) {
		node *curr = stack.back();
		stack.pop_back();
		int depth = depth_stack.back();
		depth_stack.pop_back();

		// Explode.
		if (depth == 4 && curr->is_number_pair()) {
			ret.emplace_back(action_type::EXPLODE, curr);
		}

		// Split.
		if (curr->is_leaf() && curr->val >= 10) {
			ret.emplace_back(action_type::SPLIT, curr);
		}

		if (curr->left != nullptr) {
			stack.push_back(curr->left);
			depth_stack.push_back(depth + 1);
		}

		if (curr->right != nullptr) {
			stack.push_back(curr->right);
			depth_stack.push_back(depth + 1);
		}
	}

	return ret;
}

// Recursively try and find first leaf node.
node *first_leaf(node *n)
{
	if (n == nullptr)
		return nullptr;

	if (n->is_leaf())
		return n;

	node *ret = first_leaf(n->left);
	return ret == nullptr ? first_leaf(n->right) : ret;
}

void add_val(node *n, int val)
{
	if (n == nullptr)
		return;

	node *leaf = first_leaf(n);
	if (leaf != nullptr)
		leaf->val += val;
}

node *next_left(node *head)
{
	if (head == nullptr)
		return nullptr;

	node *prev = head;
	node *curr = head->parent;

	while (curr != nullptr && curr->left == prev) {
		prev = curr;
		curr = curr->parent;
	}

	if (curr == nullptr)
		return nullptr;
	// Rightmost of left.
	for (curr = curr->left; curr != nullptr && !curr->is_leaf(); curr = curr->right)
		;

	return curr;
}

node *next_right(node *head)
{
	if (head == nullptr)
		return nullptr;

	node *prev = head;
	node *curr = head->parent;

	while (curr != nullptr && curr->right == prev) {
		prev = curr;
		curr = curr->parent;
	}

	if (curr == nullptr) {
		return nullptr;
	}

	// Leftmost of right.
	for (curr = curr->right; curr != nullptr && !curr->is_leaf(); curr = curr->left)
		;

	return curr;
}

void explode(node *n)
{
	// We might be visiting this post destroying child nodes.
	if (!n->is_number_pair()) {
		return;
	}

	int left_val = n->left->val;
	int right_val = n->right->val;

	//std::cout << "explode " << left_val << "," << right_val << std::endl;

	n->left = nullptr;
	n->right = nullptr;

	node *adj_left = next_left(n);
	if (adj_left != nullptr) {
		adj_left->val += left_val;
	}

	node *adj_right = next_right(n);
	if (adj_right != nullptr) {
		adj_right->val += right_val;
	}

	// Reset here in case we accidentally set again above.
	n->val = 0;
}

void split(node *n)
{
	if (n->val < 10)
		return;

	//std::cout << "split" << std::endl;

	int left_val = n->val / 2;
	int right_val = n->val / 2;
	if (n->val % 2 == 1)
		right_val++;

	n->left = new node();
	n->left->val = left_val;
	n->left->parent = n;

	n->right = new node();
	n->right->val = right_val;
	n->right->parent = n;

	n->val = 0;
}

void do_action(const action& a)
{
	if (a.type == action_type::EXPLODE)
		explode(a.n);
	else
		split(a.n);
}

void reduce(node *head)
{
	// First explodes.
	std::deque<action> actions = get_actions(head);
	if (actions.empty())
		return;

	int num_explodes = 0;
	int num_splits = 0;

	for (action& a : actions) {
		if (a.type == action_type::EXPLODE)
			num_explodes++;
		else
			num_splits++;
	}

	std::reverse(actions.begin(), actions.end());

	for (action& a : actions) {
		if (num_explodes > 0 && a.type == action_type::SPLIT)
			continue;

		do_action(a);
		break;
	}

	reduce(head);
}

int64_t get_magnitude(node *node)
{
	if (node == nullptr)
		return 0;

	if (node->is_leaf())
		return node->val;

	return 3L * get_magnitude(node->left) + 2L * get_magnitude(node->right);
}

node* add(node* left, node* right)
{
	node *ret = new node();

	ret->left = left;
	ret->left->parent = ret;
	ret->right = right;
	ret->right->parent = ret;

	return ret;
}

int main()
{
	std::vector<node*> snails = parse_input();

	node* curr = snails[0];
	for (int i = 1; i < snails.size(); i++) {
		curr = add(curr, snails[i]);
		reduce(curr);
	}

	//print_node(curr);
	std::cout << get_magnitude(curr) << std::endl;

	return 0;
}
