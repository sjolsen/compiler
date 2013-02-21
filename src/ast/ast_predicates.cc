#include <ast/ast_predicates.hh>

using namespace std;



namespace
{
	template <typename... Args>
	AST_node make_node (Args... args)
	{
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

	AST_node get_token (token_range& working_set,
	                    symbol sym)
	{
		if (working_set.empty () ||
		    working_set [0].type != token_type::symbol)
			throw syntax_error ("Expected " + to_string (sym),
			                    begin (working_set)->pos);
		return pop_front (working_set);
	}

	AST_node get_token (token_range& working_set,
	                    string keyword)
	{
		if (working_set.empty () ||
		    working_set [0].type != token_type::keyword)
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

	if (!working_set.empty ())
		throw syntax_error ("Expected a declaration",
		                    begin (working_set)->pos);

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
	token_range working_set = tokens;

	AST_node subdecl;
	try
	{
		subdecl = funDecl_p (working_set);

		tokens = working_set;
		return make_node (AST_type::decl, subdecl);
	}
	catch (const syntax_error&) {}

	try
	{
		subdecl = varDeclStmt_p (working_set);

		tokens = working_set;
		return make_node (AST_type::decl, subdecl);
	}
	catch (const syntax_error&)
	{
		throw syntax_error ("Expected a declaration",
		                    begin (working_set)->pos);
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

	token_range working_set = tokens;

	AST_node expression = expression_p (working_set);
	AST_node scolon = get_token (working_set, symbol::semicolon);

	tokens = working_set;
	return make_node (AST_type::statement, expression, scolon);
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
