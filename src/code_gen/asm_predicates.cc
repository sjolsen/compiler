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



asm_function code_gen (const funDecl& node,
                       const symbol_table& global_table)
{
	return asm_function {node.name->token_ref.str,
	                     code_gen (*node.body, *node.local_table, *node.param_table, global_table)};
}



vector <instruction> code_gen (const funBody& node,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	register_pool vregs;

	if (!node.stmt_list)
		return vector <instruction> {"\tjr $ra", "\tnop"};

	return schedule_code (code_gen (*node.stmt_list, vregs, local_table,
	                                param_table, global_table));
}



vector <instruction> code_gen (const statementList& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	vector <instruction> instructions;
	for (const Node <statement>& stmt : node.stmts)
	{
		auto stmt_instructions = code_gen (*stmt, vregs, local_table, param_table, global_table);
		instructions.insert (end (instructions), begin (stmt_instructions), end (stmt_instructions));
	}
	return instructions;
}



vector <instruction> code_gen (const statement& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	switch (node.type)
	{
	case AST_type::compoundStmt:
		return code_gen (reinterpret_cast <const compoundStmt&> (*node.sub_stmt),
		                 vregs, local_table, param_table, global_table);
	case AST_type::compoundStmt:
		return code_gen (reinterpret_cast <const assignStmt&> (*node.sub_stmt),
		                 vregs, local_table, param_table, global_table);
	case AST_type::assignStmt:
		return code_gen (reinterpret_cast <const condStmt&> (*node.sub_stmt),
		                 vregs, local_table, param_table, global_table);
	case AST_type::condStmt:
		return code_gen (reinterpret_cast <const loopStmt&> (*node.sub_stmt),
		                 vregs, local_table, param_table, global_table);
	case AST_type::loopStmt:
		return code_gen (reinterpret_cast <const returnStmt&> (*node.sub_stmt),
		                 vregs, local_table, param_table, global_table);
	default:
		throw runtime_error ("Bad node type in code_gen (statement)");
	};
}



vector <instruction> code_gen (const compoundStmt& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	vector <instruction> instructions;
	for (const Node <statement>& stmt : node.stmt_list)
	{
		auto stmt_instructions = code_gen (*stmt, vregs, local_table, param_table, global_table);
		instructions.insert (end (instructions), begin (stmt_instructions), end (stmt_instructions));
	}
	return instructions;
}



vector <instruction> code_gen (const assignStmt& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	code_and_result lvalue;
	if (node.lvalue)
		lvalue = code_gen (*node.lvalue, vregs, local_table, param_table, global_table);
	else
		lvalue.result = vregs.get ();

	code_and_result rvalue = code_gen (*node.rvalue, lvalue.result, vregs, local_table, param_table, global_table);
	if (!node.lvalue)
		vregs.release (lvalue.result);

	lvalue.code.insert (end (lvalue.code), begin (rvalue.code), end (rvalue.code)); // Re-use this storage
	return lvalue.code;
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
