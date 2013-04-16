#include <code_gen/asmdefs.hh>

#include <stdexcept>

using namespace std;



register_pool::register_pool ()
	: next_register (32) // Disjunction with real-register designators
{
}

virt_reg register_pool::get ()
{
	if (reg_queue.empty ())
		return next_register++;
	virt_reg reg = reg_queue.front ();
	reg_queue.pop_front ();
	return reg;
}

virt_reg register_pool::get (const std::string& name)
{
	if (var_map.count (name) == 0)
		var_map [name] = this->get ();
	return var_map [name];
}

void register_pool::release (virt_reg v)
{
	reg_queue.push_back (v);
}

int register_pool::max_live () const
{
	return next_register - 32;
}



string to_string (opname name)
{
	switch (name)
	{
	case opname::add:
		return "add";
	case opname::beq:
		return "beq";
	case opname::div:
		return "div";
	case opname::jal:
		return "jal";
	case opname::jr:
		return "jr";
	case opname::lb:
		return "lb";
	case opname::li:
		return "li";
	case opname::lw:
		return "lw";
	case opname::mfhi:
		return "mfhi";
	case opname::mflo:
		return "mflo";
	case opname::move:
		return "move";
	case opname::mult:
		return "mult";
	case opname::nop:
		return "nop";
	case opname::nor:
		return "nor";
	case opname::bit_or:
		return "bit_or";
	case opname::sb:
		return "sb";
	case opname::sll:
		return "sll";
	case opname::slt:
		return "slt";
	case opname::sub:
		return "sub";
	case opname::sw:
		return "sw";
	case opname::xori:
		return "xori";
	default:
		throw runtime_error ("Invalid opname");
	};
};



string to_string (real_reg r)
{
	switch (r)
	{
	case real_reg::zero:
		return "$zero";
	case real_reg::at:
		return "$at";
	case real_reg::v0:
		return "$v0";
	case real_reg::v1:
		return "$v1";
	case real_reg::a0:
		return "$a0";
	case real_reg::a1:
		return "$a1";
	case real_reg::a2:
		return "$a2";
	case real_reg::a3:
		return "$a3";
	case real_reg::t0:
		return "$t0";
	case real_reg::t1:
		return "$t1";
	case real_reg::t2:
		return "$t2";
	case real_reg::t3:
		return "$t3";
	case real_reg::t4:
		return "$t4";
	case real_reg::t5:
		return "$t5";
	case real_reg::t6:
		return "$t6";
	case real_reg::t7:
		return "$t7";
	case real_reg::s0:
		return "$s0";
	case real_reg::s1:
		return "$s1";
	case real_reg::s2:
		return "$s2";
	case real_reg::s3:
		return "$s3";
	case real_reg::s4:
		return "$s4";
	case real_reg::s5:
		return "$s5";
	case real_reg::s6:
		return "$s6";
	case real_reg::s7:
		return "$s7";
	case real_reg::t8:
		return "$t8";
	case real_reg::t9:
		return "$t9";
	case real_reg::k0:
		return "$k0";
	case real_reg::k1:
		return "$k1";
	case real_reg::gp:
		return "$gp";
	case real_reg::sp:
		return "$sp";
	case real_reg::fp:
		return "$fp";
	case real_reg::ra:
		return "$ra";
	default:
		throw runtime_error ("Invalid physical register");
	};
}



string to_string (const instruction& i)
{
	return to_string (i.op) + ' ' + [&] () -> string
	{
		switch (i.op)
		{
		case opname::add:
		case opname::bit_or:
		case opname::nor:
		case opname::slt:
		case opname::sub:
			return to_string (i._1.real) + ", " + to_string (i._2.real) + ", " + to_string (i._3.real);

		case opname::beq:
		case opname::sll:
		case opname::xori:
			return to_string (i._1.real) + ", " + to_string (i._2.real) + ", " + to_string (i._3.literal);

		case opname::div:
		case opname::move:
		case opname::mult:
			return to_string (i._1.real) + ", " + to_string (i._2.real);

		case opname::jal:
			return i.label;

		case opname::jr:
		case opname::mfhi:
		case opname::mflo:
			return to_string (i._1.real);

		case opname::lb:
		case opname::lw:
		case opname::sb:
		case opname::sw:
			return to_string (i._1.real) + ", " + to_string (i._2.literal) + '(' + to_string (i._3.real) + ')';

		case opname::li:
		 	return to_string (i._1.real) + ", " + to_string (i._2.literal);

		default:
			return "";
		};
	} ();
}



vector <string> lines (asm_type t)
{
	if (t == asm_type::mips_byte)
		return vector <string > {"\t.byte"};
	return vector <string> ("\t.align", "\t.word");
}



vector <string> lines (const asm_data& datum)
{
	vector <string> d_lines {datum.name + ":\n\t"};

	for (string& line : lines (datum.type))
		d_lines.push_back (move (line));

	return d_lines;
}



vector <string> lines (const asm_function& func)
{
	vector <string> f_lines {"\t.globl\t" + func.name,
	                         "\t.ent\t" + func.name,
	                         "\t.type\t" + func.name + ", @function",
	                         func.name + ':'};

	for (const instruction& i : func.body)
		f_lines.push_back (to_string (i));

	f_lines.push_back (".end\t" + func.name);
	f_lines.push_back (".size\t" + func.name + ", .-" + func.name);

	return f_lines;
}
