#include <ast/ast_nodes.hh>



file_position returnStmt::pos () const
{
	if (rtrn_expr)
		return rtrn_expr->pos ();
	return rtrn_kwd->pos ();
}

file_position var::pos () const
{
	return name->pos ();
}

file_position expression::pos () const
{
	if (op)
		return op->pos ();
	return rhs->pos ();
}

file_position relop::pos () const
{
	return sym->pos ();
}

file_position addExpr::pos () const
{
	if (op)
		return op->pos ();
	return rhs->pos ();
}

file_position addop::pos () const
{
	return sym->pos ();
}

file_position mulop::pos () const
{
	return sym->pos ();
}

file_position funcCallExpr::pos () const
{
	return name->pos ();
}

