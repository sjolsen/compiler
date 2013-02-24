#include <ast/ast_predicates.hh>

using namespace std;



#include <fstream>
#include <algorithm>
#include <stack>
#include <chrono>



#ifdef DEBUG
	#define LOGFILESTR logfile_name ()
	#define throw_error(str, pos) throw syntax_error (string (__FUNCTION__) + ": " str, pos)
	#define CALL(funcall) [&] { logfile << string (ident_level, '\t') << "Calling " #funcall << endl; \
	                            ++ident_level; \
	                            auto result = funcall; \
	                            --ident_level; \
	                            logfile << string (ident_level, '\t') << "Done" << endl; \
	                            return result; } ()
#else
	#define LOGFILESTR "/dev/null"
	#define throw_error(str, pos) throw syntax_error (str, pos)
	#define CALL(funcall) funcall
#endif



#define validate(node) if (!node) return nullptr;



namespace
{
	string logfile_name ()
	{
		using namespace chrono;
		return to_string (duration_cast <milliseconds> (system_clock::now ().time_since_epoch ()).count ()) + ".mcclog";
	}

	ofstream logfile (LOGFILESTR);
	int ident_level = 0;

	string to_string (const AST_node& node)
	{
		if (!node)
			throw logic_error ("Tried to dereference a null AST_node");
		return to_string (*node, string (ident_level, '\t'));
	}

	void printargs ()
	{
	}

	template <typename... Rest>
	void printargs (const AST_node& first,
	                Rest&&... rest)
	{
		logfile << to_string (first);
		printargs (rest...);
	}
}



namespace
{
	template <typename... Args>
	AST_node make_node (AST_type type,
	                    Args&&... args)
	{
		logfile << string (ident_level, '\t')
		        << "Making node (" << to_string (type) << "):\n";
		++ident_level;
		printargs (forward <Args> (args)...);

		auto node = make_shared <AST> (type, forward <Args> (args)...);

		--ident_level;
		logfile << string (ident_level, '\t')
		        << "Done\n";

		return node;
	}

	AST_node make_node (const token& t)
	{
		return make_shared <AST> (t);
	}

	AST_node pop_front (token_range& working_set)
	{
		AST_node front = make_node (working_set [0]);
		working_set.drop_front (1);
		return front;
	}

	AST_node get_token (token_range& working_set,
	                    token_type type)
	{
		if (working_set.empty ())
			return nullptr;
		if (working_set [0].type != type)
			return nullptr;

		return pop_front (working_set);
	}

	AST_node get_token (token_range& working_set,
	                    symbol sym)
	{
		if (working_set.empty ())
			return nullptr;
		if (working_set [0].type != token_type::symbol)
			return nullptr;
		if (working_set [0].op != sym)
			return nullptr;

		return pop_front (working_set);
	}

	AST_node get_token (token_range& working_set,
	                    string keyword)
	{
		if (working_set.empty ())
			return nullptr;
		if (working_set [0].type != token_type::keyword)
			return nullptr;
		if (working_set [0].str != keyword)
			return nullptr;

		return pop_front (working_set);
	}

	// typename token_range::iterator
	// find_close (token_range r, symbol open, symbol close)
	// {
	// 	for (int i = 1; r; r.drop_front (1))
	// 	{
	// 		if (t.type == token_type::symbol)
	// 			if (t.op == open)
	// 				++i;
	// 			else if (t.op == close)
	// 				--i;
	// 		if (i == 0)
	// 			break;
	// 	}
	// 	return begin (r);
	// }

	void validate_pairs (token_range tokens)
	{
		stack <const token*> open_tokens;

		while (tokens)
		{
			if (tokens [0].type == token_type::symbol)
				switch (tokens [0].op)
				{
				case symbol::lparen:
					open_tokens.push (&tokens [0]);
					break;
				case symbol::lbrace:
					open_tokens.push (&tokens [0]);
					break;
				case symbol::lbracket:
					open_tokens.push (&tokens [0]);
					break;

				case symbol::rparen:
					if (open_tokens.empty ())
						throw_error ("Unmatched ')'",
						             tokens [0].pos);
					if (open_tokens.top ()->op != symbol::lparen)
						throw_error ("Unmatched '" + to_string (open_tokens.top ()->op) + "'",
						             open_tokens.top ()->pos);
					open_tokens.pop ();
					break;
				case symbol::rbrace:
					if (open_tokens.empty ())
						throw_error ("Unmatched '}'",
						             tokens [0].pos);
					if (open_tokens.top ()->op != symbol::lbrace)
						throw_error ("Unmatched '" + to_string (open_tokens.top ()->op) + "'",
						             open_tokens.top ()->pos);
					open_tokens.pop ();
					break;
				case symbol::rbracket:
					if (open_tokens.empty ())
						throw_error ("Unmatched ']'",
						             tokens [0].pos);
					if (open_tokens.top ()->op != symbol::lbracket)
						throw_error ("Unmatched '" + to_string (open_tokens.top ()->op) + "'",
						             open_tokens.top ()->pos);
					open_tokens.pop ();
					break;
				}

			tokens.drop_front (1);
		}

		if (!open_tokens.empty ())
			throw_error ("Unmatched '" + to_string (open_tokens.top ()->op) + "'",
			             open_tokens.top ()->pos);
	}
}



AST program_p (const vector <token>& tokens)
{
	token_range working_set = tokens;

	validate_pairs (working_set);
	logfile << "Finished validating\n";
	AST_node declList = CALL (declList_p (working_set));
	if (!declList)
		throw_error ("Expected a declaration",
		             begin (working_set)->pos);

	return AST (move (*declList));
}



AST_node declList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node decl_list = make_node (AST_type::declList);

	AST_node next_decl = CALL (decl_p (working_set));
	validate (next_decl);
	decl_list->children.push_back (next_decl);

	for (;;)
	{
		next_decl = CALL (decl_p (working_set));
		if (next_decl)
			decl_list->children.push_back (next_decl);
		else
		{
			tokens = working_set;
			return decl_list;
		}
	}
}



AST_node decl_p (token_range& tokens)
{
	AST_node sub_decl = CALL (varDecl_p (tokens));
	if (!sub_decl)
		sub_decl = CALL (funDecl_p (tokens));
	validate (sub_decl);

	return make_node (AST_type::decl, sub_decl);
}



AST_node varDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node type_specifier = CALL (typeSpecifier_p (working_set));
	validate (type_specifier);

	AST_node ID = get_token (working_set, token_type::identifier);
	validate (ID);

	AST_node lbracket_node = get_token (working_set, symbol::lbracket);
	AST_node num;
	AST_node rbracket_node;
	if (lbracket_node)
	{
		num = get_token (working_set, token_type::int_literal);
		if (!num)
			throw_error ("Expected numeric constant",
			             begin (working_set)->pos);

		rbracket_node = get_token (working_set, symbol::rbracket);
		if (!rbracket_node)
			throw_error ("Expected ']'",
			             begin (working_set)->pos);
	}

	AST_node semicolon_node = get_token (working_set, symbol::semicolon);
	validate (semicolon_node);

	tokens = working_set;
	if (lbracket_node)
		return make_node (AST_type::varDecl, type_specifier, ID,
		                  lbracket_node, num, rbracket_node, semicolon_node);
	return make_node (AST_type::varDecl, type_specifier, ID, semicolon_node);
}



AST_node typeSpecifier_p (token_range& tokens)
{
	AST_node specifier = get_token (tokens, "int");
	if (specifier)
		return make_node (AST_type::typeSpecifier, specifier);

	specifier = get_token (tokens, "char");
	if (specifier)
		return make_node (AST_type::typeSpecifier, specifier);

	specifier = get_token (tokens, "void");
	if (specifier)
		return make_node (AST_type::typeSpecifier, specifier);

	return nullptr;
}



AST_node funDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node type_specifier = CALL (typeSpecifier_p (working_set));
	validate (type_specifier);

	AST_node ID = get_token (working_set, token_type::identifier);
	validate (ID);

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	validate (lparen_node);

	AST_node decl_list = CALL (formalDeclList_p (working_set));

	AST_node rparen_node = get_token (working_set, symbol::rparen);
	if (!rparen_node)
		throw_error ("Expected ')'",
		             begin (working_set)->pos);

	AST_node function_body = CALL (funBody_p (working_set));
	validate (function_body);

	tokens = working_set;
	if (decl_list)
		return make_node (AST_type::funDecl, type_specifier, ID,
		                  lparen_node, decl_list, rparen_node, function_body);
		return make_node (AST_type::funDecl, type_specifier, ID,
		                  lparen_node, rparen_node, function_body);
}



AST_node formalDeclList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node decl_list = make_node (AST_type::formalDeclList);

	AST_node next_decl = CALL (formalDecl_p (working_set));
	validate (next_decl);
	decl_list->children.push_back (next_decl);

	AST_node comma_node;
	for (;;)
	{
		comma_node = get_token (working_set, symbol::comma);
		if (!comma_node)
			break;
		next_decl = CALL (formalDecl_p (working_set));
		if (!next_decl)
			break;

		decl_list->children.push_back (comma_node);
		decl_list->children.push_back (next_decl);
	}

	tokens = working_set;
	return decl_list;
}



AST_node formalDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node type_specifier = CALL (typeSpecifier_p (working_set));
	validate (type_specifier);

	AST_node ID = get_token (working_set, token_type::identifier);
	validate (ID);

	AST_node lbracket_node = get_token (working_set, symbol::lbracket);
	AST_node rbracket_node;
	if (lbracket_node)
	{
		rbracket_node = get_token (working_set, symbol::rbracket);
		if (!rbracket_node)
			throw_error ("Expected ']'",
			             begin (working_set)->pos);
	}

	tokens = working_set;
	if (lbracket_node)
		return make_node (AST_type::formalDecl, type_specifier, ID,
		                  lbracket_node, rbracket_node);
	return make_node (AST_type::formalDecl, type_specifier, ID);
}



AST_node funBody_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lbrace_node = get_token (working_set, symbol::lbrace);
	validate (lbrace_node);

	AST_node decl_list = CALL (declList_p (working_set));

	AST_node statement_list = CALL (statementList_p (working_set));
	validate (statement_list);

	AST_node rbrace_node = get_token (working_set, symbol::rbrace);
	if (!rbrace_node)
		throw_error ("Expected '}'",
		             begin (working_set)->pos);

	tokens = working_set;
	if (decl_list)
		return make_node (AST_type::funBody, lbrace_node, decl_list,
		                  statement_list, rbrace_node);
	return make_node (AST_type::funBody, lbrace_node,
	                  statement_list, rbrace_node);
}



AST_node localDeclList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node decl_list = make_node (AST_type::statementList);

	AST_node next_decl;
	for (;;)
	{
		next_decl = CALL (localDeclList_p (working_set));
		if (next_decl)
			decl_list->children.push_back (next_decl);
		else
		{
			tokens = working_set;
			return decl_list;
		}
	}
}



AST_node statementList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node decl_list = make_node (AST_type::statementList);

	AST_node next_decl;
	for (;;)
	{
		next_decl = CALL (statement_p (working_set));
		if (next_decl)
			decl_list->children.push_back (next_decl);
		else
		{
			tokens = working_set;
			return decl_list;
		}
	}
}



AST_node statement_p (token_range& tokens)
{
	AST_node sub_decl = CALL (compoundStmt_p (tokens));
	if (sub_decl)
		return make_node (AST_type::statement, sub_decl);

	sub_decl = CALL (assignStmt_p (tokens));
	if (sub_decl)
		return make_node (AST_type::statement, sub_decl);

	sub_decl = CALL (condStmt_p (tokens));
	if (sub_decl)
		return make_node (AST_type::statement, sub_decl);

	sub_decl = CALL (loopStmt_p (tokens));
	if (sub_decl)
		return make_node (AST_type::statement, sub_decl);

	sub_decl = CALL (returnStmt_p (tokens));
	if (sub_decl)
		return make_node (AST_type::statement, sub_decl);

	return nullptr;
}



AST_node compoundStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lbrace_node = get_token (working_set, symbol::lbrace);
	validate (lbrace_node);

	AST_node statement_list = CALL (statementList_p (working_set));
	validate (statement_list);

	AST_node rbrace_node = get_token (working_set, symbol::rbrace);
	if (!rbrace_node)
		throw_error ("Expected '}'",
		             begin (working_set)->pos);

	tokens = working_set;
	return make_node (AST_type::compoundStmt, lbrace_node,
	                  statement_list, rbrace_node);
}



AST_node assignStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node var = CALL (var_p (working_set));
	AST_node eq_node;
	if (var)
	{
		eq_node = get_token (working_set, symbol::equal);
		if (!eq_node)
		{
			var = nullptr;
			working_set = tokens;
		}
	}

	AST_node expression = CALL (expression_p (working_set));
	validate (expression);

	AST_node semicolon_node = get_token (working_set, symbol::semicolon);
	if (!semicolon_node)
		throw_error ("Expected ';'",
		             begin (working_set)->pos);

	tokens = working_set;
	if (var)
		return make_node (AST_type::assignStmt, var, eq_node,
		                  expression, semicolon_node);
	return make_node (AST_type::assignStmt, expression, semicolon_node);
}



AST_node condStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node if_node = get_token (working_set, "if");
	validate (if_node);

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	validate (lparen_node);

	AST_node expression = CALL (expression_p (working_set));
	if (!expression)
		throw_error ("Expected expression",
		             begin (working_set)->pos);

	AST_node rparen_node = get_token (working_set, symbol::rparen);
	if (!rparen_node)
		throw_error ("Expected ')'",
		             begin (working_set)->pos);

	AST_node if_statement = CALL (statement_p (working_set));
	if (!if_statement)
		throw_error ("Expected statement",
		             begin (working_set)->pos);

	AST_node else_node = get_token (working_set, "else");
	AST_node else_statement;
	if (else_node)
	{
		else_statement = CALL (statement_p (working_set));
		if (!else_statement)
			throw_error ("Expected statement",
			             begin (working_set)->pos);
	}

	tokens = working_set;
	if (else_node)
		return make_node (AST_type::condStmt, if_node, lparen_node, expression,
		                  rparen_node, if_statement, else_node, else_statement);
		return make_node (AST_type::condStmt, if_node, lparen_node,
		                  expression, rparen_node, if_statement);
}



AST_node loopStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node while_node = get_token (working_set, "while");
	validate (while_node);

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	validate (lparen_node);

	AST_node expression = CALL (expression_p (working_set));
	if (!expression)
		throw_error ("Expected expression",
		             begin (working_set)->pos);

	AST_node rparen_node = get_token (working_set, symbol::rparen);
	if (!rparen_node)
		throw_error ("Expected ')'",
		             begin (working_set)->pos);

	AST_node while_statement = CALL (statement_p (working_set));
	if (!while_statement)
		throw_error ("Expected statement",
		             begin (working_set)->pos);

	tokens = working_set;
	return make_node (AST_type::loopStmt, while_node, lparen_node,
	                  expression, rparen_node, while_statement);
}



AST_node returnStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node return_node = get_token (working_set, "return");
	validate (return_node);

	AST_node expression = CALL (expression_p (working_set));

	AST_node semicolon_node = get_token (working_set, symbol::semicolon);
	if (!semicolon_node)
		throw_error ("Expected ';'",
		             begin (working_set)->pos);

	tokens = working_set;
	if (expression)
		return make_node (AST_type::returnStmt, return_node,
		                  expression, semicolon_node);
	return make_node (AST_type::returnStmt, return_node, semicolon_node);
}



AST_node var_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node ID = get_token (working_set, token_type::identifier);
	validate (ID);

	AST_node lbracket_node = get_token (working_set, symbol::lbracket);
	AST_node add_expr;
	AST_node rbracket_node;
	if (lbracket_node)
	{
		add_expr = get_token (working_set, token_type::int_literal);
		if (!add_expr)
			throw_error ("Expected expression",
			             begin (working_set)->pos);

		rbracket_node = get_token (working_set, symbol::rbracket);
		if (!rbracket_node)
			throw_error ("Expected ']'",
			             begin (working_set)->pos);
	}

	tokens = working_set;
	if (lbracket_node)
		return make_node (AST_type::var, ID, lbracket_node,
		                  add_expr, rbracket_node);
	return make_node (AST_type::var, ID);
}



AST_node expression_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node handle = CALL (addExpr_p (working_set));
	validate (handle);

	AST_node relop;
	AST_node add_expr;
	for (;;)
	{
		token_range sub_working_set = working_set;

		relop = CALL (relop_p (sub_working_set));
		if (!relop)
			break;

		add_expr = CALL (addExpr_p (sub_working_set));
		if (!add_expr)
			break;

		working_set = sub_working_set;
		handle = make_node (AST_type::expression, handle,
		                    relop, add_expr);
	}

	tokens = working_set;
	return handle;
}



AST_node relop_p (token_range& tokens)
{
	AST_node relop = get_token (tokens, symbol::lt_equiv);
	if (relop)
		return make_node (AST_type::relop, relop);

	relop = get_token (tokens, symbol::less_than);
	if (relop)
		return make_node (AST_type::relop, relop);

	relop = get_token (tokens, symbol::greater_than);
	if (relop)
		return make_node (AST_type::relop, relop);

	relop = get_token (tokens, symbol::gt_equiv);
	if (relop)
		return make_node (AST_type::relop, relop);

	relop = get_token (tokens, symbol::equivalent);
	if (relop)
		return make_node (AST_type::relop, relop);

	relop = get_token (tokens, symbol::not_equiv);
	if (relop)
		return make_node (AST_type::relop, relop);

	return nullptr;
}



AST_node addExpr_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node handle = CALL (term_p (working_set));
	validate (handle);

	AST_node addop;
	AST_node term;
	for (;;)
	{
		token_range sub_working_set = working_set;

		addop = CALL (addop_p (sub_working_set));
		if (!addop)
			break;

		term = CALL (term_p (sub_working_set));
		if (!term)
			break;

		working_set = sub_working_set;
		handle = make_node (AST_type::addExpr, handle,
		                    addop, term);
	}

	tokens = working_set;
	return handle;
}



AST_node addop_p (token_range& tokens)
{
	AST_node addop = get_token (tokens, symbol::plus);
	if (addop)
		return make_node (AST_type::addop, addop);

	addop = get_token (tokens, symbol::minus);
	if (addop)
		return make_node (AST_type::addop, addop);

	return nullptr;
}



AST_node term_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node handle = CALL (factor_p (working_set));
	validate (handle);

	AST_node mulop;
	AST_node factor;
	for (;;)
	{
		token_range sub_working_set = working_set;

		mulop = CALL (mulop_p (sub_working_set));
		if (!mulop)
			break;

		factor = CALL (factor_p (sub_working_set));
		if (!factor)
			break;

		working_set = sub_working_set;
		handle = make_node (AST_type::term, handle,
		                    mulop, factor);
	}

	tokens = working_set;
	return handle;
}



AST_node mulop_p (token_range& tokens)
{
	AST_node addop = get_token (tokens, symbol::asterisk);
	if (addop)
		return make_node (AST_type::mulop, addop);

	addop = get_token (tokens, symbol::solidus);
	if (addop)
		return make_node (AST_type::mulop, addop);

	return nullptr;
}



AST_node factor_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	if (lparen_node)
	{
		AST_node expression = CALL (expression_p (working_set));
		if (!expression)
			throw_error ("Expected expression",
			             begin (working_set)->pos);

		AST_node rparen_node = get_token (working_set, symbol::rparen);
		if (!rparen_node)
			throw_error ("Expected ')'",
			             begin (working_set)->pos);

		tokens = working_set;
		return make_node (AST_type::factor, lparen_node,
		                  expression, rparen_node);
	}

	AST_node rvalue = CALL (funcCallExpr_p (tokens));
	if (rvalue)
		return make_node (AST_type::factor, rvalue);

	rvalue = CALL (var_p (tokens));
	if (rvalue)
		return make_node (AST_type::factor, rvalue);

	rvalue = get_token (tokens, token_type::int_literal);
	if (rvalue)
		return make_node (AST_type::factor, rvalue);

	rvalue = get_token (tokens, token_type::char_literal);
	if (rvalue)
		return make_node (AST_type::factor, rvalue);

	rvalue = get_token (tokens, token_type::string_literal);
	if (rvalue)
		return make_node (AST_type::factor, rvalue);

	return nullptr;
}



AST_node funcCallExpr_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node ID = get_token (working_set, token_type::identifier);
	validate (ID);

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	validate (lparen_node);

	AST_node arg_list = CALL (argList_p (working_set));
	AST_node rparen_node = get_token (working_set, symbol::rparen);
	if (!rparen_node)
		throw_error ("Expected ')'",
		             begin (working_set)->pos);

	tokens = working_set;
	if (arg_list)
		return make_node (AST_type::funcCallExpr, ID, lparen_node,
		                  arg_list, rparen_node);
		return make_node (AST_type::funcCallExpr, ID,
		                  lparen_node, rparen_node);
}



AST_node argList_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node expression_list = make_node (AST_type::argList);

	AST_node next_expression = CALL (expression_p (working_set));
	validate (next_expression);
	expression_list->children.push_back (next_expression);

	AST_node comma_node;
	for (;;)
	{
		comma_node = get_token (working_set, symbol::comma);
		if (!comma_node)
			break;
		next_expression = CALL (expression_p (working_set));
		if (!next_expression)
			break;

		expression_list->children.push_back (comma_node);
		expression_list->children.push_back (next_expression);
	}

	tokens = working_set;
	return expression_list;
}
