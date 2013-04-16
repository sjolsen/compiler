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

	basic_mc_type get_type (const var& v,
	                        const symbol_table& local_table,
	                        const symbol_table& param_table,
	                        const symbol_table& global_table) // Merge with code in semantic_check.cc
	{
		symbol_entry var_entry;
		try
		{
			var_entry = local_table.at (node.name->token_ref.str);
		}
		catch (const out_of_range&)
		{
			try
			{
				var_entry = param_table.at (node.name->token_ref.str);
			}
			catch (const out_of_range&)
			{
				try
				{
					var_entry = global_table.at (node.name->token_ref.str);
				}
				catch (const out_of_range&)
				{
					throw error ("Undefined variable",
					             node.pos ());
				}
			}
		}
		return var_entry.type [0];
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
		return vector <instruction> {{opname::jr, real_reg::ra}, {opname::nop}};

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
	lvalue_reference lvalue;
	if (node.lvalue)
		lvalue = code_gen (*node.lvalue, vregs, local_table, param_table, global_table);
	else
		lvalue.data_reg = vregs.get ();

	vector <instruction> rvalue = code_gen (*node.rvalue, lvalue.data_reg, vregs, local_table, param_table, global_table);
	if (node.lvalue)
	{
		if (node.lvalue->size)
		{
			vregs.release (lvalue.data_reg);
			vregs.release (lvalue.address_reg);
		}
	}
	else
		vregs.release (lvalue.data_reg);

	lvalue.load_code.insert (end (lvalue.load_code), begin (rvalue), end (rvalue)); // Re-use this storage
	lvalue.load_code.insert (end (lvalue.load_code), begin (lvalue.store_code), end (lvalue.store_code));
	return lvalue.code;
}



vector <instruction> code_gen (const condStmt& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	virt_reg cond_register = vregs.get ();
	vector <instruction> conditional = code_gen (*node.cond_expr, cond_register, vregs, local_table, param_table, global_table);
	vregs.release (cond_register);

	vector <instruction> then_code = code_gen (*node.then_stmt, vregs, local_table, param_table, global_table);
	vector <instruction> else_code;
	if (node.else_stmt)
		else_code = code_gen (*node.else_stmt, vregs, local_table, param_table, global_table);

	conditional.push_back (instruction {opname::beq, cond_register, real_reg::zero, 0, then_code.size () + 2}); // Skip then-code plus delay no-op and else-skip branch
	conditional.push_back (instruction {opname::nop});
	conditional.insert (end (conditional.code), begin (then_code), end (then_code));
	conditional.push_back (instruction {opname::beq, real_reg::zero, real_reg::zero, 0, else_code.size () + 1}); // Skip else-code plus delay no-op
	conditional.push_back (instruction {opname::nop});
	conditional.insert (end (conditional.code), begin (else_code), end (else_code));

	return conditional;
}



vector <instruction> code_gen (const loopStmt& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	virt_reg cond_register = vregs.get ();
	vector <instruction> test = code_gen (*node.cond_expr, cond_register, local_table, param_table, global_table);
	vregs.release (cond_register);

	vector <instruction> body = code_gen (*node.then_stmt, local_table, param_table, global_table);
	test.push_back (instruction {opname::beq, cond_register, real_reg::zero, 0, body.size () + 2}); // Skip body-code plus delay no-op and loopback branch
	test.push_back (instruction {opname::nop});
	test.insert (end (test.code), begin (body), end (body));
	test.push_back (instruction {opname::beq, real_reg::zero, real_reg::zero, 0, 0 - (body.size () + test.size () + 3)}); // Skip back across test and body code, plus this branch and the test-fail branch/no-op
	test.push_back (instruction {opname::nop});

	return test;
}



vecotr <instruction> code_gen (const returnStmt& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	vector <instruction> expr_code;
	if (node.rtrn_expr)
		expr_code = code_gen (*node.rtrn_expr, real_reg::v0, vregs, local_table, param_table, global_table);

	expr_code.push_back (instruction {opname::jr, real_reg::ra}); // Not SysV-compliant. Should replace during scheduling.
	expr_code.push_back (instruction {opname::nop});
	return expr_code;
}



lvalue_reference code_gen (const var& node,
                           register_pool& vregs,
                           const symbol_table& local_table,
                           const symbol_table& param_table,
                           const symbol_table& global_table)
{
	virt_reg name = vregs.get (node.name->token_ref.str);
	if (!node.size)
		return lvalue_reference {name};

	// Load code
	lvalue_reference reference = {vregs.get (), vregs.get ()};
	reference.load_code = code_gen (*node.size, reference.address_reg, vregs, local_table, param_table, global_table);

	if (get_type (node, local_table, param_table, global_table) == basic_mc_type::mc_int)
		reference.load_code.push_back (instruction {opname::sll, reference.address_reg, reference.address_reg, 2}); // 4-byte ints
	reference.load_code.push_back (instruction {opname::add, reference.address_reg, reference.address_reg, name}); // Add offset to array start

	if (get_type (node, local_table, param_table, global_table) == basic_mc_type::mc_int)
		reference.load_code.push_back (instruction {opname::lw, reference.data_reg, 0, reference.address_reg});
	else
		reference.load_code.push_back (instruction {opname::lb, reference.data_reg, 0, reference.address_reg});

	// Store code
	if (get_type (node, local_table, param_table, global_table) == basic_mc_type::mc_int)
		reference.load_code.push_back (instruction {opname::sw, reference.data_reg, 0, reference.address_reg});
	else
		reference.load_code.push_back (instruction {opname::sb, reference.data_reg, 0, reference.address_reg});

	return reference;
}



vector <instruction> code_gen (const expression& node,
                               mips_register r,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	if (!node.lhs)
		return code_gen (*node.rhs, r, vregs, local_table, param_table, global_table);
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