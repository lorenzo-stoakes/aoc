#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <unordered_set>
#include <vector>

static constexpr size_t NUM_INPUTS = 14;

enum class instruction_type
{
	INP,
	ADD,
	MUL,
	DIV,
	MOD,
	EQL,
};

static instruction_type type_from_name(const std::string& name)
{
	if (name == "inp") {
		return instruction_type::INP;
	}

	if (name == "add") {
		return instruction_type::ADD;
	}

	if (name == "mul") {
		return instruction_type::MUL;
	}

	if (name == "div") {
		return instruction_type::DIV;
	}

	if (name == "mod") {
		return instruction_type::MOD;
	}

	if (name == "eql") {
		return instruction_type::EQL;
	}

	throw std::runtime_error("Impossible! " + name);
}

enum class variable_name
{
	W,
	X,
	Y,
	Z,
};

static variable_name varname_from_name(const std::string& name)
{
	switch (name[0]) {
	case 'w':
		return variable_name::W;
	case 'x':
		return variable_name::X;
	case 'y':
		return variable_name::Y;
	case 'z':
		return variable_name::Z;
	}

	throw std::runtime_error("Impossible! " + name);
}

struct instruction
{
	instruction_type type;
	variable_name a;

	bool b_is_immediate = false;
	union
	{
		variable_name b;
		int val;
	};

	void print() const
	{
		switch (type) {
		case instruction_type::INP:
			std::cout << "inp ";
			break;
		case instruction_type::ADD:
			std::cout << "add ";
			break;
		case instruction_type::MUL:
			std::cout << "mul ";
			break;
		case instruction_type::DIV:
			std::cout << "div ";
			break;
		case instruction_type::MOD:
			std::cout << "mod ";
			break;
		case instruction_type::EQL:
			std::cout << "eql ";
			break;
		}

		auto print_varname = [](variable_name varname) {
			switch (varname) {
			case variable_name::W:
				std::cout << "w";
				break;
			case variable_name::X:
				std::cout << "x";
				break;
			case variable_name::Y:
				std::cout << "y";
				break;
			case variable_name::Z:
				std::cout << "z";
				break;
			}
		};

		print_varname(a);

		if (type == instruction_type::INP) {
			std::cout << std::endl;
			return;
		}

		std::cout << " ";
		if (b_is_immediate) {
			std::cout << val << std::endl;
		} else {
			print_varname(b);
			std::cout << std::endl;
		}
	}
};

struct machine {
	machine(std::vector<int64_t>& vars) : var_list{vars}
	{}

	int64_t w = 0, x = 0, y = 0, z = 0;

	std::vector<int64_t>& var_list;
	size_t var_offset = 0;

	int64_t& varname_ref(const variable_name varname)
	{
		switch (varname) {
		case variable_name::W:
			return w;
		case variable_name::X:
			return x;
		case variable_name::Y:
			return y;
		case variable_name::Z:
			return z;
		}

		throw std::runtime_error("Impossible!");
	}

	void reset_vars()
	{
		w = 0;
		x = 0;
		y = 0;
		z = 0;

		var_offset = 0;
	}

	void eval(const instruction& instr)
	{
		switch (instr.type) {
		case instruction_type::INP:
		{
			if (var_offset >= var_list.size())
				throw std::runtime_error("Stack overflow!");

			int64_t val = var_list[var_offset++];
			varname_ref(instr.a) = val;

			break;
		}
		case instruction_type::ADD:
		{
			const int64_t sum = varname_ref(instr.a) +
				(instr.b_is_immediate ? instr.val : varname_ref(instr.b));
			varname_ref(instr.a) = sum;
			break;
		}
		case instruction_type::MUL:
		{
			const int64_t mul = varname_ref(instr.a) *
				(instr.b_is_immediate ? instr.val : varname_ref(instr.b));
			varname_ref(instr.a) = mul;

			break;
		}
		case instruction_type::DIV:
		{
			const int64_t div = varname_ref(instr.a) /
				(instr.b_is_immediate ? instr.val : varname_ref(instr.b));

			varname_ref(instr.a) = div;

			break;
		}
		case instruction_type::MOD:
		{
			const int64_t mod = varname_ref(instr.a) %
				(instr.b_is_immediate ? instr.val : varname_ref(instr.b));
			varname_ref(instr.a) = mod;

			break;
		}

		case instruction_type::EQL:
		{
			const int64_t a = varname_ref(instr.a);
			const int64_t b = instr.b_is_immediate ? instr.val : varname_ref(instr.b);
			varname_ref(instr.a) = a == b ? 1 : 0;

			break;
		}
		}
	}

	void print()
	{
		std::cout << "w=" << w << " x=" << x << " y=" << y << " z=" << z << std::endl;
	}
};

std::vector<instruction> parse_input()
{
	std::vector<instruction> ret;

	while (true) {
		instruction instr;

		std::string type;
		if (!(std::cin >> type))
			return ret;
		instr.type = type_from_name(type);

		std::string a;
		std::cin >> a;
		instr.a = varname_from_name(a);

		if (instr.type == instruction_type::INP) {
			ret.push_back(instr);
			continue;
		}

		std::string b;
		std::cin >> b;

		switch (b[0]) {
		case 'w':
		case 'x':
		case 'y':
		case 'z':
			instr.b = varname_from_name(b);
			break;
		default:
			instr.b_is_immediate = true;
			instr.val = std::atoi(b.c_str());
			break;
		}

		ret.push_back(instr);
	}
}

void naive()
{
	std::vector<instruction> instrs = parse_input();
      //std::vector<int64_t> inputs = { 1, 5, 9, 3, 9, 5, 9, 1, 9, 8, 8, 9, 9, 9 };
	//std::vector<int64_t> inputs = { 1, 5, 9, 3, 9, 5, 9, 1, 9, 4, 9, 9, 9, 9 };
	//std::vector<int64_t> inputs = { 9,9,4,9,7,4,2,9,2,9,8,5,7,9};
	//std::vector<int64_t> inputs = { 8,3,4,9,7,4,2,9,1,2,1,4,9,8};
//	std::vector<int64_t> inputs =
//		{9,3,4,9,9,6,2,9,6,9,8,9,9,9};

	std::vector<int64_t> inputs =
		{9,3,4,9,9,7,9,9,9,9,9,9,9,9};

	machine mach(inputs);

	auto do_eval = [&mach, &instrs] {
		mach.reset_vars();
		mach.z = 0;

		for (const auto& instr : instrs) {
			mach.eval(instr);
		}
	};

	while (true) {
		do_eval();

/*
		for (const int64_t inp : inputs) {
			std::cout << inp;
		}
		std::cout << std::endl;
		mach.print();

		return;
*/

		if (mach.z == 0)
			break;

		ssize_t index = NUM_INPUTS - 1;
		for (index = NUM_INPUTS - 1; inputs[index] == 1 && index >= 0; index--) {
			inputs[index] = 9;
		}
		if (index < 0)
			break;

		inputs[index]--;
	}

	for (const int64_t inp : inputs) {
		std::cout << inp;
	}
	std::cout << std::endl;

	mach.print();
}

std::vector<std::vector<instruction>> split_programs(const std::vector<instruction>& instrs)
{
	std::vector<std::vector<instruction>> ret;

	bool first = true;
	std::vector<instruction> curr;

	for (const instruction& instr : instrs) {
		if (instr.type == instruction_type::INP) {
			if (first) {
				first = false;
			} else {
				ret.push_back(curr);
				curr = std::vector<instruction>();
			}
		}

		curr.push_back(instr);
	}
	// Push back last one.
	ret.push_back(curr);

	return ret;
}

int main()
{
	naive();
	return 0;

	std::vector<instruction> instrs = parse_input();

	auto programs = split_programs(instrs);

	// Having read through the code manually it seems like only z impacts future input runs.
	// w is always the input value. x and y are used as variables.

	std::unordered_set<int64_t> zs;

	// Run program and put unique z values in zs.
	auto run_inputs = [&programs, &zs] (size_t program_idx, int64_t z,
					    int64_t w_from = 1, int64_t w_to = 9) {
		std::vector<int64_t> inputs(1);
		machine mach(inputs);

		for (int64_t w = w_from; w <= w_to; w++) {
			inputs[0] = w;
			mach.z = z;

			for (const instruction& instr : programs[program_idx]) {
				mach.eval(instr);
			}

			zs.insert(mach.z);
			mach.reset_vars();
		}
	};

	// get first set of z ranges.
	run_inputs(0, 0);

	std::vector<std::unordered_set<int64_t>> zs_history { zs } ;

	// Get possible zs up to 2nd to last program.
	std::cout << "0 " << zs.size() << std::endl;
	for (size_t program_idx = 1; program_idx < programs.size() - 1; program_idx++) {
		zs.clear();

		for (const int64_t z : zs_history[program_idx - 1]) {
			run_inputs(program_idx, z);
		}

		zs_history.push_back(zs);

		std::cout << program_idx << " " << zs.size() << std::endl;
	}

	// Now run the final program and look for what input zs result in 0.

	int64_t max_z = 0;

	{
		std::unordered_set<int64_t> prev_zs = zs;
		zs.clear();

		for (const int64_t z : prev_zs) {
			run_inputs(programs.size() - 1, z);
			if (zs.find(0) != zs.end()) {
				max_z = std::max(max_z, z);
				zs.clear();
			}
		}
	}

	std::vector<int64_t> target_zs(programs.size());

	std::cout << (programs.size() - 1) << " " << max_z << std::endl;

	target_zs[programs.size() - 1] = max_z;

	for (ssize_t program_idx = programs.size() - 2; program_idx > 0; program_idx--) {
		int64_t prev_max_z = max_z;
		max_z = std::numeric_limits<int64_t>::min();

		for (const int64_t z : zs_history[program_idx - 1]) {
			run_inputs(program_idx, z);

			if (zs.find(prev_max_z) != zs.end()) {
				zs.clear();
				max_z = std::max(max_z, z);
			}
		}

		std::cout << program_idx << " " << max_z << std::endl;
		target_zs[program_idx] = max_z;
	}

	std::vector<int64_t> inputs(1);
	machine mach(inputs);

	int64_t z = 0;
	for (size_t program_idx = 0; program_idx < programs.size(); program_idx++) {
		int64_t target_z;
		if (program_idx == programs.size() - 1)
			target_z = 0;
		else
			target_z = target_zs[program_idx + 1];

		int64_t w;
		for (w = 9; w >= 1; w--) {
			inputs[0] = w;

			mach.reset_vars();
			mach.z = z;
			for (const instruction& instr : programs[program_idx]) {
				mach.eval(instr);
			}

			if (mach.z == target_z) {
				std::cout << program_idx << " w=" << w << std::endl;
				z = target_z;
				break;
			}
		}

		if (w <= 0)
			throw std::runtime_error("broken");
	}

	return 0;
}
