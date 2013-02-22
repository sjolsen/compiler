#include <ast/ast_predicates.hh>

using namespace std;



#include <fstream>
#include <algorithm>
#include <iterator>



namespace
{
	ofstream logfile ("logfile");

	string to_string (AST_node& n)
	{
		if (!n)
			throw logic_error ("Tried to print a null node");
		return to_string (*n);
	}

	void printargs ()
	{}

	template <typename T, typename... Args>
	void printargs (T&& t, Args&&... args)
	{
		logfile << to_string (t) << endl;
		printargs (args...);
	}
}



namespace
{
	template <typename... Args>
	AST_node make_node (Args&&... args)
	{
		logfile << "Making node =======================\n";
		printargs (args...);
		auto node = make_shared <AST> (forward <Args> (args)...);
		logfile << "============================== Done\n";
		return node;
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

	AST_node get_token (string f,
	                    token_range& working_set,
	                    symbol sym)
	{
		if (working_set.empty () ||
		    working_set [0].type != token_type::symbol ||
		    working_set [0].op != sym)
			throw syntax_error (f + " Expected '" + to_string (sym) + "'",
			                    begin (working_set)->pos);
		return pop_front (working_set);
	}

	AST_node get_token (string f,
	                    token_range& working_set,
	                    string keyword)
	{
		if (working_set.empty () ||
		    working_set [0].type != token_type::keyword ||
		    working_set [0].str != keyword)
			throw syntax_error (f + " Expected '" + keyword + "'",
			                    begin (working_set)->pos);
		return pop_front (working_set);
	}

	typename token_range::iterator
	find_close (token_range r, symbol open, symbol close)
	{
		int i = 1;

		auto pred = [&] (const token& t)
		{
			if (t.type == token_type::symbol)
				if (t.op == open)
					++i;
				else if (t.op == close)
					--i;
			return i == 0;
		};

		while (r)
			if (pred (r [0]))
				return begin (r);
			else
				r.drop_front (1);
		return end (r);
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

	AST_node decl = decl_p (working_set);
	if (!decl)
		return make_node ();

	AST_node declList = declList_p (working_set);
	if (!declList)
		return make_node ();

	tokens = working_set;
	return make_node (AST_type::declList, decl, declList);
}



AST_node decl_p (token_range& tokens)
{
	AST_node sub_decl = funDecl_p (tokens);
	if (sub_decl)
		return make_node (AST_type::decl, sub_decl);

	sub_decl = varDeclStmt_p (tokens);
	if (sub_decl)
		return make_node (AST_type::decl, sub_decl);

	return nullptr;
}



AST_node varDeclStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node varDecl = varDecl_p (working_set);
	if (!varDecl)
		return nullptr;
	AST_node scolon_node = get_token (__FUNCTION__, working_set, symbol::semicolon);

	tokens = working_set;
	return make_node (AST_type::varDeclStmt, varDecl, scolon_node);
}



AST_node varDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	// Match `varDecl -> stringDecl`

	AST_node stringDecl = stringDecl_p (working_set);
	if (stringDecl)
	{
		tokens = working_set;
		return make_node (AST_type::varDecl, stringDecl);
	}

	// Match `varDecl -> intTypeSpec ID = expression`

	AST_node typespec = intTypeSpec_p (working_set);
	if (!typespec)
		return nullptr;
	AST_node ID;
	try
	{
		 ID = get_identifier (working_set);
	}
	catch (const syntax_error&)
	{
		return nullptr;
	}

	{
		token_range sub_working_set = working_set;

		AST_node eq_node;
		try
		{
			eq_node = get_token (__FUNCTION__, sub_working_set, symbol::equal);
		}
		catch (const syntax_error&) {}
		if (eq_node)
		{
			AST_node initexpr = expression_p (sub_working_set);

			tokens = sub_working_set;
			return make_node (AST_type::varDecl, typespec, ID, eq_node, initexpr);
		}
	}

	// Match `varDecl -> intTypeSpec ID`

	AST_node lbracket_node;
	try
	{
		lbracket_node = get_token (__FUNCTION__, working_set, symbol::lbracket);
	}
	catch (const syntax_error&)
	{
		tokens = working_set;
		return make_node (AST_type::varDecl, typespec, ID);
	}

	// Match `varDecl -> intTypeSpec ID [ ] = initBraceList`

	auto rbracket_iter = find_close (working_set, symbol::lbracket, symbol::rbracket);
	if (rbracket_iter == end (working_set))
		throw syntax_error ("Unmatched '['",
		                    lbracket_node->tokenp->pos);

	token_range size_expr_range (begin (working_set), rbracket_iter);
	if (!size_expr_range)
	{
		token_range sub_working_set = working_set;

		AST_node rbracket_node = get_token (__FUNCTION__, sub_working_set, symbol::rbracket);
		AST_node eq_node;
		try
		{
			eq_node = get_token (__FUNCTION__, sub_working_set, symbol::equal);
		}
		catch (const syntax_error&)
		{
			return nullptr;
		}

		AST_node bracelist = initBraceList_p (sub_working_set);
		if (!bracelist)
			throw syntax_error (string (__FUNCTION__) + " Expected brace-enclosed initializer list",
			                    begin (sub_working_set)->pos);

		tokens = sub_working_set;
		return make_node (AST_type::varDecl, typespec, ID, lbracket_node,
		                  rbracket_node, eq_node, bracelist);
	}

	// Match `varDecl -> intTypeSpec ID [ expression ] = initBraceList`

	AST_node size_expr = expression_p (size_expr_range);
	if (size_expr_range)
		throw syntax_error (string (__FUNCTION__) + " Expected ']'",
		                    end (size_expr_range)->pos);

	working_set = token_range (end (size_expr_range), end (working_set));
	AST_node rbracket_node = get_token (__FUNCTION__, working_set, symbol::rbracket);

	AST_node eq_node;
	try
	{
		eq_node = get_token (__FUNCTION__, working_set, symbol::equal);
	}
	catch (const syntax_error&) {}
	if (eq_node)
	{
		AST_node bracelist = initBraceList_p (working_set);
		if (!bracelist)
			throw syntax_error (string (__FUNCTION__) + " Expected brace-enclosed initializer list",
			                    begin (working_set)->pos);

		tokens = working_set;
		return make_node (AST_type::varDecl, typespec, ID, lbracket_node,
		                  size_expr, rbracket_node, eq_node, bracelist);
	}

	// Match `varDecl -> intTypeSpec ID [ expression ]`
	tokens = working_set;
	return make_node (AST_type::varDecl, typespec, ID,
	                  lbracket_node, size_expr, rbracket_node);
}



AST_node stringDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node char_node;
	try
	{
		char_node = get_token (__FUNCTION__, working_set, "char");
	}
	catch (const syntax_error&)
	{
		return nullptr;
	}
	AST_node ID = get_identifier (working_set);
	AST_node lbracket_node;
	try
	{
		lbracket_node = get_token (__FUNCTION__, working_set, symbol::lbracket);
	}
	catch (const syntax_error&)
	{
		return nullptr;
	}

	// Match `stringDecl -> intTypeSpec ID [ ] = STRING`

	auto rbracket_iter = find_close (working_set, symbol::lbrace, symbol::rbrace);
	if (rbracket_iter == end (working_set))
		throw syntax_error ("Unmatched '['",
		                    lbracket_node->tokenp->pos);

	token_range size_expr_range (begin (working_set), rbracket_iter);
	if (!size_expr_range)
	{
		token_range sub_working_set = working_set;

		AST_node rbracket_node = get_token (__FUNCTION__, sub_working_set, symbol::rbracket);
		AST_node eq_node = get_token (__FUNCTION__, sub_working_set, symbol::equal);
		AST_node initstr = get_string (sub_working_set);
		if (!initstr)
			return nullptr;

		tokens = sub_working_set;
		return make_node (AST_type::stringDecl, char_node, ID, lbracket_node,
		                  rbracket_node, eq_node, initstr);
	}

	// Match `stringDecl -> intTypeSpec ID [ expression ] = STRING`

	AST_node size_expr = expression_p (size_expr_range);
	if (size_expr_range)
		throw syntax_error (string (__FUNCTION__) + " Expected ']'",
		                    end (size_expr_range)->pos);

	working_set = token_range (end (size_expr_range), end (working_set));
	AST_node rbracket_node = get_token (__FUNCTION__, working_set, symbol::rbracket);

	AST_node eq_node;
	try
	{
		eq_node = get_token (__FUNCTION__, working_set, symbol::equal);
	}
	catch (const syntax_error&) {}
	if (eq_node)
	{
		AST_node initstr = get_string (working_set);
		if (!initstr)
			return nullptr;

		tokens = working_set;
		return make_node (AST_type::stringDecl, char_node, ID, lbracket_node,
		                  size_expr, rbracket_node, eq_node, initstr);
	}

	return make_node (AST_type::stringDecl, char_node, ID, lbracket_node,
	                  size_expr, rbracket_node);
}



AST_node intTypeSpec_p (token_range& tokens)
{
	try
	{
		return make_node (AST_type::intTypeSpec, get_token (__FUNCTION__, tokens, "int"));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::intTypeSpec, get_token (__FUNCTION__, tokens, "char"));
	}
	catch (const syntax_error&) {}

	return nullptr;
}



AST_node typeSpecifier_p (token_range& tokens)
{
	AST_node int_node = intTypeSpec_p (tokens);
	if (int_node)
		return make_node (AST_type::typeSpecifier, int_node);

	try
	{
		return make_node (AST_type::typeSpecifier, get_token (__FUNCTION__, tokens, "void"));
	}
	catch (const syntax_error&) {}

	return nullptr;
}



AST_node initBraceList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lbrace_node = get_token (__FUNCTION__, working_set, symbol::lbrace);

	auto rbrace_iter = find_close (working_set, symbol::lbracket, symbol::rbracket);
	if (rbrace_iter == end (working_set))
		throw syntax_error ("Unmatched '{'",
		                    lbrace_node->tokenp->pos);

	token_range exprlist_range (begin (working_set), rbrace_iter);
	if (!exprlist_range)
	{
		AST_node rbrace_node = get_token (__FUNCTION__, working_set, symbol::rbrace);

		tokens = working_set;
		return make_node (AST_type::initBraceList, lbrace_node, rbrace_node);
	}

	AST_node expr_list = exprList_p (exprlist_range);
	if (exprlist_range)
		throw syntax_error (string (__FUNCTION__) + " Expected '}'",
		                    begin (exprlist_range)->pos);

	working_set = token_range (rbrace_iter, end (working_set));
	AST_node rbrace_node = get_token (__FUNCTION__, working_set, symbol::rbrace);

	tokens = working_set;
	return make_node (AST_type::initBraceList, lbrace_node, expr_list, rbrace_node);
}



AST_node exprList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node head = expression_p (working_set);
	AST_node comma_node = get_token (__FUNCTION__, working_set, symbol::comma);
	if (!comma_node)
	{
		tokens = working_set;
		return make_node (AST_type::exprList, head);
	}

	AST_node tail = exprList_p (working_set);

	tokens = working_set;
	return make_node (AST_type::exprList, head, comma_node, tail);
}



AST_node funDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node typespec = typeSpecifier_p (working_set);
	if (!typespec)
		return nullptr;

	AST_node ID;
	AST_node lparen_node;
	try
	{
		ID = get_identifier (working_set);
		lparen_node = get_token (__FUNCTION__, working_set, symbol::lparen);
	}
	catch (const syntax_error&)
	{
		return nullptr;
	}

	auto rparen_iter = find_close (working_set, symbol::lparen, symbol::rparen);
	if (rparen_iter == end (working_set))
		throw syntax_error ("Unmatched '('",
		                    lparen_node->tokenp->pos);

	token_range decl_list_range (begin (working_set), rparen_iter);
	AST_node decl_list;
	if (decl_list_range)
		decl_list = formalDeclList_p (decl_list_range);
	if (decl_list_range)
		throw syntax_error (string (__FUNCTION__) + " Expected ')'",
		                    begin (decl_list_range)->pos);

	working_set = token_range (rparen_iter, end (working_set));
	AST_node rparen_node = get_token (__FUNCTION__, working_set, symbol::rparen);
	AST_node compound_stmt = compoundStmt_p (working_set);
	if (!compound_stmt)
		return nullptr;

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
	if (!head)
		return nullptr;

	AST_node comma_node;
	try
	{
		comma_node = get_token (__FUNCTION__, working_set, symbol::comma);
	}
	catch (const syntax_error&)
	{
		tokens = working_set;
		return make_node (AST_type::formalDeclList, head);
	}

	AST_node tail = formalDeclList_p (working_set);
	if (!tail)
		throw syntax_error (string (__FUNCTION__) + " Expected parameter declaration",
		                    begin (working_set)->pos);

	tokens = working_set;
//	return make_node (AST_type::formalDeclList, head, comma_node, tail);
	AST_node list = make_node (AST_type::formalDeclList, head, comma_node);
	for (AST_node& child : tail->children)
		list->children.emplace_back (move (child));
	return list;
}



AST_node formalDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node vardecl = varDecl_p (working_set);
	if (vardecl)
	{
		tokens = working_set;
		return make_node (AST_type::formalDecl, vardecl);
	}

	AST_node typespec = typeSpecifier_p (working_set);
	AST_node ID = get_identifier (working_set);
	AST_node lbracket_node = get_token (__FUNCTION__, working_set, symbol::lbracket);
	AST_node rbracket_node = get_token (__FUNCTION__, working_set, symbol::rbracket);

	tokens = working_set;
	return make_node (AST_type::formalDecl, typespec, ID, lbracket_node, rbracket_node);
}



AST_node statementList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node statement = statement_p (working_set);
	if (!statement)
		return make_node ();

	AST_node statementList = statementList_p (working_set);
	if (!statementList)
		throw logic_error ("statementList returned a null pointer");

	tokens = working_set;
	return make_node (AST_type::statementList, statement, statementList);
}



AST_node statement_p (token_range& tokens)
{
	AST_node sub_statement;
	sub_statement = varDeclStmt_p (tokens);
	if (sub_statement)
		return make_node (AST_type::statement, sub_statement);

	sub_statement = compoundStmt_p (tokens);
	if (sub_statement)
		return make_node (AST_type::statement, sub_statement);

	sub_statement = condStmt_p (tokens);
	if (sub_statement)
		return make_node (AST_type::statement, sub_statement);

	sub_statement = loopStmt_p (tokens);
	if (sub_statement)
		return make_node (AST_type::statement, sub_statement);

	sub_statement = returnStmt_p (tokens);
	if (sub_statement)
		return make_node (AST_type::statement, sub_statement);

	try
	{
		sub_statement = get_token (__FUNCTION__, tokens, symbol::semicolon);
		return make_node (AST_type::statement, sub_statement);
	}
	catch (const syntax_error&) {}

	token_range working_set = tokens;

	AST_node expression = expression_p (working_set);
	if (!expression)
		return nullptr;
	AST_node scolon_node = get_token (__FUNCTION__, working_set, symbol::semicolon);

	tokens = working_set;
	return make_node (AST_type::statement, expression, scolon_node);
}



AST_node compoundStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lbrace_node;
	try
	{
		lbrace_node = get_token (__FUNCTION__, working_set, symbol::lbrace);
	}
	catch (const syntax_error&)
	{
		return nullptr;
	}

	auto rbrace_iter = find_close (working_set, symbol::lbrace, symbol::rbrace);
	if (rbrace_iter == end (working_set))
		throw syntax_error ("Unmatched '{'",
		                    lbrace_node->tokenp->pos);

	token_range stmt_list_range (begin (working_set), rbrace_iter);
	if (!stmt_list_range)
	{
		AST_node rbrace_node = get_token (__FUNCTION__, working_set, symbol::rbrace);

		tokens = working_set;
		return make_node (AST_type::compoundStmt, lbrace_node, rbrace_node);
	}

	AST_node stmt_list = statementList_p (stmt_list_range);
	if (stmt_list_range)
		throw syntax_error (string (__FUNCTION__) + " Expected '}'",
		                    begin (stmt_list_range)->pos);

	working_set = token_range (rbrace_iter, end (working_set));
	AST_node rbrace_node = get_token (__FUNCTION__, working_set, symbol::rbrace);

	tokens = working_set;
	return make_node (AST_type::compoundStmt, lbrace_node, stmt_list, rbrace_node);
}



AST_node condStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node if_node;
	try
	{
		if_node = get_token (__FUNCTION__, working_set, "if");
	}
	catch (const syntax_error&)
	{
		return nullptr;
	}

	AST_node lparen_node = get_token (__FUNCTION__, working_set, symbol::lparen);



	auto rparen_iter = find_close (working_set, symbol::lparen, symbol::rparen);
	if (rparen_iter == end (working_set))
		throw syntax_error ("Unmatched '('",
		                    lparen_node->tokenp->pos);

	token_range expression_range (begin (working_set), rparen_iter);
	AST_node expression;
	if (expression_range)
		expression = expression_p (expression_range);
	if (expression_range)
		throw syntax_error (string (__FUNCTION__) + " Expected ')'",
		                    begin (expression_range)->pos);

	working_set = token_range (rparen_iter, end (working_set));
	AST_node rparen_node = get_token (__FUNCTION__, working_set, symbol::rparen);
	AST_node statement = statement_p (working_set);

	AST_node else_node;
	try
	{
		else_node = get_token (__FUNCTION__, working_set, "else");
	}
	catch (const syntax_error&)
	{
		tokens = working_set;
		return make_node (AST_type::condStmt, if_node, lparen_node,
		                  expression, rparen_node, statement);
	}
	AST_node else_statement = statement_p (working_set);

	tokens = working_set;
	return make_node (AST_type::condStmt, if_node, lparen_node, expression,
	                  rparen_node, statement, else_node, else_statement);
}



AST_node loopStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node while_node;
	try
	{
		while_node = get_token (__FUNCTION__, working_set, "while");
	}
	catch (const syntax_error&)
	{
		return nullptr;
	}

	AST_node lparen_node = get_token (__FUNCTION__, working_set, symbol::lparen);

	auto rparen_iter = find_close (working_set, symbol::lparen, symbol::rparen);
	if (rparen_iter == end (working_set))
		throw syntax_error ("Unmatched '('",
		                    lparen_node->tokenp->pos);

	token_range expression_range (begin (working_set), rparen_iter);
	AST_node expression;
	if (expression_range)
		expression = expression_p (expression_range);
	if (expression_range)
		throw syntax_error (string (__FUNCTION__) + " Expected ')'",
		                    begin (expression_range)->pos);

	working_set = token_range (rparen_iter, end (working_set));
	AST_node rparen_node = get_token (__FUNCTION__, working_set, symbol::rparen);
	AST_node statement = statement_p (working_set);

	tokens = working_set;
	return make_node (AST_type::loopStmt, while_node, lparen_node,
	                  expression, rparen_node, statement);
}



AST_node returnStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node return_node;
	try
	{
		return_node = get_token (__FUNCTION__, working_set, "return");
	}
	catch (const syntax_error&)
	{
		return nullptr;
	}

	AST_node scolon_node;
	try
	{
		scolon_node = get_token (__FUNCTION__, working_set, symbol::semicolon);

		tokens = working_set;
		return make_node (AST_type::returnStmt, return_node, scolon_node);
	}
	catch (const syntax_error&) {}

	AST_node expression = expression_p (working_set);
	if (!expression)
		throw syntax_error ("Invalid expression in return statement",
		                    begin (working_set)->pos);
	scolon_node = scolon_node = get_token (__FUNCTION__, working_set, symbol::semicolon);

	tokens = working_set;
	return make_node (AST_type::returnStmt, return_node, expression, scolon_node);
}



AST_node lvalue_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node ID;
	try
	{
		ID = get_identifier (working_set);
	}
	catch (const syntax_error&)
	{
		return nullptr;
	}

	AST_node lbracket_node;
	try
	{
		lbracket_node = get_token (__FUNCTION__, working_set, symbol::lbracket);
	}
	catch (const syntax_error&)
	{
		tokens = working_set;
		return make_node (AST_type::lvalue, ID);
	}

	auto rbracket_iter = find_close (working_set, symbol::lbracket, symbol::rbracket);
	if (rbracket_iter == end (working_set))
		throw syntax_error ("Unmatched '['",
		                    lbracket_node->tokenp->pos);

	token_range expression_range (begin (working_set), rbracket_iter);
	AST_node expression;
	if (expression_range)
		expression = expression_p (expression_range);
	if (expression_range)
		throw syntax_error (string (__FUNCTION__) + " Expected ']'",
		                    begin (expression_range)->pos);

	working_set = token_range (rbracket_iter, end (working_set));
	AST_node rbracket_node = get_token (__FUNCTION__, working_set, symbol::rbracket);

	tokens = working_set;
	return make_node (AST_type::lvalue, ID, lbracket_node, expression, rbracket_node);
}



AST_node rvalue_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lparen_node;
	try
	{
		lparen_node = get_token (__FUNCTION__, working_set, symbol::lparen);
	}
	catch (const syntax_error&) {}
	if (lparen_node)
	{
		auto rparen_iter = find_close (working_set, symbol::lparen, symbol::rparen);
		if (rparen_iter == end (working_set))
			throw syntax_error ("Unmatched '('",
			                    lparen_node->tokenp->pos);

		token_range expression_range (begin (working_set), rparen_iter);
		AST_node expression;
		if (expression_range)
			expression = expression_p (expression_range);
		if (expression_range)
			throw syntax_error (string (__FUNCTION__) + " Expected ')'",
			                    begin (expression_range)->pos);

		working_set = token_range (rparen_iter, end (working_set));
		AST_node rparen_node = get_token (__FUNCTION__, working_set, symbol::rparen);

		tokens = working_set;
		return make_node (AST_type::rvalue, lparen_node, expression, rparen_node);
	}

	AST_node subvalue;
	subvalue = funcCallExpr_p (tokens);
	if (subvalue)
		return make_node (AST_type::rvalue, subvalue);

	subvalue = lvalue_p (tokens);
	if (subvalue)
		return make_node (AST_type::rvalue, subvalue);

	try
	{
		subvalue = get_int_l (tokens);
		return make_node (AST_type::rvalue, subvalue);
	}
	catch (const syntax_error&) {}

	try
	{
		subvalue = get_char_l (tokens);
		return make_node (AST_type::rvalue, subvalue);
	}
	catch (const syntax_error&) {}

	try
	{
		subvalue = get_string (tokens);
		return make_node (AST_type::rvalue, subvalue);
	}
	catch (const syntax_error&) {}

	return nullptr;
}



AST_node expression_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lvalue_node;
	AST_node eq_node;
	try
	{
		lvalue_node = lvalue_p (working_set);
		eq_node = get_token (__FUNCTION__, working_set, symbol::equal);
	}
	catch (const syntax_error&)
	{
		AST_node equiv_node = equivExpr_p (tokens);
		if (equiv_node)
			return make_node (AST_type::expression, equiv_node);
		return nullptr;
	}

	AST_node expression = expression_p (working_set);

	tokens = working_set;
	return make_node (AST_type::expression, lvalue_node,
	                  eq_node, expression);
}



AST_node equivExpr_p (token_range& tokens)
{
	AST_node handle = relExpr_p (tokens);
	if (!handle)
		return nullptr;

	AST_node equivop_node;
	AST_node rel_expr;
	for (;;)
	{
		token_range working_set = tokens;

		equivop_node = equivop_p (working_set);
		if (!equivop_node)
			break;

		rel_expr = relExpr_p (working_set);
		if (!rel_expr)
			break;

		tokens = working_set;
		handle = make_node (AST_type::equivExpr, handle,
		                    equivop_node, rel_expr);
	}

	return make_node (AST_type::equivExpr, handle);
}



AST_node equivop_p (token_range& tokens)
{
	try
	{
		return make_node (AST_type::equivop, get_token (__FUNCTION__, tokens, symbol::equivalent));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::equivop, get_token (__FUNCTION__, tokens, symbol::not_equiv));
	}
	catch (const syntax_error&) {}

	return nullptr;
}



AST_node relExpr_p (token_range& tokens)
{
	AST_node handle = addExpr_p (tokens);
	if (!handle)
		return nullptr;

	AST_node relop_node;
	AST_node add_expr;
	for (;;)
	{
		token_range working_set = tokens;

		relop_node = relop_p (working_set);
		if (!relop_node)
			break;

		add_expr = addExpr_p (working_set);
		if (!add_expr)
			break;

		tokens = working_set;
		handle = make_node (AST_type::relExpr, handle,
		                    relop_node, add_expr);
	}

	return make_node (AST_type::relExpr, handle);
}



AST_node relop_p (token_range& tokens)
{
	try
	{
		return make_node (AST_type::equivop, get_token (__FUNCTION__, tokens, symbol::gt_equiv));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::equivop, get_token (__FUNCTION__, tokens, symbol::greater_than));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::equivop, get_token (__FUNCTION__, tokens, symbol::lt_equiv));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::equivop, get_token (__FUNCTION__, tokens, symbol::less_than));
	}
	catch (const syntax_error&) {}

	return nullptr;
}



AST_node addExpr_p (token_range& tokens)
{
	AST_node handle = term_p (tokens);
	if (!handle)
		return nullptr;

	AST_node addop_node;
	AST_node term;
	for (;;)
	{
		token_range working_set = tokens;

		addop_node = addop_p (working_set);
		if (!addop_node)
			break;

		term = term_p (working_set);
		if (!term)
			break;

		tokens = working_set;
		handle = make_node (AST_type::addExpr, handle, addop_node, term);
	}

	return make_node (AST_type::addExpr, handle);
}



AST_node addop_p (token_range& tokens)
{
	try
	{
		return make_node (AST_type::addop, get_token (__FUNCTION__, tokens, symbol::plus));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::addop, get_token (__FUNCTION__, tokens, symbol::minus));
	}
	catch (const syntax_error&) {}

	return nullptr;
}



AST_node term_p (token_range& tokens)
{
	AST_node handle = rvalue_p (tokens);
	if (!handle)
		return nullptr;

	AST_node mulop_node;
	AST_node rvalue;
	for (;;)
	{
		token_range working_set = tokens;

		mulop_node = mulop_p (working_set);
		if (!mulop_node)
			break;

		rvalue = rvalue_p (working_set);
		if (!rvalue)
			break;

		tokens = working_set;
		handle = make_node (AST_type::term, handle, mulop_node, rvalue);
	}

	return make_node (AST_type::term, handle);
}



AST_node mulop_p (token_range& tokens)
{
	try
	{
		return make_node (AST_type::mulop, get_token (__FUNCTION__, tokens, symbol::asterisk));
	}
	catch (const syntax_error&) {}

	try
	{
		return make_node (AST_type::mulop, get_token (__FUNCTION__, tokens, symbol::solidus));
	}
	catch (const syntax_error&) {}

	return nullptr;
}



AST_node funcCallExpr_p (token_range& tokens)
{
	token_range working_set = tokens;

	try
	{
		AST_node ID = get_identifier (working_set);
		AST_node arg_list = argList_p (working_set);
		if (!arg_list)
			return nullptr;

		tokens = working_set;
		return make_node (AST_type::funcCallExpr, ID, arg_list);
	}
	catch (const syntax_error&) {}

	return nullptr;
}



AST_node argList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lparen_node;
	try
	{
		lparen_node = get_token (__FUNCTION__, working_set, symbol::lparen);
	}
	catch (const syntax_error&)
	{
		return nullptr;
	}

	auto rparen_iter = find_close (working_set, symbol::lparen, symbol::rparen);
	if (rparen_iter == end (working_set))
		throw syntax_error ("Unmatched '('",
		                    lparen_node->tokenp->pos);

	token_range expr_list_range (begin (working_set), rparen_iter);
	AST_node expr_list;
	if (expr_list_range)
		expr_list = exprList_p (expr_list_range);
	if (expr_list_range)
		throw syntax_error (string (__FUNCTION__) + " Expected ')'",
		                    begin (expr_list_range)->pos);

	working_set = token_range (rparen_iter, end (working_set));
	AST_node rparen_node = get_token (__FUNCTION__, working_set, symbol::rparen);

	tokens = working_set;
	if (expr_list)
		return make_node (AST_type::argList, lparen_node,
		                  expr_list, rparen_node);
	else
		return make_node (AST_type::argList, lparen_node, rparen_node);
}
