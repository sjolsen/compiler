#include <ast/ast_predicates.hh>
#include <ast/symbol_table.hh>

#include <algorithm>
#include <stack>
#include <chrono>

using namespace std;



#define validate(node) if (!node) return nullptr;



namespace
{
	template <typename AstType, typename... Args>
	Node <AstType> make_node (Args&&... args)
	{
		return Node <AstType> (new AstType (forward <Args> (args)...));
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
						throw error ("Unmatched ')'",
						             tokens [0].pos);
					if (open_tokens.top ()->op != symbol::lparen)
						throw error ("Unmatched '" + to_string (open_tokens.top ()->op) + "'",
						             open_tokens.top ()->pos);
					open_tokens.pop ();
					break;
				case symbol::rbrace:
					if (open_tokens.empty ())
						throw error ("Unmatched '}'",
						             tokens [0].pos);
					if (open_tokens.top ()->op != symbol::lbrace)
						throw error ("Unmatched '" + to_string (open_tokens.top ()->op) + "'",
						             open_tokens.top ()->pos);
					open_tokens.pop ();
					break;
				case symbol::rbracket:
					if (open_tokens.empty ())
						throw error ("Unmatched ']'",
						             tokens [0].pos);
					if (open_tokens.top ()->op != symbol::lbracket)
						throw error ("Unmatched '" + to_string (open_tokens.top ()->op) + "'",
						             open_tokens.top ()->pos);
					open_tokens.pop ();
					break;
				default:
					break;
				}

			tokens.drop_front (1);
		}

		if (!open_tokens.empty ())
			throw error ("Unmatched '" + to_string (open_tokens.top ()->op) + "'",
			             open_tokens.top ()->pos);
	}
}



program program_p (const vector <token>& tokens)
{
	token_range working_set = tokens;
	if (working_set.empty ())
		throw error ("Expected a declaration",
		             file_position ());

	validate_pairs (working_set);

	Node <declList> decl_list = declList_p (working_set);
	if (!decl_list || !working_set.empty ())
		throw error ("Expected a declaration",
		             begin (working_set)->pos);

	auto prog = program (move (decl_list));
	prog.parent = nullptr;
	populate_table (*prog.table, *prog.decl_list);

	return prog;
}



Node <declList> declList_p (token_range& tokens)
{
	token_range working_set = tokens;

	vector <Node <decl>> decl_list;

	Node <decl> next_decl = decl_p (working_set);
	validate (next_decl);
	decl_list.push_back (move (next_decl));

	for (;;)
	{
		next_decl = decl_p (working_set);
		if (next_decl)
			decl_list.push_back (move (next_decl));
		else
		{
			tokens = working_set;
			return make_node <declList> (move (decl_list));
		}
	}
}



Node <decl> decl_p (token_range& tokens)
{
	Node <basic_decl> sub_decl = varDecl_p (tokens);
	if (!sub_decl)
		sub_decl = funDecl_p (tokens);
	validate (sub_decl);

	return make_node <decl> (move (sub_decl));
}



Node <varDecl> varDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <typeSpecifier> type_specifier = typeSpecifier_p (working_set);
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
			throw error ("Expected numeric constant",
			             begin (working_set)->pos);
		if (num->token_ref.value < 1)
			throw error ("Arrays must have a size of at least one",
			             begin (working_set)->pos);

		rbracket_node = get_token (working_set, symbol::rbracket);
		if (!rbracket_node)
			throw error ("Expected ']'",
			             begin (working_set)->pos);
	}

	AST_node semicolon_node = get_token (working_set, symbol::semicolon);
	validate (semicolon_node);

	tokens = working_set;
	return make_node <varDecl> (move (type_specifier), move (ID), move (num));
}



Node <typeSpecifier> typeSpecifier_p (token_range& tokens)
{
	Node <terminal> specifier = get_token (tokens, "int");
	if (specifier)
		return make_node <typeSpecifier> (move (specifier));

	specifier = get_token (tokens, "char");
	if (specifier)
		return make_node <typeSpecifier> (move (specifier));

	specifier = get_token (tokens, "void");
	if (specifier)
		return make_node <typeSpecifier> (move (specifier));

	return nullptr;
}



Node <funDecl> funDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <typeSpecifier> type_specifier = typeSpecifier_p (working_set);
	validate (type_specifier);

	Node <terminal> ID = get_token (working_set, token_type::identifier);
	validate (ID);

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	validate (lparen_node);

	Node <formalDeclList> decl_list = formalDeclList_p (working_set);

	AST_node rparen_node = get_token (working_set, symbol::rparen);
	if (!rparen_node)
		throw error ("Expected ')'",
		             begin (working_set)->pos);

	Node <funBody> function_body = funBody_p (working_set);
	if (!function_body)
	{
		AST_node semicolon_node = get_token (working_set, symbol::semicolon);
		validate (semicolon_node);

		tokens = working_set;
		return make_node <funDecl> (move (type_specifier), move (ID), move (decl_list), nullptr);
	}

	tokens = working_set;
	auto func = make_node <funDecl> (move (type_specifier), move (ID), move (decl_list), move (function_body));
	if (func->decl_list)
		populate_table (*func->param_table, *func->decl_list);
	if (func->body->decl_list)
		populate_table (*func->local_table, *func->body->decl_list, *func->param_table);

	return func;
}



Node <formalDeclList> formalDeclList_p (token_range& tokens)
{
	token_range working_set = tokens;

	vector <Node <formalDecl>> decl_list;

	Node <formalDecl> next_decl = formalDecl_p (working_set);
	validate (next_decl);
	decl_list.push_back (move (next_decl));

	AST_node comma_node;
	for (;;)
	{
		comma_node = get_token (working_set, symbol::comma);
		if (!comma_node)
			break;
		next_decl = formalDecl_p (working_set);
		if (!next_decl)
			break;

		decl_list.push_back (move (next_decl));
	}

	if (comma_node)
	{
		Node <terminal> ellipsis_node = get_token (working_set, symbol::ellipsis);
		if (ellipsis_node)
		{
			Node <terminal> ellipsis_name (new terminal (ellipsis_node->token_ref));
			decl_list.push_back (make_node <formalDecl> (make_node <typeSpecifier> (move (ellipsis_node)),
			                                             move (ellipsis_name), false));
		}
	}

	tokens = working_set;
	return make_node <formalDeclList> (move (decl_list));
}



Node <formalDecl> formalDecl_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <typeSpecifier> type_specifier = typeSpecifier_p (working_set);
	validate (type_specifier);

	Node <terminal> ID = get_token (working_set, token_type::identifier);
	validate (ID);

	AST_node lbracket_node = get_token (working_set, symbol::lbracket);
	AST_node rbracket_node;
	if (lbracket_node)
	{
		rbracket_node = get_token (working_set, symbol::rbracket);
		if (!rbracket_node)
			throw error ("Expected ']'",
			             begin (working_set)->pos);
	}

	tokens = working_set;
	return make_node <formalDecl> (move (type_specifier), move (ID), move ((bool) lbracket_node));
}



Node <funBody> funBody_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lbrace_node = get_token (working_set, symbol::lbrace);
	validate (lbrace_node);

	Node <localDeclList> decl_list = localDeclList_p (working_set);
	validate (decl_list);

	Node <statementList> statement_list = statementList_p (working_set);
	validate (statement_list);

	AST_node rbrace_node = get_token (working_set, symbol::rbrace);
	if (!rbrace_node)
	{
		if (statement_list)
			throw error ("Expected '}' or statement",
			             begin (working_set)->pos);
		else
			throw error ("Expected '}', declaration, or statement",
			             begin (working_set)->pos);
	}

	tokens = working_set;
	return make_node <funBody> (move (decl_list), move (statement_list));
}



Node <localDeclList> localDeclList_p (token_range& tokens)
{
	token_range working_set = tokens;

	vector <Node <varDecl>> decl_list;

	Node <varDecl> next_decl;
	for (;;)
	{
		next_decl = varDecl_p (working_set);
		if (next_decl)
			decl_list.push_back (move (next_decl));
		else
		{
			tokens = working_set;
			return make_node <localDeclList> (move (decl_list));
		}
	}
}



Node <statementList> statementList_p (token_range& tokens)
{
	token_range working_set = tokens;

	vector <Node <statement>> decl_list;

	Node <statement> next_decl;
	for (;;)
	{
		next_decl = statement_p (working_set);
		if (next_decl)
			decl_list.push_back (move (next_decl));
		else
		{
			tokens = working_set;
			return make_node <statementList> (move (decl_list));
		}
	}
}



Node <statement> statement_p (token_range& tokens)
{
	AST_node sub_decl = compoundStmt_p (tokens);
	if (sub_decl)
		return make_node <statement> (move (sub_decl));

	sub_decl = assignStmt_p (tokens);
	if (sub_decl)
		return make_node <statement> (move (sub_decl));

	sub_decl = condStmt_p (tokens);
	if (sub_decl)
		return make_node <statement> (move (sub_decl));

	sub_decl = loopStmt_p (tokens);
	if (sub_decl)
		return make_node <statement> (move (sub_decl));

	sub_decl = returnStmt_p (tokens);
	if (sub_decl)
		return make_node <statement> (move (sub_decl));

	return nullptr;
}



Node <compoundStmt> compoundStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lbrace_node = get_token (working_set, symbol::lbrace);
	validate (lbrace_node);

	Node <statementList> statement_list = statementList_p (working_set);
	validate (statement_list);

	AST_node rbrace_node = get_token (working_set, symbol::rbrace);
	if (!rbrace_node)
		throw error ("Expected '}' or statement",
		             begin (working_set)->pos);

	tokens = working_set;
	return make_node <compoundStmt> (move (statement_list));
}



Node <assignStmt> assignStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <var> lvalue = var_p (working_set);
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

	Node <expression> rvalue = expression_p (working_set);
	validate (rvalue);

	AST_node semicolon_node = get_token (working_set, symbol::semicolon);
	if (!semicolon_node)
		return nullptr;

	tokens = working_set;
	return make_node <assignStmt> (move (lvalue), move (rvalue));
}



Node <condStmt> condStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node if_node = get_token (working_set, "if");
	validate (if_node);

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	validate (lparen_node);

	Node <expression> expression = expression_p (working_set);
	if (!expression)
		throw error ("Expected expression",
		             begin (working_set)->pos);

	AST_node rparen_node = get_token (working_set, symbol::rparen);
	if (!rparen_node)
		throw error ("Expected ')'",
		             begin (working_set)->pos);

	Node <statement> if_statement = statement_p (working_set);
	if (!if_statement)
		throw error ("Expected statement",
		             begin (working_set)->pos);

	AST_node else_node = get_token (working_set, "else");
	Node <statement> else_statement;
	if (else_node)
	{
		else_statement = statement_p (working_set);
		if (!else_statement)
			throw error ("Expected statement",
			             begin (working_set)->pos);
	}

	tokens = working_set;
	return make_node <condStmt> (move (expression), move (if_statement), move (else_statement));
}



Node <loopStmt> loopStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node while_node = get_token (working_set, "while");
	validate (while_node);

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	validate (lparen_node);

	Node <expression> expression = expression_p (working_set);
	if (!expression)
		throw error ("Expected expression",
		             begin (working_set)->pos);

	AST_node rparen_node = get_token (working_set, symbol::rparen);
	if (!rparen_node)
		throw error ("Expected ')'",
		             begin (working_set)->pos);

	Node <statement> while_statement = statement_p (working_set);
	if (!while_statement)
		throw error ("Expected statement",
		             begin (working_set)->pos);

	tokens = working_set;
	return make_node <loopStmt> (move (expression), move (while_statement));
}



Node <returnStmt> returnStmt_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <terminal> return_node = get_token (working_set, "return");
	validate (return_node);

	Node <expression> expression = expression_p (working_set);

	AST_node semicolon_node = get_token (working_set, symbol::semicolon);
	if (!semicolon_node)
		throw error ("Expected ';'",
		             begin (working_set)->pos);

	tokens = working_set;
	return make_node <returnStmt> (move (return_node), move (expression));
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
		add_expr = addExpr_p (working_set);
		if (!add_expr)
			throw error ("Expected expression",
			             begin (working_set)->pos);

		rbracket_node = get_token (working_set, symbol::rbracket);
		if (!rbracket_node)
			throw error ("Expected ']'",
			             begin (working_set)->pos);
	}

	tokens = working_set;
	return make_node <var> (move (ID), move (add_expr));
}



Node <expression> expression_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <addExpr> add_expr = addExpr_p (working_set);
	validate (add_expr);

	auto handle = make_node <expression> (nullptr, nullptr, move (add_expr));

	Node <relop> relop;
	for (;;)
	{
		token_range sub_working_set = working_set;

		relop = relop_p (sub_working_set);
		if (!relop)
			break;

		add_expr = addExpr_p (sub_working_set);
		if (!add_expr)
			break;

		working_set = sub_working_set;
		handle = make_node <expression> (move (handle), move (relop), move (add_expr));
	}

	tokens = working_set;
	return handle;
}



Node <relop> relop_p (token_range& tokens)
{
	Node <terminal> op_node = get_token (tokens, symbol::lt_equiv);
	if (op_node)
		return make_node <relop> (move (op_node));

	op_node = get_token (tokens, symbol::less_than);
	if (op_node)
		return make_node <relop> (move (op_node));

	op_node = get_token (tokens, symbol::greater_than);
	if (op_node)
		return make_node <relop> (move (op_node));

	op_node = get_token (tokens, symbol::gt_equiv);
	if (op_node)
		return make_node <relop> (move (op_node));

	op_node = get_token (tokens, symbol::equivalent);
	if (op_node)
		return make_node <relop> (move (op_node));

	op_node = get_token (tokens, symbol::not_equiv);
	if (op_node)
		return make_node <relop> (move (op_node));

	return nullptr;
}



Node <addExpr> addExpr_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <term> term = term_p (working_set);
	validate (term);

	auto handle = make_node <addExpr> (nullptr, nullptr, move (term));

	Node <addop> addop;
	for (;;)
	{
		token_range sub_working_set = working_set;

		addop = addop_p (sub_working_set);
		if (!addop)
			break;

		term = term_p (sub_working_set);
		if (!term)
			break;

		working_set = sub_working_set;
		handle = make_node <addExpr> (move (handle), move (addop), move (term));
	}

	tokens = working_set;
	return handle;
}



Node <addop> addop_p (token_range& tokens)
{
	Node <terminal> op_node = get_token (tokens, symbol::plus);
	if (op_node)
		return make_node <addop> (move (op_node));

	op_node = get_token (tokens, symbol::minus);
	if (op_node)
		return make_node <addop> (move (op_node));

	return nullptr;
}



Node <term> term_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <factor> factor = factor_p (working_set);
	validate (factor);

	auto handle = make_node <term> (nullptr, nullptr, move (factor));

	Node <mulop> mulop;
	for (;;)
	{
		token_range sub_working_set = working_set;

		mulop = mulop_p (sub_working_set);
		if (!mulop)
			break;

		factor = factor_p (sub_working_set);
		if (!factor)
			break;

		working_set = sub_working_set;
		handle = make_node <term> (move (handle), move (mulop), move (factor));
	}

	tokens = working_set;
	return handle;
}



Node <mulop> mulop_p (token_range& tokens)
{
	Node <terminal> op_node = get_token (tokens, symbol::asterisk);
	if (op_node)
		return make_node <mulop> (move (op_node));

	op_node = get_token (tokens, symbol::solidus);
	if (op_node)
		return make_node <mulop> (move (op_node));

	return nullptr;
}



Node <factor> factor_p (token_range& tokens)
{
	token_range working_set = tokens;

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	if (lparen_node)
	{
		AST_node expression = expression_p (working_set);
		if (!expression)
			throw error ("Expected expression",
			             begin (working_set)->pos);

		AST_node rparen_node = get_token (working_set, symbol::rparen);
		if (!rparen_node)
			throw error ("Expected ')'",
			             begin (working_set)->pos);

		tokens = working_set;
		return make_node <factor> (move (expression));
	}

	AST_node rvalue = funcCallExpr_p (tokens);
	if (rvalue)
		return make_node <factor> (move (rvalue));

	rvalue = var_p (tokens);
	if (rvalue)
		return make_node <factor> (move (rvalue));

	rvalue = get_token (tokens, token_type::int_literal);
	if (rvalue)
		return make_node <factor> (move (rvalue));

	rvalue = get_token (tokens, token_type::char_literal);
	if (rvalue)
		return make_node <factor> (move (rvalue));

	rvalue = get_token (tokens, token_type::string_literal);
	if (rvalue)
		return make_node <factor> (move (rvalue));

	return nullptr;
}



Node <funcCallExpr> funcCallExpr_p (token_range& tokens)
{
	token_range working_set = tokens;

	Node <terminal> ID = get_token (working_set, token_type::identifier);
	validate (ID);

	AST_node lparen_node = get_token (working_set, symbol::lparen);
	validate (lparen_node);

	Node <argList> arg_list = argList_p (working_set);
	AST_node rparen_node = get_token (working_set, symbol::rparen);
	if (!rparen_node)
		throw error ("Expected ')'",
		             begin (working_set)->pos);

	tokens = working_set;
	return make_node <funcCallExpr> (move (ID), move (arg_list));
}



Node <argList> argList_p (token_range& tokens)
{
	token_range working_set = tokens;

	vector <Node <expression>> expression_list;

	Node <expression> next_expression = expression_p (working_set);
	if (!next_expression)
		return make_node <argList> (move (expression_list));

	expression_list.push_back (move (next_expression));

	AST_node comma_node;
	for (;;)
	{
		comma_node = get_token (working_set, symbol::comma);
		if (!comma_node)
			break;
		next_expression = expression_p (working_set);
		if (!next_expression)
			break;

		expression_list.push_back (move (next_expression));
	}

	tokens = working_set;
	return make_node <argList> (move (expression_list));
}
