#include <semantic.hh>
#include <tokenizer.hh>

#include <stdexcept>

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
		if (return_type.type == basic_mc_type::mc_void)
			return;
		else
			throw error ("Expected expression in return statement",
			             node->pos ());

	mc_type rexpr_type = semantic_check (*node.rtrn_expr, local_table, global_table);
	if (rexpr_type != return_type)
		throw error ("Type mismatch in return statement (expected " + to_string (return_type) +
		             "; got " + to_string (rexpr_type) + ")",
		             node.rtrn_expr->pos ());
}



void semantic_check (const var& node)
{
}



void semantic_check (const expression& node)
{
}



void semantic_check (const relop& node)
{
}



void semantic_check (const addExpr& node)
{
}



void semantic_check (const addop& node)
{
}



void semantic_check (const term& node)
{
}



void semantic_check (const mulop& node)
{
}



void semantic_check (const factor& node)
{
}



void semantic_check (const funcCallExpr& node)
{
}



void semantic_check (const argList& node)
{
}
