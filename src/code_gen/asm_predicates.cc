#include <code_gen/asm_predicates.hh>

#include <functional>

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
			return t.size * mc_size (t.type);
		return mc_size (t.type);
	}

	basic_mc_type get_type (const var& node,
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
		return var_entry.type [0].type;
	}

	function <int (int, int)> operator_from_node (const relop& op)
	{
		switch (op.sym->token_ref.op)
		{
		case symbol::less_than:
			return [] (int a, int b) { return a < b; };
		case symbol::greater_than:
			return [] (int a, int b) { return a > b; };
		case symbol::lt_equiv:
			return [] (int a, int b) { return a <= b; };
		case symbol::gt_equiv:
			return [] (int a, int b) { return a >= b; };
		case symbol::equivalent:
			return [] (int a, int b) { return a == b; };
		case symbol::not_equiv:
			return [] (int a, int b) { return a != b; };
		default:
			throw runtime_error ("Bad operator");
		};
	}

	function <int (int, int)> operator_from_node (const addop& op)
	{
		switch (op.sym->token_ref.op)
		{
		case symbol::plus:
			return [] (int a, int b) { return a + b; };
		case symbol::minus:
			return [] (int a, int b) { return a - b; };
		default:
			throw runtime_error ("Bad operator");
		};
	}

	function <int (int, int)> operator_from_node (const mulop& op)
	{
		switch (op.sym->token_ref.op)
		{
		case symbol::asterisk:
			return [] (int a, int b) { return a * b; };
		case symbol::solidus:
			return [] (int a, int b) { return a / b; };
		default:
			throw runtime_error ("Bad operator");
		};
	}

	string make_label ()
	{
		static int number = 0;
		return ".L" + to_string (number++);
	}

	int max_callee_args;
}



local_var_layout::local_var_layout ()
	: size (0)
{
}

local_var_layout::local_var_layout (typename vector <mc_type>::const_iterator first,
                                    typename vector <mc_type>::const_iterator last)
	: size (last - first)
{
	int current_offset = 0;

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
	return code_gen (*node.decl_list, *node.table);
}



vector <string> code_gen (const declList& node,
                          const symbol_table& global_table)
{
	vector <asm_data> data;
	vector <asm_function> functions;

	for (const Node <decl>& top_level_decl : node.decls)
	{
		if (top_level_decl->sub_decl->type == AST_type::varDecl)
			// data.push_back (code_gen (dynamic_cast <const varDecl&> (*top_level_decl->sub_decl),
			//                           global_table));
			;
		else
			functions.push_back (code_gen (dynamic_cast <const funDecl&> (*top_level_decl->sub_decl),
			                               global_table));
	}

	vector <string> instructions = {"\t.data"};
	for (const asm_data& datum : data)
		for (string& line : lines (datum))
			instructions.push_back (move (line));

	instructions.push_back ("");
	instructions.push_back ("\t.abicalls");
	instructions.push_back ("\t.text");
//	instructions.push_back ("\t.set\tnoreorder");
//	instructions.push_back ("\t.align\t2");
       	instructions.push_back ("\t.set\tnomips16\n");
	instructions.push_back ("");

	for (const asm_function& func : functions)
		for (string& line : lines (func))
			instructions.push_back (move (line));

	return instructions;
}



asm_function code_gen (const funDecl& node,
                       const symbol_table& global_table)
{
	max_callee_args = 0;
	if (node.body)
		return asm_function {node.name->token_ref.str,
		                     code_gen (*node.body, *node.local_table, *node.param_table, global_table)};
	return asm_function {node.name->token_ref.str, vector <instruction> ()};
}



vector <instruction> code_gen (const funBody& node,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	register_pool vregs;

	if (!node.stmt_list)
	{
		vector <instruction> body;
		body.push_back (instruction {opname::jr, real_reg::ra, 0, 0});
		body.push_back (instruction {opname::nop, 0, 0, 0});
		return body;
	}

	return schedule_code (code_gen (*node.stmt_list, vregs, local_table, param_table, global_table),
	                      vregs, local_table, param_table, global_table);
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
	switch (node.sub_stmt->type)
	{
	case AST_type::compoundStmt:
		return code_gen (dynamic_cast <const compoundStmt&> (*node.sub_stmt),
		                 vregs, local_table, param_table, global_table);
	case AST_type::assignStmt:
		return code_gen (dynamic_cast <const assignStmt&> (*node.sub_stmt),
		                 vregs, local_table, param_table, global_table);
	case AST_type::condStmt:
		return code_gen (dynamic_cast <const condStmt&> (*node.sub_stmt),
		                 vregs, local_table, param_table, global_table);
	case AST_type::loopStmt:
		return code_gen (dynamic_cast <const loopStmt&> (*node.sub_stmt),
		                 vregs, local_table, param_table, global_table);
	case AST_type::returnStmt:
		return code_gen (dynamic_cast <const returnStmt&> (*node.sub_stmt),
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
	for (const Node <statement>& stmt : node.stmt_list->stmts)
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
	return lvalue.load_code;
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

	string end_label = make_label ();
	string else_label;
	if (node.else_stmt)
		else_label = make_label ();
	else
		else_label = end_label;

	conditional.push_back (instruction {opname::beq, cond_register, real_reg::zero, 0, else_label});
	conditional.push_back (instruction {opname::nop, 0, 0, 0});
	conditional.insert (end (conditional), begin (then_code), end (then_code));
	conditional.push_back (instruction {opname::b, 0, 0, 0, end_label});
	conditional.push_back (instruction {opname::nop, 0, 0, 0, else_label});
	conditional.insert (end (conditional), begin (else_code), end (else_code));

	return conditional;
}



vector <instruction> code_gen (const loopStmt& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	virt_reg cond_register = vregs.get ();
	vector <instruction> test = code_gen (*node.cond_expr, cond_register, vregs, local_table, param_table, global_table);
	vregs.release (cond_register);

	string test_label = make_label ();
	test [0].label = test_label;
	string end_label = make_label ();

	vector <instruction> body = code_gen (*node.then_stmt, vregs, local_table, param_table, global_table);
	test.push_back (instruction {opname::beq, cond_register, real_reg::zero, body.size () + 2}); // Skip body-code plus delay no-op and loopback branch
	test.push_back (instruction {opname::nop, 0, 0, 0});
	test.insert (end (test), begin (body), end (body));
	test.push_back (instruction {opname::b, 0, 0, 0, test_label});
	test.push_back (instruction {opname::nop, 0, 0, 0, end_label});

	return test;
}



vector <instruction> code_gen (const returnStmt& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	vector <instruction> expr_code;
	if (node.rtrn_expr)
		expr_code = code_gen (*node.rtrn_expr, real_reg::v0, vregs, local_table, param_table, global_table);

	expr_code.push_back (instruction {opname::jr, real_reg::ra, 0, 0}); // Not SysV-compliant. Should replace during scheduling.
	expr_code.push_back (instruction {opname::nop, 0, 0, 0});
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
		reference.store_code.push_back (instruction {opname::sw, reference.data_reg, 0, reference.address_reg});
	else
		reference.store_code.push_back (instruction {opname::sb, reference.data_reg, 0, reference.address_reg});

	return reference;
}



vector <instruction> code_gen (const expression& node,
                               mips_register r,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	vector <instruction> rhs_code = code_gen (*node.rhs, r, vregs, local_table, param_table, global_table);
	if (!node.lhs)
		return rhs_code;

	register_pool temp_pool = vregs;
	virt_reg l = temp_pool.get ();
	vector <instruction> lhs_code = code_gen (*node.lhs, l, vregs, local_table, param_table, global_table);

	if (lhs_code.size () == 1 && lhs_code [0].op == opname::li &&
	    rhs_code.size () == 1 && lhs_code [0].op == opname::li) // Constant expression
	 	return vector <instruction> {instruction {opname::li, r, operator_from_node (*node.op) (lhs_code [0]._2.literal, rhs_code [0]._2.literal), 0}};

	vregs = temp_pool;
	lhs_code.insert (end (lhs_code), begin (rhs_code), end (rhs_code)); // Reuse
	vector <instruction> relop_code = code_gen (*node.op, l, r);
	lhs_code.insert (end (lhs_code), begin (relop_code), end (relop_code));

	vregs.release (l);
	return lhs_code;
}



vector <instruction> code_gen (const relop& op,
                               mips_register l,
                               mips_register r)
{
	vector <instruction> code;

	switch (op.sym->token_ref.op)
	{
	case symbol::less_than:
		code.push_back (instruction {opname::slt, r, l, r});
		break;

	case symbol::greater_than:
		code.push_back (instruction {opname::slt, r, r, l});
		break;

	case symbol::lt_equiv:
		code.push_back (instruction {opname::slt, r, r, l});
		code.push_back (instruction {opname::xori, r, r, 1}); // Xor with the number 1 for boolean negation
		break;

	case symbol::gt_equiv:
		code.push_back (instruction {opname::slt, r, l, r});
		code.push_back (instruction {opname::xori, r, r, 1}); // Xor with the number 1 for boolean negation
		break;

	case symbol::equivalent:
		code.push_back (instruction {opname::slt, real_reg::at, r, l});
		code.push_back (instruction {opname::slt, r, l, r});
		code.push_back (instruction {opname::nor, r, real_reg::at, r});
		break;

	case symbol::not_equiv:
		code.push_back (instruction {opname::slt, real_reg::at, r, l});
		code.push_back (instruction {opname::slt, r, l, r});
		code.push_back (instruction {opname::bit_or, r, real_reg::at, r});
		break;

	default:
		throw runtime_error ("Bad operator");
	};

	return code;
}



vector <instruction> code_gen (const addExpr& node,
                               mips_register r,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	vector <instruction> rhs_code = code_gen (*node.rhs, r, vregs, local_table, param_table, global_table);
	if (!node.lhs)
		return rhs_code;

	register_pool temp_pool = vregs;
	virt_reg l = temp_pool.get ();
	vector <instruction> lhs_code = code_gen (*node.lhs, l, vregs, local_table, param_table, global_table);

	if (lhs_code.size () == 1 && lhs_code [0].op == opname::li &&
	    rhs_code.size () == 1 && lhs_code [0].op == opname::li) // Constant expression
	 	return vector <instruction> {instruction {opname::li, r, operator_from_node (*node.op) (lhs_code [0]._2.literal, rhs_code [0]._2.literal), 0}};

	vregs = temp_pool;
	lhs_code.insert (end (lhs_code), begin (rhs_code), end (rhs_code)); // Reuse
	if (node.op->sym->token_ref.op == symbol::plus)
		lhs_code.push_back (instruction {opname::add, r, l, r});
	else
		lhs_code.push_back (instruction {opname::sub, r, l, r});

	vregs.release (l);
	return lhs_code;
}



vector <instruction> code_gen (const term& node,
                               mips_register r,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	vector <instruction> rhs_code = code_gen (*node.rhs, r, vregs, local_table, param_table, global_table);
	if (!node.lhs)
		return rhs_code;

	register_pool temp_pool = vregs;
	virt_reg l = temp_pool.get ();
	vector <instruction> lhs_code = code_gen (*node.lhs, l, vregs, local_table, param_table, global_table);

	if (lhs_code.size () == 1 && lhs_code [0].op == opname::li &&
	    rhs_code.size () == 1 && lhs_code [0].op == opname::li) // Constant expression
		return vector <instruction> {instruction {opname::li, r, operator_from_node (*node.op) (lhs_code [0]._2.literal, rhs_code [0]._2.literal), 0}};

	vregs = temp_pool;
	lhs_code.insert (end (lhs_code), begin (rhs_code), end (rhs_code)); // Reuse
	if (node.op->sym->token_ref.op == symbol::asterisk)
		lhs_code.push_back (instruction {opname::mult, l, r, 0});
	else
		lhs_code.push_back (instruction {opname::div, l, r, 0});
	lhs_code.push_back (instruction {opname::mflo, r, 0, 0});

	vregs.release (l);
	return lhs_code;
}



vector <instruction> code_gen (const factor& node,
                               mips_register r,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	vector <instruction> code;
	lvalue_reference reference;

	switch (node.rvalue->type)
	{
	case AST_type::expression:
		return code_gen (dynamic_cast <const expression&> (*node.rvalue), r, vregs, local_table, param_table, global_table);

	case AST_type::terminal:
		return vector <instruction> {instruction {opname::li, r, dynamic_cast <const terminal&> (*node.rvalue).token_ref.value, 0}};

	case AST_type::funcCallExpr:
		code = code_gen (dynamic_cast <const funcCallExpr&> (*node.rvalue), vregs, local_table, param_table, global_table);
		code.push_back (instruction {opname::move, r, real_reg::v0, 0});
		return code;

	case AST_type::var:
		reference = code_gen (dynamic_cast <const var&> (*node.rvalue), vregs, local_table, param_table, global_table);
		code = move (reference.load_code);
		code.push_back (instruction {opname::move, r, reference.data_reg, 0});
		if (dynamic_cast <const var&> (*node.rvalue).size)
		{
			vregs.release (reference.data_reg);
			vregs.release (reference.address_reg);
		}
		return code;

	default:
		throw runtime_error ("Bad node type in code_gen (factor)");
	};
}



vector <instruction> code_gen (const funcCallExpr& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table)
{
	vector <instruction> call_code;

	if (node.arg_list)
	{
		const vector <Node <expression>>& args = node.arg_list->args;
		vector <virt_reg> build_registers;

		// Construct arguments

		for (int i = 0; i < args.size (); ++i)
		{
			virt_reg build_register = vregs.get ();
			build_registers.push_back (build_register);

			vector <instruction> expr_code = code_gen (*args [i], build_register, vregs, local_table, param_table, global_table);
			call_code.insert (end (call_code), begin (expr_code), end (expr_code));
		}

		// Store arguments

		call_code.push_back (instruction {opname::store_frame, 0, 0, 0});

		for (int i = 0; i < 4 && i < args.size (); ++i)
		{
			call_code.push_back (instruction {opname::move, static_cast <int> (real_reg::a0) + i, build_registers [i], 0});
			vregs.release (build_registers [i]);
		}
		for (int i = 4; i < args.size (); ++i)
		{
			call_code.push_back (instruction {opname::sw, build_registers [i], 4 * i, real_reg::sp});
			vregs.release (build_registers [i]);
		}

		max_callee_args = max (max_callee_args, args.size () ? max <int> (args.size (), 4) : 0);
	}

	call_code.push_back (instruction {opname::jal, 0, 0, 0, node.name->token_ref.str});
	call_code.push_back (instruction {opname::nop, 0, 0, 0});
	call_code.push_back (instruction {opname::load_frame, 0, 0, 0});

	return call_code;
}



namespace
{
	inline int dword_align (int bytes)
	{
		return ((bytes + 7) / 8) * 8;
	}
}



vector <instruction> schedule_code (vector <instruction> code,
                                    register_pool& vregs,
                                    const symbol_table& local_table,
                                    const symbol_table& param_table,
                                    const symbol_table& global_table)
{
	string epilogue_label = make_label ();

	// SysV function returns

	for (instruction& i : code)
		if (i.op == opname::jr &&
		    i._1.real == real_reg::ra)
			i = instruction {opname::b, 0, 0, 0, epilogue_label};

	// Stack frame

	vector <mc_type> local_types;
	for (const auto& pair : local_table)
		local_types.push_back (pair.second.type [0]);

	local_var_layout layout (begin (local_types), end (local_types));
	const int local_offset = dword_align (4 * max_callee_args); // Start of local variable storage
	const int stack_size = dword_align (layout.size + 8) + local_offset; // Add frame pointer and return address

	vector <instruction> store_code = {instruction {opname::sw, real_reg::ra, stack_size - 8, real_reg::sp},
	                                   instruction {opname::sw, real_reg::fp, stack_size - 4, real_reg::sp}};
	vector <instruction> load_code = {instruction {opname::lw, real_reg::ra, stack_size - 8, real_reg::sp},
	                                  instruction {opname::lw, real_reg::fp, stack_size - 4, real_reg::sp}};

	if (param_table.size () != 0)
	{
		store_code.push_back (instruction {opname::sw, real_reg::a0, stack_size, real_reg::sp});
		store_code.push_back (instruction {opname::sw, real_reg::a1, stack_size + 4, real_reg::sp});
		store_code.push_back (instruction {opname::sw, real_reg::a2, stack_size + 8, real_reg::sp});
		store_code.push_back (instruction {opname::sw, real_reg::a3, stack_size + 12, real_reg::sp});

		load_code.push_back (instruction {opname::lw, real_reg::a0, stack_size, real_reg::sp});
		load_code.push_back (instruction {opname::lw, real_reg::a1, stack_size + 4, real_reg::sp});
		load_code.push_back (instruction {opname::lw, real_reg::a2, stack_size + 8, real_reg::sp});
		load_code.push_back (instruction {opname::lw, real_reg::a3, stack_size + 12, real_reg::sp});
	}

	for (auto i = 0; i != code.size (); ++i)
	{
		if (code [i].op == opname::store_frame)
		{
			while (code [i].op == opname::store_frame ||
			       code [i].op == opname::load_frame)
				code.erase (begin (code) + i);
			code.insert (begin (code) + i, begin (store_code), end (store_code));
		}
		else if (code [i].op == opname::load_frame)
		{
			while (code [i].op == opname::store_frame ||
			       code [i].op == opname::load_frame)
				code.erase (begin (code) + i);
			code.insert (begin (code) + i, begin (load_code), end (load_code));
		}
	}

	code.insert (begin (code), instruction {opname::addi, real_reg::sp, real_reg::sp, 0 - stack_size});
	code.push_back (instruction {opname::addi, real_reg::sp, real_reg::sp, stack_size, epilogue_label});
	code.push_back (instruction {opname::jr, real_reg::ra, 0, 0});
	code.push_back (instruction {opname::nop, 0, 0, 0});

	return code;
}
