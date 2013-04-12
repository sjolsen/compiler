#include <code_gen/asmdefs.hh>

using namespace std;



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
