#include <code_gen/asm_predicates.hh>

using namespace std;



namespace
{
	mips_byte mc_size (basic_mc_type t)
	{
		switch (t)
		{
		case basic_mc_type::mc_int:
			return 1;
		case basic_mc_type::mc_char:
			return 4;
		default:
			return 0;
		}
	}

	inline mips_byte mc_align (basic_mc_type t)
	{ return mc_size (move (t)); }

	mips_byte mc_size (mc_type t)
	{
		if (t.size != 0) // Array)
			return t.size;
		return mc_align (t.type);
	}
}



local_var_layout::local_var_layout ()
	: size (0)
{
}

local_var_layout::local_var_layout (typename vector <mc_type>::const_iterator first,
                                    typename vector <mc_type>::const_iterator last)
	: size (last - first)
{
	typename offsets::value_type current_offset = 0;

	for (auto i = first; i != last; ++i)
	{
		mips_byte align = mc_align (i->type);
		if (align != 0)
		{
			current_offset += current_offset % align;
			offsets.push_back (current_offset);
			current_offset += mc_size (*i);
		}
	}
}



vector <string> code_gen (const program& node)
{
	return code_gen (*p.decl_list, *p.table);
}



vector <string> code_gen (const declList& node,
                               const symbol_table& global_table)
{
	vector <asm_data> data;
	vector <asm_function> functions;

	for (const Node <decl>& top_level_decl : *node.decls)
	{
		if (top_level_decl->sub_decl->type == AST_type::varDecl)
			data.push_back (code_gen (reinterpret_cast <const varDecl&> (*top_level_decl->sub_decl),
			                          global_table));
		else
			functions.push_back (code_gen (reinterpret_cast <const funDecl&> (*top_level_decl->sub_decl),
			                               global_table));
	}

	vector <instruction> instructions = {"data:"};
	for (const asm_data& datum : data)
		instructions.push_back (lines (datum));

	instructions.push_back ("text:");
	instructions.push_back ("\t.align\t2");
       	instructions.push_back ("\t.set\tnomips16");
	for (const asm_function& func : functions)
		instructions.push_back (lines (func));

	return instructions;
}



void code_gen (const funDecl& node,
               const symbol_table& global_table)
{
}



void code_gen (const funBody& node,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}



void code_gen (const statementList& node,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}



void code_gen (const statement& node,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}



void code_gen (const compoundStmt& node,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}



void code_gen (const assignStmt& node,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}



void code_gen (const condStmt& node,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}



void code_gen (const loopStmt& node,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}



void code_gen (const returnStmt& node,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}



void code_gen (const var& node,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}



void code_gen (const expression& node,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}



void code_gen (const addExpr& node,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}



void code_gen (const term& node,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}



void code_gen (const factor& node,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}



void code_gen (const funcCallExpr& node,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}



void code_gen (const argList& node,
               const vector <mc_type>& signature,
               const symbol_table& local_table,
               const symbol_table& param_table,
               const symbol_table& global_table)
{
}
