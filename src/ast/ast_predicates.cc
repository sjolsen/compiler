#include <ast/ast_predicates.hh>

using namespace std;



#include <fstream>
#include <algorithm>
#include <iterator>



namespace
{
	ofstream log ("logfile");

	string to_string (AST_node& n)
	{ return to_string (*n); }

	void printargs ()
	{}

	template <typename T, typename... Args>
	void printargs (T&& t, Args&&... args)
	{
		log << to_string (t) << endl;
		printargs (args...);
	}
}



namespace
{
	template <typename... Args>
	AST_node make_node (Args&&... args)
	{
		log << "Making node...\n";
		printargs (args...);
		return make_shared <AST> (forward <Args> (args)...);
	}

	AST_node pop_front (token_range& working_set)
	{
		AST_node front = make_node (working_set [0]);
		working_set.drop_front (1);
		return front;
	}

	AST_node get_identifier (token_range& working_set)
	{
		if (working_set.empty () ||
		    working_set [0].type != token_type::identifier)
			throw syntax_error ("Expected identifier",
			                    begin (working_set)->pos);
		return pop_front (working_set);
	}

	AST_node get_string (token_range& working_set)
	{
		if (working_set.empty () ||
		    working_set [0].type != token_type::string_literal)
			throw syntax_error ("Expected string literal",
			                    begin (working_set)->pos);
		return pop_front (working_set);
	}

	AST_node get_int_l (token_range& working_set)
	{
		if (working_set.empty () ||
		    working_set [0].type != token_type::int_literal)
			throw syntax_error ("Expected int literal",
			                    begin (working_set)->pos);
		return pop_front (working_set);
	}

	AST_node get_char_l (token_range& working_set)
	{
		if (working_set.empty () ||
		    working_set [0].type != token_type::char_literal)
			throw syntax_error ("Expected char literal",
			                    begin (working_set)->pos);
		return pop_front (working_set);
	}

	AST_node get_token (token_range& working_set,
	                    symbol sym)
	{
		if (working_set.empty () ||
		    working_set [0].type != token_type::symbol ||
		    working_set [0].op != sym)
			throw syntax_error ("Expected '" + to_string (sym) + "'",
			                    begin (working_set)->pos);
		return pop_front (working_set);
	}

	AST_node get_token (token_range& working_set,
	                    string keyword)
	{
		if (working_set.empty () ||
		    working_set [0].type != token_type::keyword ||
		    working_set [0].str != keyword)
			throw syntax_error ("Expected '" + keyword + "'",
			                    begin (working_set)->pos);
		return pop_front (working_set);
	}
}



AST program_p (const vector <token>& tokens)
{
	token_range working_set (tokens);

	AST_node decl = decl_p (working_set);
	AST_node declList = declList_p (working_set);

	return AST (AST_type::program, decl, declList);
}



AST_node declList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node decl;
	try
	{
		decl = decl_p (working_set);
	}
	catch (const syntax_error&)
	{
		return make_node ();
	}

	AST_node declList = declList_p (working_set);

	tokens = working_set;
	return make_node (AST_type::declList, decl, declList);
}



AST_node decl_p (token_range& tokens)
{
	try
	{
		return make_node (AST_type::decl, funDecl_p (tokens));
	}
	catch (const syntax_error&)
	{
		return make_node (AST_type::decl, varDeclStmt_p (tokens));
	}
}



AST_node varDeclStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node varDecl = varDecl_p (working_set);
	AST_node scolon_node = get_token (working_set, symbol::semicolon);

	tokens = working_set;
	return make_node (AST_type::varDeclStmt, varDecl, scolon_node);
}



AST_node varDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	// Match `varDecl -> stringDecl`

	try
	{
		AST_node stringDecl = stringDecl_p (working_set);

		tokens = working_set;
		return make_node (AST_type::varDecl, stringDecl);
	}
	catch (const syntax_error&) {}

	AST_node typespec = intTypeSpec_p (working_set);
	AST_node ID = get_identifier (working_set);

	// Match `varDecl -> intTypeSpec ID = expression`

	try
	{
		token_range sub_working_set = working_set;

		AST_node eq_node = get_token (sub_working_set, symbol::equal);
		AST_node initexpr = expression_p (sub_working_set);

		tokens = sub_working_set;
		return make_node (AST_type::varDecl, typespec, ID, eq_node, initexpr);
	}
	catch (const syntax_error&) {}

	// Match `varDecl -> intTypeSpec ID`

	AST_node lbracket_node;
	try
	{
		lbracket_node = get_token (working_set, symbol::lbracket);
	}
	catch (const syntax_error&)
	{
		tokens = working_set;
		return make_node (AST_type::varDecl, typespec, ID);
	}

	// Match `varDecl -> intTypeSpec ID [ ] = initBraceList`

	try
	{
		token_range sub_working_set = working_set;

		AST_node rbracket_node = get_token (sub_working_set, symbol::rbracket);
		AST_node eq_node = get_token (sub_working_set, symbol::equal);
		AST_node bracelist = initBraceList_p (sub_working_set);

		tokens = sub_working_set;
		return make_node (AST_type::varDecl, typespec, ID, lbracket_node,
		                  rbracket_node, eq_node, bracelist);
	}
	catch (const syntax_error&) {}

	// Match `varDecl -> intTypeSpec ID [ expression ] = initBraceList`

	AST_node size_expr = expression_p (working_set);
	AST_node rbracket_node = get_token (working_set, symbol::rbracket);
	try
	{
		token_range sub_working_set = working_set;

		AST_node eq_node = get_token (sub_working_set, symbol::equal);
		AST_node bracelist = initBraceList_p (sub_working_set);

		tokens = sub_working_set;
		return make_node (AST_type::varDecl, typespec, ID, lbracket_node,
		                  size_expr, rbracket_node, eq_node, bracelist);
	}
	catch (const syntax_error&) {}

	// Match `varDecl -> intTypeSpec ID [ expression ]`

	tokens = working_set;
	return make_node (AST_type::varDecl, typespec, ID,
	                  lbracket_node, size_expr, rbracket_node);
}



AST_node stringDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node char_node = get_token (working_set, "char");
	AST_node ID = get_identifier (working_set);
	AST_node lbracket_node = get_token (working_set, symbol::lbracket);

	AST_node size_expr;
	try
	{
		size_expr = expression_p (working_set);
	}
	catch (const syntax_error&) {}

	AST_node rbracket_node = get_token (working_set, symbol::rbracket);
	AST_node eq_node = get_token (working_set, symbol::equal);
	AST_node initstr = get_string (working_set);

	tokens = working_set;
	if (size_expr)
		return make_node (AST_type::varDecl, char_node, ID, lbracket_node,
		                  size_expr, rbracket_node, eq_node, initstr);
	else
		return make_node (AST_type::varDecl, char_node, ID, lbracket_node,
		                  rbracket_node, eq_node, initstr);
}



AST_node intTypeSpec_p (token_range& tokens)
{
	try
	{
		return make_node (AST_type::intTypeSpec, get_token (tokens, "int"));
	}
	catch (const syntax_error&)
	{
		return make_node (AST_type::intTypeSpec, get_token (tokens, "char"));
	}
}



AST_node typeSpecifier_p (token_range& tokens)
{
	try
	{
		return make_node (AST_type::typeSpecifier, intTypeSpec_p (tokens));
	}
	catch (const syntax_error&)
	{
		return make_node (AST_type::typeSpecifier, get_token (tokens, "void"));
	}
}



AST_node initBraceList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lbrace_node = get_token (working_set, symbol::lbrace);
	try
	{
		token_range sub_working_set = working_set;

		AST_node expr_list = exprList_p (sub_working_set);
		AST_node rbrace_node = get_token (sub_working_set, symbol::rbrace);

		tokens = sub_working_set;
		return make_node (AST_type::initBraceList, lbrace_node, expr_list, rbrace_node);
	}
	catch (const syntax_error&) {}

	AST_node rbrace_node = get_token (working_set, symbol::rbrace);

	tokens = working_set;
	return make_node (AST_type::initBraceList, lbrace_node, rbrace_node);
}



AST_node exprList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node head = expression_p (working_set);
	try
	{
		token_range sub_working_set = working_set;

		AST_node comma_node = get_token (sub_working_set, symbol::comma);
		AST_node tail = exprList_p (sub_working_set);

		tokens = sub_working_set;
		return make_node (AST_type::exprList, head, comma_node, tail);
	}
	catch (const syntax_error&)
	{
		tokens = working_set;
		return make_node (AST_type::exprList, head);
	}
}



AST_node funDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node typespec = typeSpecifier_p (working_set);
	AST_node ID = get_identifier (working_set);
	AST_node lparen_node = get_token (working_set, symbol::lparen);

	AST_node decl_list;
	try
	{
		decl_list = formalDeclList_p (working_set);
	}
	catch (const syntax_error&) {}

	AST_node rparen_node = get_token (working_set, symbol::rparen);
	AST_node compound_stmt = compoundStmt_p (working_set);

	tokens = working_set;
	if (decl_list)
		return make_node (AST_type::funDecl, typespec, ID, lparen_node,
		                  decl_list, rparen_node, compound_stmt);
	else
		return make_node (AST_type::funDecl, typespec, ID, lparen_node,
		                  rparen_node, compound_stmt);
}



AST_node formalDeclList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node head = formalDecl_p (working_set);
	try
	{
		token_range sub_working_set = working_set;

		AST_node comma_node = get_token (sub_working_set, symbol::comma);
		AST_node tail = formalDeclList_p (sub_working_set);

		tokens = sub_working_set;
		return make_node (AST_type::formalDeclList, head, comma_node, tail);
	}
	catch (const syntax_error&)
	{
		tokens = working_set;
		return make_node (AST_type::formalDeclList, head);
	}
}



AST_node formalDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	try
	{
		AST_node vardecl = varDecl_p (working_set);

		tokens = working_set;
		return make_node (AST_type::formalDecl, vardecl);
	}
	catch (const syntax_error&) {}

	AST_node typespec = typeSpecifier_p (working_set);
	AST_node ID = get_identifier (working_set);
	AST_node lbracket_node = get_token (working_set, symbol::lbracket);
	AST_node rbracket_node = get_token (working_set, symbol::rbracket);

	tokens = working_set;
	return make_node (AST_type::formalDecl, typespec, ID, lbracket_node, rbracket_node);
}



AST_node statementList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node statement;
	try
	{
		statement = statement_p (working_set);
	}
	catch (const syntax_error&)
	{
		return make_node ();
	}

	AST_node statementList = statementList_p (working_set);

	tokens = working_set;
	return make_node (AST_type::statementList, statement, statementList);
}



AST_node statement_p (token_range& tokens)
{
	try
	{
		return make_node (AST_type::statement, varDeclStmt_p (tokens));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::statement, compoundStmt_p (tokens));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::statement, condStmt_p (tokens));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::statement, loopStmt_p (tokens));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::statement, returnStmt_p (tokens));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::statement, get_token (tokens, symbol::semicolon));
	}
	catch (const syntax_error&) {}

	token_range working_set = tokens;

	AST_node expression = expression_p (working_set);
	AST_node scolon_node = get_token (working_set, symbol::semicolon);

	tokens = working_set;
	return make_node (AST_type::statement, expression, scolon_node);
}



AST_node compoundStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lbrace_node = get_token (working_set, symbol::lbrace);
	AST_node slist = statementList_p (working_set);
	AST_node rbrace_node = get_token (working_set, symbol::rbrace);

	tokens = working_set;
	return make_node (AST_type::compoundStmt, lbrace_node, slist, rbrace_node);
}



AST_node condStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node if_node = get_token (working_set, "if");
	AST_node lparen_node = get_token (working_set, symbol::lparen);
	AST_node expression = expression_p (working_set);
	AST_node rparen_node = get_token (working_set, symbol::rparen);
	AST_node statement = statement_p (working_set);

	try
	{
		token_range sub_working_set = working_set;

		AST_node else_node = get_token (sub_working_set, "else");
		AST_node else_statement = statement_p (working_set);

		tokens = sub_working_set;
		return make_node (AST_type::condStmt, if_node, lparen_node, expression,
		                  rparen_node, statement, else_node, else_statement);
	}
	catch (const syntax_error&) {}

	tokens = working_set;
	return make_node (AST_type::condStmt, if_node, lparen_node,
	                  expression, rparen_node, statement);
}



AST_node loopStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node while_node = get_token (working_set, "while");
	AST_node lparen_node = get_token (working_set, symbol::lparen);
	AST_node expression = expression_p (working_set);
	AST_node rparen_node = get_token (working_set, symbol::rparen);
	AST_node statement = statement_p (working_set);

	tokens = working_set;
	return make_node (AST_type::loopStmt, while_node, lparen_node,
	                  expression, rparen_node, statement);
}



AST_node returnStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node return_node = get_token (working_set, "return");
	AST_node expression;
	try
	{
		expression = expression_p (working_set);
	}
	catch (const syntax_error&) {}
	AST_node scolon_node = get_token (working_set, symbol::semicolon);

	tokens = working_set;
	if (expression)
		return make_node (AST_type::returnStmt, return_node,
		                  expression, scolon_node);
	else
		return make_node (AST_type::returnStmt, return_node, scolon_node);
}



AST_node lvalue_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node ID = get_identifier (working_set);
	AST_node lbracket_node;
	try
	{
		lbracket_node = get_token (working_set, symbol::lbracket);
	}
	catch (const syntax_error&)
	{
		tokens = working_set;
		return make_node (AST_type::lvalue, ID);
	}

	AST_node expression = expression_p (working_set);
	AST_node rbracket_node = get_token (working_set, symbol::rbracket);

	tokens = working_set;
	return make_node (AST_type::lvalue, ID, lbracket_node, expression, rbracket_node);
}



AST_node rvalue_p (token_range& tokens)
{
	try
	{
		return make_node (AST_type::rvalue, funcCallExpr_p (tokens));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::rvalue, lvalue_p (tokens));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::rvalue, get_int_l (tokens));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::rvalue, get_char_l (tokens));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::rvalue, get_string (tokens));
	}
	catch (const syntax_error&) {}

	token_range working_set = tokens;

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	AST_node expression = expression_p (working_set);
	AST_node rparen_node = get_token (working_set, symbol::rparen);

	tokens = working_set;
	return make_node (AST_type::rvalue, lparen_node, expression, rparen_node);
}



AST_node expression_p (token_range& tokens)
{
	try
	{
		token_range working_set = tokens;

		AST_node lvalue_node = lvalue_p (working_set);
		AST_node eq_node = get_token (working_set, symbol::equal);
		AST_node expression = expression_p (working_set);

		tokens = working_set;
		return make_node (AST_type::expression, lvalue_node,
		                  eq_node, expression);
	}
	catch (const syntax_error&) {}

	return make_node (AST_type::expression, equivExpr_p (tokens));
}



AST_node equivExpr_p (token_range& tokens)
{
	AST_node handle = relExpr_p (tokens);

	AST_node equivop_node;
	AST_node rel_expr;
	for (;;)
	{
		token_range working_set = tokens;

		try
		{
			equivop_node = equivop_p (working_set);
			rel_expr = relExpr_p (working_set);

			tokens = working_set;
			handle = make_node (AST_type::equivExpr, handle,
			                    equivop_node, rel_expr);
		}
		catch (const syntax_error&)
		{
			return make_node (AST_type::equivExpr, handle);
		}
	}
}



AST_node equivop_p (token_range& tokens)
{
	try
	{
		return make_node (AST_type::equivop, get_token (tokens, symbol::equivalent));
	}
	catch (const syntax_error&)
	{
		return make_node (AST_type::equivop, get_token (tokens, symbol::not_equiv));
	}
}



AST_node relExpr_p (token_range& tokens)
{
	AST_node handle = addExpr_p (tokens);

	AST_node relop_node;
	AST_node add_expr;
	for (;;)
	{
		token_range working_set = tokens;

		try
		{
			relop_node = relop_p (working_set);
			add_expr = addExpr_p (working_set);

			tokens = working_set;
			handle = make_node (AST_type::relExpr, handle,
			                    relop_node, add_expr);
		}
		catch (const syntax_error&)
		{
			return make_node (AST_type::relExpr, handle);
		}
	}
}



AST_node relop_p (token_range& tokens)
{
	try
	{
		return make_node (AST_type::equivop, get_token (tokens, symbol::gt_equiv));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::equivop, get_token (tokens, symbol::greater_than));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::equivop, get_token (tokens, symbol::lt_equiv));
	}
	catch (const syntax_error&) {}

	return make_node (AST_type::equivop, get_token (tokens, symbol::less_than));
}



AST_node addExpr_p (token_range& tokens)
{
	AST_node handle = term_p (tokens);

	AST_node addop_node;
	AST_node term;
	for (;;)
	{
		token_range working_set = tokens;

		try
		{
			addop_node = addop_p (working_set);
			term = term_p (working_set);

			tokens = working_set;
			handle = make_node (AST_type::addExpr, handle, addop_node, term);
		}
		catch (const syntax_error&)
		{
			return make_node (AST_type::addExpr, handle);
		}
	}
}



AST_node addop_p (token_range& tokens)
{
	try
	{
		return make_node (AST_type::addop, get_token (tokens, symbol::plus));
	}
	catch (const syntax_error&)
	{
		return make_node (AST_type::addop, get_token (tokens, symbol::minus));
	}
}



AST_node term_p (token_range& tokens)
{
	AST_node handle = rvalue_p (tokens);

	AST_node mulop_node;
	AST_node rvalue;
	for (;;)
	{
		token_range working_set = tokens;

		try
		{
			mulop_node = mulop_p (working_set);
			rvalue = rvalue_p (working_set);

			tokens = working_set;
			handle = make_node (AST_type::term, handle, mulop_node, rvalue);
		}
		catch (const syntax_error&)
		{
			return make_node (AST_type::term, handle);
		}
	}
}



AST_node mulop_p (token_range& tokens)
{
	try
	{
		return make_node (AST_type::mulop, get_token (tokens, symbol::asterisk));
	}
	catch (const syntax_error&)
	{
		return make_node (AST_type::mulop, get_token (tokens, symbol::solidus));
	}
}



AST_node funcCallExpr_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node ID = get_identifier (working_set);
	AST_node arg_list = argList_p (working_set);

	tokens = working_set;
	return make_node (AST_type::funcCallExpr, ID, arg_list);
}



AST_node argList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	AST_node expr_list;
	try
	{
		expr_list = exprList_p (working_set);
	}
	catch (const syntax_error&) {}
	AST_node rparen_node = get_token (working_set, symbol::rparen);

	tokens = working_set;
	if (expr_list)
		return make_node (AST_type::argList, lparen_node,
		                  expr_list, rparen_node);
	else
		return make_node (AST_type::argList, lparen_node, rparen_node);
}
