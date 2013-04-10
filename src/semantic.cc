#include <semantic.hh>
#include <tokenizer.hh>

#include <stdexcept>
#include <algorithm>
#include <iterator>

using namespace std;



void semantic_check (const program& p)
{
	semantic_check (*p.decl_list, *p.table);
}



void semantic_check (const declList& node,
                     const symbol_table& global_table)
{
	for (const Node <decl>& declaration : node.decls)
		semantic_check (*declaration, global_table);
}



void semantic_check (const decl& node,
                     const symbol_table& global_table)
{
	if (node.sub_decl->type == AST_type::funDecl)
		semantic_check (reinterpret_cast <const funDecl&> (*node.sub_decl),
		                global_table);
}



void semantic_check (const funDecl& node,
                     const symbol_table& global_table)
{
	if (node.body)
		semantic_check (*node.body, *node.table, global_table);
}



void semantic_check (const funBody& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table)
{

	semantic_check (*node.stmt_list, local_table, global_table);
}



void semantic_check (const statementList& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table)
{
	for (const Node <statement>& stmt : node.stmts)
		semantic_check (*stmt, local_table, global_table);
}



void semantic_check (const statement& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table)
{
	switch (node.sub_stmt->type)
	{
	case AST_type::compoundStmt:
		semantic_check (reinterpret_cast <const compoundStmt&> (*node.sub_stmt),
		                local_table,
		                global_table);
		break;
	case AST_type::assignStmt:
		semantic_check (reinterpret_cast <const assignStmt&> (*node.sub_stmt),
		                local_table,
		                global_table);
		break;
	case AST_type::condStmt:
		semantic_check (reinterpret_cast <const condStmt&> (*node.sub_stmt),
		                local_table,
		                global_table);
		break;
	case AST_type::loopStmt:
		semantic_check (reinterpret_cast <const loopStmt&> (*node.sub_stmt),
		                local_table,
		                global_table);
		break;
	case AST_type::returnStmt:
		semantic_check (reinterpret_cast <const returnStmt&> (*node.sub_stmt),
		                local_table,
		                global_table);
		break;

	default:
		throw logic_error ("Bad enumeration value (semantic_check (statement))");
	}
}



void semantic_check (const compoundStmt& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table)
{
	semantic_check (*node.stmt_list, local_table, global_table);
}



void semantic_check (const assignStmt& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table)
{
	if (node.lvalue == nullptr)
	{
		semantic_check (*node.rvalue, local_table, global_table);
		return;
	}

	mc_type lvalue_type = semantic_check (*node.lvalue, local_table, global_table);
	mc_type rvalue_type = semantic_check (*node.rvalue, local_table, global_table);

	if (lvalue_type != rvalue_type)
		throw error ("Type mismatch in assignment (expected " + to_string (lvalue_type) +
		             "; got " + to_string (rvalue_type) + ")",
		             node.rvalue->pos ());
}



void semantic_check (const condStmt& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table)
{
	if (semantic_check (*node.cond_expr, local_table, global_table).type == basic_mc_type::mc_void)
		throw error ("Conditional expression must be non-void",
		             node.cond_expr->pos ());

	semantic_check (*node.then_stmt, local_table, global_table);
	if (node.else_stmt)
		semantic_check (*node.else_stmt, local_table, global_table);
}



void semantic_check (const loopStmt& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table)
{
	if (semantic_check (*node.cond_expr, local_table, global_table).type == basic_mc_type::mc_void)
		throw error ("Conditional expression must be non-void",
		             node.cond_expr->pos ());

	semantic_check (*node.then_stmt, local_table, global_table);
}



void semantic_check (const returnStmt& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table)
{
	const AST* fundeclp = node.parent;
	while (fundeclp->type != AST_type::funDecl)
		fundeclp = fundeclp->parent;

	mc_type return_type = fundeclp->get_type () [0];
	if (node.rtrn_expr == nullptr)
	{
		if (return_type.type == basic_mc_type::mc_void)
			return;
		else
			throw error ("Expected expression in return statement",
			             node.pos ());
	}

	mc_type rexpr_type = semantic_check (*node.rtrn_expr, local_table, global_table);
	if (rexpr_type != return_type)
		throw error ("Type mismatch in return statement (expected " + to_string (return_type) +
		             "; got " + to_string (rexpr_type) + ")",
		             node.rtrn_expr->pos ());
}



mc_type semantic_check (const var& node,
                        const symbol_table& local_table,
                        const symbol_table& global_table)
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
			var_entry = global_table.at (node.name->token_ref.str);
		}
		catch (const out_of_range&)
		{
			throw error ("Undefined variable",
			             node.pos ());
		}
	}
	mc_type var_type = var_entry.type [0];

	if (node.size == nullptr)
		return var_type;

	if (var_type.size == 0)
		throw error ("Array declared with size 0",
		             var_entry.decl_node->pos ());

	if (var_type.size > 0) // Defined locally as an array
	{
		if (node.size->lhs == nullptr &&
		    node.size->rhs->lhs == nullptr &&
		    node.size->rhs->rhs->rvalue->type == AST_type::terminal) // Single-factor literal expression
		{
			const token& literal_token = reinterpret_cast <const terminal&> (*node.size->rhs->rhs->rvalue).token_ref;

			if (literal_token.type == token_type::int_literal ||
			    literal_token.type == token_type::char_literal)
			{
				if (literal_token.value >= var_type.size)
					throw error ("Out of bounds array access",
					             node.size->pos ());
			}
			else // String literal
				throw error ("String literal used as array index",
				             node.size->pos ());
		}
		else // Complex expression; don’t perform bounds check
		{
			mc_type index_type = semantic_check (*node.size, local_table, global_table);

			if (index_type.type != basic_mc_type::mc_int &&
			    index_type.type != basic_mc_type::mc_char) // Invalid index type
				throw error ("Invalid index type (" + to_string (index_type) + ")",
				             node.size->pos ());
		}
	}

	if (var_type.size != 0 &&
	    var_type.type == basic_mc_type::mc_void) // void array
		throw error ("Array of void used in expression",
		             node.pos ());

	return mc_type (var_type.type, 0);
}



mc_type semantic_check (const expression& node,
                        const symbol_table& local_table,
                        const symbol_table& global_table)
{
	if (!node.lhs)
		return semantic_check (*node.rhs, local_table, global_table);

	mc_type lhs_type = semantic_check (*node.lhs, local_table, global_table);
	mc_type rhs_type = semantic_check (*node.rhs, local_table, global_table);

	if ((lhs_type.type != basic_mc_type::mc_int &&
	     lhs_type.type != basic_mc_type::mc_char) ||
	    (rhs_type.type != basic_mc_type::mc_int &&
	     rhs_type.type != basic_mc_type::mc_char)) // One operand is non-integral
		throw error ("Expression compares non-integral type",
		             node.op->pos ());

	if (lhs_type.type == basic_mc_type::mc_int ||
	    rhs_type.type == basic_mc_type::mc_int)
		return mc_type (basic_mc_type::mc_int, 0); // Widening conversion

	return mc_type (basic_mc_type::mc_char, 0);
}



mc_type semantic_check (const addExpr& node,
                        const symbol_table& local_table,
                        const symbol_table& global_table)
{
	if (!node.lhs)
		return semantic_check (*node.rhs, local_table, global_table);

	mc_type lhs_type = semantic_check (*node.lhs, local_table, global_table);
	mc_type rhs_type = semantic_check (*node.rhs, local_table, global_table);

	if ((lhs_type.type != basic_mc_type::mc_int &&
	     lhs_type.type != basic_mc_type::mc_char) ||
	    (rhs_type.type != basic_mc_type::mc_int &&
	     rhs_type.type != basic_mc_type::mc_char)) // One operand is non-integral
		throw error ("Expression compares non-integral type",
		             node.op->pos ());

	if (lhs_type.type == basic_mc_type::mc_int ||
	    rhs_type.type == basic_mc_type::mc_int)
		return mc_type (basic_mc_type::mc_int, 0); // Widening conversion

	return mc_type (basic_mc_type::mc_char, 0);
}



mc_type semantic_check (const term& node,
                        const symbol_table& local_table,
                        const symbol_table& global_table)
{
	if (!node.lhs)
		return semantic_check (*node.rhs, local_table, global_table);

	mc_type lhs_type = semantic_check (*node.lhs, local_table, global_table);
	mc_type rhs_type = semantic_check (*node.rhs, local_table, global_table);

	if ((lhs_type.type != basic_mc_type::mc_int &&
	     lhs_type.type != basic_mc_type::mc_char) ||
	    (rhs_type.type != basic_mc_type::mc_int &&
	     rhs_type.type != basic_mc_type::mc_char)) // One operand is non-integral
		throw error ("Expression compares non-integral type",
		             node.op->pos ());

	if (lhs_type.type == basic_mc_type::mc_int ||
	    rhs_type.type == basic_mc_type::mc_int)
		return mc_type (basic_mc_type::mc_int, 0); // Widening conversion

	return mc_type (basic_mc_type::mc_char, 0);
}



mc_type semantic_check (const factor& node,
                        const symbol_table& local_table,
                        const symbol_table& global_table)
{
	mc_type rvalue_type;

	switch (node.rvalue->type)
	{
	case AST_type::expression:
		rvalue_type = semantic_check (reinterpret_cast <const expression&> (*node.rvalue),
		                              local_table,
		                              global_table);
		break;

	case AST_type::funcCallExpr:
		rvalue_type = semantic_check (reinterpret_cast <const funcCallExpr&> (*node.rvalue),
		                              local_table,
		                              global_table);
		break;

	case AST_type::var:
		rvalue_type = semantic_check (reinterpret_cast <const var&> (*node.rvalue),
		                              local_table,
		                              global_table);
		break;

	case AST_type::terminal:
	{
		const token& literal_token = reinterpret_cast <const terminal&> (*node.rvalue).token_ref;

		switch (literal_token.type)
		{
		case token_type::int_literal:
			rvalue_type = mc_type (basic_mc_type::mc_int, 0);
			break;
		case token_type::char_literal:
			rvalue_type = mc_type (basic_mc_type::mc_char, 0);
			break;
		default:
			throw error ("Unsupported literal type",
			             literal_token.pos);
		}
		break;
	}

	default:
		throw logic_error ("Bad enumeration value (semantic_check (factor))");
	}

	return rvalue_type;
}



mc_type semantic_check (const funcCallExpr& node,
                        const symbol_table& local_table,
                        const symbol_table& global_table)
{
	const vector <mc_type>* p_signature;
	try
	{
		p_signature = &global_table.at (node.name->token_ref.str).type;
	}
	catch (const out_of_range&)
	{
		throw error ("Undefined function",
		             node.pos ());
	}

	semantic_check (*node.arg_list, *p_signature, local_table, global_table);
	return (*p_signature) [0];
}



void semantic_check (const argList& node,
                     const vector <mc_type>& signature,
                     const symbol_table& local_table,
                     const symbol_table& global_table)
{
	if (node.args.size () != signature.size () - 1)
		throw error ("Wrong number of arguments (expected " + to_string (signature.size () - 1) +
		             "; got " + to_string (node.args.size ()) + ")",
		             node.parent->pos ());

	vector <mc_type> argument_types;
	transform (begin (node.args), end (node.args), back_inserter (argument_types),
	           [&] (const Node <expression>& argument)
	{
		return semantic_check (*argument, local_table, global_table);
	});

	auto bad_argument = mismatch (begin (signature) + 1, end (signature), begin (argument_types),
	                              [] (mc_type parameter_type, mc_type argument_type)
	{
		if ((argument_type.size && !parameter_type.size) ||
		    (!argument_type.size && parameter_type.size))
			return false;
		return argument_type.type == parameter_type.type;
	});

	if (bad_argument.first != end (signature))
		throw error ("Invalid argument to parameter " + to_string (bad_argument.first - begin (signature)) +
		             " of function (expected " + to_string (*bad_argument.first) + "; got " +
		             to_string (*bad_argument.second) + ")",
		             node.args [bad_argument.second - begin (argument_types)]->pos ());
}
