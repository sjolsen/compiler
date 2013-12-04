declList::declList (std::vector <Node <decl>>&& _decls)
	: decls (move (_decls))
{
	type = AST_type::declList;
	link (this, decls);
}

declList::declList ()
{
	type = AST_type::declList;
}

vector <string> declList::contents () const
{
	return collect (valid (decls) ? lines (decls) : vector <string> {});
}

decl::decl (Node <basic_decl>&& _sub_decl)
	: sub_decl (move (_sub_decl))
{
	type = AST_type::decl;
	link (this, sub_decl);
}

decl::decl ()
{
	type = AST_type::decl;
}

vector <string> decl::contents () const
{
	return collect (valid (sub_decl) ? lines (sub_decl) : vector <string> {});
}

typeSpecifier::typeSpecifier (Node <terminal>&& _kwd_node)
	: kwd_node (move (_kwd_node))
{
	type = AST_type::typeSpecifier;
	link (this, kwd_node);
}

typeSpecifier::typeSpecifier ()
{
	type = AST_type::typeSpecifier;
}

vector <string> typeSpecifier::contents () const
{
	return collect (valid (kwd_node) ? lines (kwd_node) : vector <string> {});
}

formalDeclList::formalDeclList (std::vector <Node <formalDecl>>&& _decls)
	: decls (move (_decls))
{
	type = AST_type::formalDeclList;
	link (this, decls);
}

formalDeclList::formalDeclList ()
{
	type = AST_type::formalDeclList;
}

vector <string> formalDeclList::contents () const
{
	return collect (valid (decls) ? lines (decls) : vector <string> {});
}

funBody::funBody (Node <localDeclList>&& _decl_list, Node <statementList>&& _stmt_list)
	: decl_list (move (_decl_list)),
	  stmt_list (move (_stmt_list))
{
	type = AST_type::funBody;
	link (this, decl_list, stmt_list);
}

funBody::funBody ()
{
	type = AST_type::funBody;
}

vector <string> funBody::contents () const
{
	return collect (valid (decl_list) ? lines (decl_list) : vector <string> {},
	                valid (stmt_list) ? lines (stmt_list) : vector <string> {});
}

localDeclList::localDeclList (std::vector <Node <varDecl>>&& _decls)
	: decls (move (_decls))
{
	type = AST_type::localDeclList;
	link (this, decls);
}

localDeclList::localDeclList ()
{
	type = AST_type::localDeclList;
}

vector <string> localDeclList::contents () const
{
	return collect (valid (decls) ? lines (decls) : vector <string> {});
}

statementList::statementList (std::vector <Node <statement>>&& _stmts)
	: stmts (move (_stmts))
{
	type = AST_type::statementList;
	link (this, stmts);
}

statementList::statementList ()
{
	type = AST_type::statementList;
}

vector <string> statementList::contents () const
{
	return collect (valid (stmts) ? lines (stmts) : vector <string> {});
}

statement::statement (AST_node&& _sub_stmt)
	: sub_stmt (move (_sub_stmt))
{
	type = AST_type::statement;
	link (this, sub_stmt);
}

statement::statement ()
{
	type = AST_type::statement;
}

vector <string> statement::contents () const
{
	return collect (valid (sub_stmt) ? lines (sub_stmt) : vector <string> {});
}

compoundStmt::compoundStmt (Node <statementList>&& _stmt_list)
	: stmt_list (move (_stmt_list))
{
	type = AST_type::compoundStmt;
	link (this, stmt_list);
}

compoundStmt::compoundStmt ()
{
	type = AST_type::compoundStmt;
}

vector <string> compoundStmt::contents () const
{
	return collect (valid (stmt_list) ? lines (stmt_list) : vector <string> {});
}

assignStmt::assignStmt (Node <var>&& _lvalue, Node <expression>&& _rvalue)
	: lvalue (move (_lvalue)),
	  rvalue (move (_rvalue))
{
	type = AST_type::assignStmt;
	link (this, lvalue, rvalue);
}

assignStmt::assignStmt ()
{
	type = AST_type::assignStmt;
}

vector <string> assignStmt::contents () const
{
	return collect (valid (lvalue) ? lines (lvalue) : vector <string>
	                {},
	                valid (rvalue) ? lines (rvalue) : vector <string> {});
}

condStmt::condStmt (Node <expression>&& _cond_expr, Node <statement>&& _then_stmt, Node <statement>&& _else_stmt)
	: cond_expr (move (_cond_expr)),
	  then_stmt (move (_then_stmt)),
	  else_stmt (move (_else_stmt))
{
	type = AST_type::condStmt;
	link (this, cond_expr, then_stmt, else_stmt);
}

condStmt::condStmt ()
{
	type = AST_type::condStmt;
}

vector <string> condStmt::contents () const
{
	return collect (valid (cond_expr) ? lines (cond_expr) : vector <string> {},
	                valid (then_stmt) ? lines (then_stmt) : vector <string> {},
	                valid (else_stmt) ? lines (else_stmt) : vector <string> {});
}

loopStmt::loopStmt (Node <expression>&& _cond_expr, Node <statement>&& _then_stmt)
	: cond_expr (move (_cond_expr)),
	  then_stmt (move (_then_stmt))
{
	type = AST_type::loopStmt;
	link (this, cond_expr, then_stmt);
}

loopStmt::loopStmt ()
{
	type = AST_type::loopStmt;
}

vector <string> loopStmt::contents () const
{
	return collect (valid (cond_expr) ? lines (cond_expr) : vector <string> {},
	                valid (then_stmt) ? lines (then_stmt) : vector <string> {});
}

returnStmt::returnStmt (Node <terminal>&& _rtrn_kwd, Node <expression>&& _rtrn_expr)
	: rtrn_kwd (move (_rtrn_kwd)),
	  rtrn_expr (move (_rtrn_expr))
{
	type = AST_type::returnStmt;
	link (this, rtrn_kwd, rtrn_expr);
}

returnStmt::returnStmt ()
{
	type = AST_type::returnStmt;
}

vector <string> returnStmt::contents () const
{
	return collect (valid (rtrn_kwd) ? lines (rtrn_kwd) : vector <string> {},
	                valid (rtrn_expr) ? lines (rtrn_expr) : vector <string> {});
}

var::var (Node <terminal>&& _name, Node <addExpr>&& _size)
	: name (move (_name)),
	  size (move (_size))
{
	type = AST_type::var;
	link (this, name, size);
}

var::var ()
{
	type = AST_type::var;
}

vector <string> var::contents () const
{
	return collect (valid (name) ? lines (name) : vector <string> {},
	                valid (size) ? lines (size) : vector <string> {});
}

expression::expression (Node <expression>&& _lhs, Node <relop>&& _op, Node <addExpr>&& _rhs)
	: lhs (move (_lhs)),
	  op (move (_op)),
	  rhs (move (_rhs))
{
	type = AST_type::expression;
	link (this, lhs, op, rhs);
}

expression::expression ()
{
	type = AST_type::expression;
}

vector <string> expression::contents () const
{
	return collect (valid (lhs) ? lines (lhs) : vector <string> {},
	                valid (op) ? lines (op) : vector <string> {},
	                valid (rhs) ? lines (rhs) : vector <string> {});
}

relop::relop (Node <terminal>&& _sym)
	: sym (move (_sym))
{
	type = AST_type::relop;
	link (this, sym);
}

relop::relop ()
{
	type = AST_type::relop;
}

vector <string> relop::contents () const
{
	return collect (valid (sym) ? lines (sym) : vector <string> {});
}

addExpr::addExpr (Node <addExpr>&& _lhs, Node <addop>&& _op, Node <term>&& _rhs)
	: lhs (move (_lhs)),
	  op (move (_op)),
	  rhs (move (_rhs))
{
	type = AST_type::addExpr;
	link (this, lhs, op, rhs);
}

addExpr::addExpr ()
{
	type = AST_type::addExpr;
}

vector <string> addExpr::contents () const
{
	return collect (valid (lhs) ? lines (lhs) : vector <string> {},
	                valid (op) ? lines (op) : vector <string> {},
	                valid (rhs) ? lines (rhs) : vector <string> {});
}

addop::addop (Node <terminal>&& _sym)
	: sym (move (_sym))
{
	type = AST_type::addop;
	link (this, sym);
}

addop::addop ()
{
	type = AST_type::addop;
}

vector <string> addop::contents () const
{
	return collect (valid (sym) ? lines (sym) : vector <string> {});
}

term::term (Node <term>&& _lhs, Node <mulop>&& _op, Node <factor>&& _rhs)
	: lhs (move (_lhs)),
	  op (move (_op)),
	  rhs (move (_rhs))
{
	type = AST_type::term;
	link (this, lhs, op, rhs);
}

term::term ()
{
	type = AST_type::term;
}

vector <string> term::contents () const
{
	return collect (valid (lhs) ? lines (lhs) : vector <string> {},
	                valid (op) ? lines (op) : vector <string> {},
	                valid (rhs) ? lines (rhs) : vector <string> {});
}

mulop::mulop (Node <terminal>&& _sym)
	: sym (move (_sym))
{
	type = AST_type::mulop;
	link (this, sym);
}

mulop::mulop ()
{
	type = AST_type::mulop;
}

vector <string> mulop::contents () const
{
	return collect (valid (sym) ? lines (sym) : vector <string> {});
}

factor::factor (AST_node&& _rvalue)
	: rvalue (move (_rvalue))
{
	type = AST_type::factor;
	link (this, rvalue);
}

factor::factor ()
{
	type = AST_type::factor;
}

vector <string> factor::contents () const
{
	return collect (valid (rvalue) ? lines (rvalue) : vector <string> {});
}

funcCallExpr::funcCallExpr (Node <terminal>&& _name, Node <argList>&& _arg_list)
	: name (move (_name)),
	  arg_list (move (_arg_list))
{
	type = AST_type::funcCallExpr;
	link (this, name, arg_list);
}

funcCallExpr::funcCallExpr ()
{
	type = AST_type::funcCallExpr;
}

vector <string> funcCallExpr::contents () const
{
	return collect (valid (name) ? lines (name) : vector <string> {},
	                valid (arg_list) ? lines (arg_list) : vector <string> {});
}

argList::argList (std::vector <Node <expression>>&& _args)
	: args (move (_args))
{
	type = AST_type::argList;
	link (this, args);
}

argList::argList ()
{
	type = AST_type::argList;
}

vector <string> argList::contents () const
{
	return collect (valid (args) ? lines (args) : vector <string> {});
}
