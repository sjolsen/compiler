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

	string log_string (const AST_node& node)
	{
		if (!node)
			return "";
		return to_string (*node, string (ident_level, '\t'));
	}

	string log_string (bool b)
	{ return b ? string (ident_level, '\t') + "[]\n" : ""; }

	string log_string (const token& t)
	{ return string (ident_level, '\t') + to_string (t) + '\n'; }

	void printargs ()
	{
	}

	template <typename T, typename... Rest>
	void printargs (const T& first,
	                Rest&&... rest)
	{
		logfile << log_string (first);
		printargs (rest...);
	}
}



namespace
{
	template <typename NodeType, typename... Args>
	Node <NodeType> make_node (Args&&... args)
	{
		logfile << string (ident_level, '\t')
		        << "Making node:\n";
		++ident_level;
		printargs (forward <Args> (args)...);

		auto node = make_shared <NodeType> (forward <Args> (args)...);

		--ident_level;
		logfile << string (ident_level, '\t')
		        << "Done\n";

		return node;
	}

	Node <terminal> pop_front (token_range& working_set)
	{
		Node <terminal> front = make_node <terminal> (working_set [0]);
		working_set.drop_front (1);
		return front;
	}

	Node <terminal> get_token (token_range& working_set,
	                           token_type type)
	{
		if (working_set.empty ())
			return nullptr;
		if (working_set [0].type != type)
			return nullptr;

		return pop_front (working_set);
	}

	Node <terminal> get_token (token_range& working_set,
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

	Node <terminal> get_token (token_range& working_set,
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



program program_p (const vector <token>& tokens)
{
	token_range working_set = tokens;

	validate_pairs (working_set);
	logfile << "Finished validating\n";

	Node <declList> decl_list = CALL (declList_p (working_set));
	if (!decl_list)
		throw_error ("Expected a declaration",
		             begin (working_set)->pos);

	return program (decl_list);
}



Node <declList> declList_p (token_range& tokens)
{
	token_range working_set = tokens;

	auto decl_list = make_node <declList> ();

	Node <decl> next_decl = CALL (decl_p (working_set));
	validate (next_decl);
	decl_list->decls.push_back (next_decl);

	for (;;)
	{
		next_decl = CALL (decl_p (working_set));
		if (next_decl)
			decl_list->decls.push_back (next_decl);
		else
		{
			tokens = working_set;
			return decl_list;
		}
	}
}



Node <decl> decl_p (token_range& tokens)
{
	AST_node sub_decl = CALL (varDecl_p (tokens));
	if (!sub_decl)
		sub_decl = CALL (funDecl_p (tokens));
	validate (sub_decl);

	return make_node <decl> (sub_decl);
}



Node <varDecl> varDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <typeSpecifier> type_specifier = CALL (typeSpecifier_p (working_set));
	validate (type_specifier);

	Node <terminal> ID = get_token (working_set, token_type::identifier);
	validate (ID);

	AST_node lbracket_node = get_token (working_set, symbol::lbracket);
	Node <terminal> num;
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
	return make_node <varDecl> (type_specifier, ID, num);
}



Node <typeSpecifier> typeSpecifier_p (token_range& tokens)
{
	Node <terminal> specifier = get_token (tokens, "int");
	if (specifier)
		return make_node <typeSpecifier> (specifier);

	specifier = get_token (tokens, "char");
	if (specifier)
		return make_node <typeSpecifier> (specifier);

	specifier = get_token (tokens, "void");
	if (specifier)
		return make_node <typeSpecifier> (specifier);

	return nullptr;
}



Node <funDecl> funDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <typeSpecifier> type_specifier = CALL (typeSpecifier_p (working_set));
	validate (type_specifier);

	Node <terminal> ID = get_token (working_set, token_type::identifier);
	validate (ID);

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	validate (lparen_node);

	Node <formalDeclList> decl_list = CALL (formalDeclList_p (working_set));

	AST_node rparen_node = get_token (working_set, symbol::rparen);
	if (!rparen_node)
		throw_error ("Expected ')'",
		             begin (working_set)->pos);

	Node <funBody> function_body = CALL (funBody_p (working_set));
	validate (function_body);

	tokens = working_set;
	return make_node <funDecl> (type_specifier, ID, decl_list, function_body);
}



Node <formalDeclList> formalDeclList_p (token_range& tokens)
{
	token_range working_set = tokens;

	auto decl_list = make_node <formalDeclList> ();

	Node <formalDecl> next_decl = CALL (formalDecl_p (working_set));
	validate (next_decl);
	decl_list->decls.push_back (next_decl);

	AST_node comma_node;
	for (;;)
	{
		comma_node = get_token (working_set, symbol::comma);
		if (!comma_node)
			break;
		next_decl = CALL (formalDecl_p (working_set));
		if (!next_decl)
			break;

		decl_list->decls.push_back (next_decl);
	}

	tokens = working_set;
	return decl_list;
}



Node <formalDecl> formalDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <typeSpecifier> type_specifier = CALL (typeSpecifier_p (working_set));
	validate (type_specifier);

	Node <terminal> ID = get_token (working_set, token_type::identifier);
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
	return make_node <formalDecl> (type_specifier, ID, (bool) lbracket_node);
}



Node <funBody> funBody_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lbrace_node = get_token (working_set, symbol::lbrace);
	validate (lbrace_node);

	Node <localDeclList> decl_list = CALL (localDeclList_p (working_set));
	validate (decl_list);

	Node <statementList> statement_list = CALL (statementList_p (working_set));
	validate (statement_list);

	AST_node rbrace_node = get_token (working_set, symbol::rbrace);
	if (!rbrace_node)
		throw_error ("Expected '}', declaration, or statement",
		             begin (working_set)->pos);

	tokens = working_set;
	return make_node <funBody> (decl_list,
	                            statement_list);
}



Node <localDeclList> localDeclList_p (token_range& tokens)
{
	token_range working_set = tokens;

	auto decl_list = make_node <localDeclList> ();

	Node <varDecl> next_decl;
	for (;;)
	{
		next_decl = CALL (varDecl_p (working_set));
		if (next_decl)
			decl_list->decls.push_back (next_decl);
		else
		{
			tokens = working_set;
			return decl_list;
		}
	}
}



Node <statementList> statementList_p (token_range& tokens)
{
	token_range working_set = tokens;

	auto decl_list = make_node <statementList> ();

	Node <statement> next_decl;
	for (;;)
	{
		next_decl = CALL (statement_p (working_set));
		if (next_decl)
			decl_list->stmts.push_back (next_decl);
		else
		{
			tokens = working_set;
			return decl_list;
		}
	}
}



Node <statement> statement_p (token_range& tokens)
{
	AST_node sub_decl = CALL (compoundStmt_p (tokens));
	if (sub_decl)
		return make_node <statement> (sub_decl);

	sub_decl = CALL (assignStmt_p (tokens));
	if (sub_decl)
		return make_node <statement> (sub_decl);

	sub_decl = CALL (condStmt_p (tokens));
	if (sub_decl)
		return make_node <statement> (sub_decl);

	sub_decl = CALL (loopStmt_p (tokens));
	if (sub_decl)
		return make_node <statement> (sub_decl);

	sub_decl = CALL (returnStmt_p (tokens));
	if (sub_decl)
		return make_node <statement> (sub_decl);

	return nullptr;
}



Node <compoundStmt> compoundStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lbrace_node = get_token (working_set, symbol::lbrace);
	validate (lbrace_node);

	Node <statementList> statement_list = CALL (statementList_p (working_set));
	validate (statement_list);

	AST_node rbrace_node = get_token (working_set, symbol::rbrace);
	if (!rbrace_node)
		throw_error ("Expected '}' or statement",
		             begin (working_set)->pos);

	tokens = working_set;
	return make_node <compoundStmt> (statement_list);
}



Node <assignStmt> assignStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <var> lvalue = CALL (var_p (working_set));
	AST_node eq_node;
	if (lvalue)
	{
		eq_node = get_token (working_set, symbol::equal);
		if (!eq_node)
		{
			lvalue = nullptr;
			working_set = tokens;
		}
	}

	Node <expression> rvalue = CALL (expression_p (working_set));
	validate (rvalue);

	AST_node semicolon_node = get_token (working_set, symbol::semicolon);
	if (!semicolon_node)
		return nullptr;

	tokens = working_set;
	return make_node <assignStmt> (lvalue, rvalue);
}



Node <condStmt> condStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node if_node = get_token (working_set, "if");
	validate (if_node);

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	validate (lparen_node);

	Node <expression> expression = CALL (expression_p (working_set));
	if (!expression)
		throw_error ("Expected expression",
		             begin (working_set)->pos);

	AST_node rparen_node = get_token (working_set, symbol::rparen);
	if (!rparen_node)
		throw_error ("Expected ')'",
		             begin (working_set)->pos);

	Node <statement> if_statement = CALL (statement_p (working_set));
	if (!if_statement)
		throw_error ("Expected statement",
		             begin (working_set)->pos);

	AST_node else_node = get_token (working_set, "else");
	Node <statement> else_statement;
	if (else_node)
	{
		else_statement = CALL (statement_p (working_set));
		if (!else_statement)
			throw_error ("Expected statement",
			             begin (working_set)->pos);
	}

	tokens = working_set;
	return make_node <condStmt> (expression, if_statement, else_statement);
}



Node <loopStmt> loopStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node while_node = get_token (working_set, "while");
	validate (while_node);

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	validate (lparen_node);

	Node <expression> expression = CALL (expression_p (working_set));
	if (!expression)
		throw_error ("Expected expression",
		             begin (working_set)->pos);

	AST_node rparen_node = get_token (working_set, symbol::rparen);
	if (!rparen_node)
		throw_error ("Expected ')'",
		             begin (working_set)->pos);

	Node <statement> while_statement = CALL (statement_p (working_set));
	if (!while_statement)
		throw_error ("Expected statement",
		             begin (working_set)->pos);

	tokens = working_set;
	return make_node <loopStmt> (expression, while_statement);
}



Node <returnStmt> returnStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node return_node = get_token (working_set, "return");
	validate (return_node);

	Node <expression> expression = CALL (expression_p (working_set));

	AST_node semicolon_node = get_token (working_set, symbol::semicolon);
	if (!semicolon_node)
		throw_error ("Expected ';'",
		             begin (working_set)->pos);

	tokens = working_set;
	return make_node <returnStmt> (expression);
}



Node <var> var_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <terminal> ID = get_token (working_set, token_type::identifier);
	validate (ID);

	AST_node lbracket_node = get_token (working_set, symbol::lbracket);
	Node <addExpr> add_expr;
	AST_node rbracket_node;
	if (lbracket_node)
	{
		add_expr = CALL (addExpr_p (working_set));
		if (!add_expr)
			throw_error ("Expected expression",
			             begin (working_set)->pos);

		rbracket_node = get_token (working_set, symbol::rbracket);
		if (!rbracket_node)
			throw_error ("Expected ']'",
			             begin (working_set)->pos);
	}

	tokens = working_set;
	return make_node <var> (ID, add_expr);
}



Node <expression> expression_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <addExpr> add_expr = CALL (addExpr_p (working_set));
	validate (add_expr);

	auto handle = make_node <expression> (nullptr, nullptr, add_expr);

	Node <relop> relop;
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
		handle = make_node <expression> (handle, relop, add_expr);
	}

	tokens = working_set;
	return handle;
}



Node <relop> relop_p (token_range& tokens)
{
	Node <terminal> op_node = get_token (tokens, symbol::lt_equiv);
	if (op_node)
		return make_node <relop> (op_node);

	op_node = get_token (tokens, symbol::less_than);
	if (op_node)
		return make_node <relop> (op_node);

	op_node = get_token (tokens, symbol::greater_than);
	if (op_node)
		return make_node <relop> (op_node);

	op_node = get_token (tokens, symbol::gt_equiv);
	if (op_node)
		return make_node <relop> (op_node);

	op_node = get_token (tokens, symbol::equivalent);
	if (op_node)
		return make_node <relop> (op_node);

	op_node = get_token (tokens, symbol::not_equiv);
	if (op_node)
		return make_node <relop> (op_node);

	return nullptr;
}



Node <addExpr> addExpr_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <term> term = CALL (term_p (working_set));
	validate (term);

	auto handle = make_node <addExpr> (nullptr, nullptr,
	                                   term);

	Node <addop> addop;
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
		handle = make_node <addExpr> (handle, addop, term);
	}

	tokens = working_set;
	return handle;
}



Node <addop> addop_p (token_range& tokens)
{
	Node <terminal> op_node = get_token (tokens, symbol::plus);
	if (op_node)
		return make_node <addop> (op_node);

	op_node = get_token (tokens, symbol::minus);
	if (op_node)
		return make_node <addop> (op_node);

	return nullptr;
}



Node <term> term_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <factor> factor = CALL (factor_p (working_set));
	validate (factor);

	auto handle = make_node <term> (nullptr, nullptr, factor);

	Node <mulop> mulop;
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
		handle = make_node <term> (handle, mulop, factor);
	}

	tokens = working_set;
	return handle;
}



Node <mulop> mulop_p (token_range& tokens)
{
	Node <terminal> op_node = get_token (tokens, symbol::asterisk);
	if (op_node)
		return make_node <mulop> (op_node);

	op_node = get_token (tokens, symbol::solidus);
	if (op_node)
		return make_node <mulop> (op_node);

	return nullptr;
}



Node <factor> factor_p (token_range& tokens)
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
		return make_node <factor> (expression);
	}

	AST_node rvalue = CALL (funcCallExpr_p (tokens));
	if (rvalue)
		return make_node <factor> (rvalue);

	rvalue = CALL (var_p (tokens));
	if (rvalue)
		return make_node <factor> (rvalue);

	rvalue = get_token (tokens, token_type::int_literal);
	if (rvalue)
		return make_node <factor> (rvalue);

	rvalue = get_token (tokens, token_type::char_literal);
	if (rvalue)
		return make_node <factor> (rvalue);

	rvalue = get_token (tokens, token_type::string_literal);
	if (rvalue)
		return make_node <factor> (rvalue);

	return nullptr;
}



Node <funcCallExpr> funcCallExpr_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <terminal> ID = get_token (working_set, token_type::identifier);
	validate (ID);

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	validate (lparen_node);

	Node <argList> arg_list = CALL (argList_p (working_set));
	AST_node rparen_node = get_token (working_set, symbol::rparen);
	if (!rparen_node)
		throw_error ("Expected ')'",
		             begin (working_set)->pos);

	tokens = working_set;
	return make_node <funcCallExpr> (ID, arg_list);
}



Node <argList> argList_p (token_range& tokens)
{
	token_range working_set = tokens;

	auto expression_list = make_node <argList> ();

	Node <expression> next_expression = CALL (expression_p (working_set));
	validate (next_expression);
	expression_list->args.push_back (next_expression);

	AST_node comma_node;
	for (;;)
	{
		comma_node = get_token (working_set, symbol::comma);
		if (!comma_node)
			break;
		next_expression = CALL (expression_p (working_set));
		if (!next_expression)
			break;

		expression_list->args.push_back (next_expression);
	}

	tokens = working_set;
	return expression_list;
}
