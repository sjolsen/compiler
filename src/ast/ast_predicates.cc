#include <ast/ast_predicates.hh>

using namespace std;



namespace
{
	template <typename... Args>
	AST_node make_node (Args... args)
	{
		return make_shared <AST> (forward <Args> (args)...);
	}

	AST_node pop_front (token_range& tokens)
	{
		AST_node front = make_node (tokens [0]);
		tokens.drop_front (1);
		return front;
	}

	const struct token_constant
		: token
	{
		token_constant (symbol s)
			: type (token_type::symbol),
			  op (s)
		{
		}
	} SEMICOLON (symbol::semicolon),
	  EQUAL     (symbol::equal),
	  LBRACKET  (symbol::lbracket),
	  RBRACKET  (symbol::rbracket);
}



AST program_p (const deque <token>& tokens)
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
	catch (const syntax_error&)
	{
	}

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

	if (working_set.empty () ||
	    working_set [0] != SEMICOLON)
		throw syntax_error ("Expected ';'",
		                    *.pos);
	AST_node scolon_node = pop_front (working_set);

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
	catch (const syntax_error&)
	{
	}

	// Match `varDecl -> intTypeSpec ID`

	AST_node typespec = intTypeSpec_p (working_set);

	if (working_set.empty () ||
	    working_set [0].type != token_type::identifier)
		throw syntax_error ("Expected identifier",
		                    begin (working_set)->pos);
	AST_node ID = pop_front (working_set);

	if (working_set.empty () ||
	    (working_set [0] != EQUAL &&
	     working_set [0] != LBRACKET))
	{
		tokens = working_set;
		return make_node (AST_type::varDecl, typespec, ID);
	}

	// Match `varDecl -> intTypeSpec ID = expression`

	if (working_set [0] == EQUAL)
	{
		AST_node eq_node = pop_front (working_set);
		AST_node initexpr = expression_p (working_set);

		tokens = working_set;
		return make_node (AST_type::varDecl, typespec, ID, eq_node, initexpr);
	}

	// Match `varDecl -> intTypeSpec ID [ ] = initBraceList`

	AST_node lbracket_node = pop_front (working_set);

	if (working_set.empty ())
		throw syntax_error ("Expected ']' or expression"
		                    begin (working_set)->pos);

	if (working_set [0] == RBRACKET)
	{
		AST_node rbracket_node = pop_front (working_set);
		if (working_set.empty () ||
		    working_set [0] != EQUAL)
			throw syntax_error ("Expected '='"
			                    begin (working_set)->pos);
		AST_node eq_node = pop_front (working_set);
		AST_node bracelist = initBraceList_p (working_set);

		tokens = working_set;
		return make_node (AST_type::varDecl, typespec, ID, lbracket_node,
		                  rbracket_node, eq_node, bracelist);
	}

	// Match `varDecl -> intTypeSpec ID [ expression ]`

	AST_node size_expr = expression_p (working_set);

	if (working_set.empty () ||
	    working_set [0] != RBRACKET)
		throw syntax_error ("Expected ']'"
		                    begin (working_set)->pos);
	AST_node rbracket_node = pop_front (working_set);

	if (working_set.empty () ||
	    working_set [0] != EQUAL)
	{
		tokens = working_set;
		return make_node (AST_type::varDecl, typespec, ID,
		                  lbracket_node, size_expr, rbracket_node);
	}

	// Match `varDecl -> intTypeSpec ID [ expression ] = initBraceList`

	AST_node eq_node = pop_front (working_set);
	AST_node bracelist = initBraceList_p (working_set);

	tokens = working_set;
	return make_node (AST_type::varDecl, typespec, ID, lbracket_node,
	                  size_expr, rbracket_node, eq_node, bracelist);
}
