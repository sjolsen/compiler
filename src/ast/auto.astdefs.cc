program::program (AST_node _decl_list)
	: type (AST_type::program),
	  decl_list (_decl_list)
{
}

virtual vector <string> program::contents ()
{
	return collect (decl_list ? lines (decl_list) : {});
}

declList::declList (std::vector <AST_node> _decls)
	: type (AST_type::declList),
	  decls (_decls)
{
}

virtual vector <string> declList::contents ()
{
	return collect (decls ? lines (decls) : {});
}

decl::decl (AST_node _sub_decl)
	: type (AST_type::decl),
	  sub_decl (_sub_decl)
{
}

virtual vector <string> decl::contents ()
{
	return collect (sub_decl ? lines (sub_decl) : {});
}

varDecl::varDecl (AST_node _type_spec,
                  AST_node _name,
                  AST_node _size)
	: type (AST_type::varDecl),
	  type_spec (_type_spec)
{
}

virtual vector <string> varDecl::contents ()
{
	return collect (type_spec ? lines (type_spec) : {},
	                name ? lines (name) : {},
	                size ? lines (size) : {});
}

typeSpecifier::typeSpecifier (AST_node _kwd_node)
	: type (AST_type::typeSpecifier),
	  kwd_node (_kwd_node)
{
}

virtual vector <string> typeSpecifier::contents ()
{
	return collect (kwd_node ? lines (kwd_node) : {});
}

funDecl::funDecl (AST_node _type_spec,
                  AST_node _name,
                  AST_node _decl_list,
                  AST_node _body)
	: type (AST_type::funDecl),
	  type_spec (_type_spec)
{
}

virtual vector <string> funDecl::contents ()
{
	return collect (type_spec ? lines (type_spec) : {},
	                name ? lines (name) : {},
	                decl_list ? lines (decl_list) : {},
	                body ? lines (body) : {});
}

formalDeclList::formalDeclList (std::vector <AST_node> _decls)
	: type (AST_type::formalDeclList),
	  decls (_decls)
{
}

virtual vector <string> formalDeclList::contents ()
{
	return collect (decls ? lines (decls) : {});
}

formalDecl::formalDecl (AST_node _type_spec,
                        AST_node _name,
                        bool _is_array)
	: type (AST_type::formalDecl),
	  type_spec (_type_spec)
{
}

virtual vector <string> formalDecl::contents ()
{
	return collect (type_spec ? lines (type_spec) : {},
	                name ? lines (name) : {},
	                is_array ? lines (is_array) : {});
}

funBody::funBody (AST_node _decl_list,
                  AST_node _stmt_list)
	: type (AST_type::funBody),
	  decl_list (_decl_list)
{
}

virtual vector <string> funBody::contents ()
{
	return collect (decl_list ? lines (decl_list) : {},
	                stmt_list ? lines (stmt_list) : {});
}

localDeclList::localDeclList (std::vector <AST_node> _decls)
	: type (AST_type::localDeclList),
	  decls (_decls)
{
}

virtual vector <string> localDeclList::contents ()
{
	return collect (decls ? lines (decls) : {});
}

statementList::statementList (std::vector <AST_node> _stmts)
	: type (AST_type::statementList),
	  stmts (_stmts)
{
}

virtual vector <string> statementList::contents ()
{
	return collect (stmts ? lines (stmts) : {});
}

statement::statement (AST_node _sub_stmt)
	: type (AST_type::statement),
	  sub_stmt (_sub_stmt)
{
}

virtual vector <string> statement::contents ()
{
	return collect (sub_stmt ? lines (sub_stmt) : {});
}

compoundStmt::compoundStmt (AST_node _stmt_list)
	: type (AST_type::compoundStmt),
	  stmt_list (_stmt_list)
{
}

virtual vector <string> compoundStmt::contents ()
{
	return collect (stmt_list ? lines (stmt_list) : {});
}

assignStmt::assignStmt (AST_node _lvalue,
                        AST_node _rvalue)
	: type (AST_type::assignStmt),
	  lvalue (_lvalue)
{
}

virtual vector <string> assignStmt::contents ()
{
	return collect (lvalue ? lines (lvalue) : {},
	                rvalue ? lines (rvalue) : {});
}

condStmt::condStmt (AST_node _cond_expr,
                    AST_node _then_stmt,
                    AST_node _else_stmt)
	: type (AST_type::condStmt),
	  cond_expr (_cond_expr)
{
}

virtual vector <string> condStmt::contents ()
{
	return collect (cond_expr ? lines (cond_expr) : {},
	                then_stmt ? lines (then_stmt) : {},
	                else_stmt ? lines (else_stmt) : {});
}

loopStmt::loopStmt (AST_node _cond_expr,
                    AST_node _then_stmt)
	: type (AST_type::loopStmt),
	  cond_expr (_cond_expr)
{
}

virtual vector <string> loopStmt::contents ()
{
	return collect (cond_expr ? lines (cond_expr) : {},
	                then_stmt ? lines (then_stmt) : {});
}

returnStmt::returnStmt (AST_node _rtrn_expr)
	: type (AST_type::returnStmt),
	  rtrn_expr (_rtrn_expr)
{
}

virtual vector <string> returnStmt::contents ()
{
	return collect (rtrn_expr ? lines (rtrn_expr) : {});
}

var::var (AST_node _name,
          AST_node _size)
	: type (AST_type::var),
	  name (_name)
{
}

virtual vector <string> var::contents ()
{
	return collect (name ? lines (name) : {},
	                size ? lines (size) : {});
}

expression::expression (AST_node _lhs,
                        AST_node _op,
                        AST_node _rhs)
	: type (AST_type::expression),
	  lhs (_lhs)
{
}

virtual vector <string> expression::contents ()
{
	return collect (lhs ? lines (lhs) : {},
	                op ? lines (op) : {},
	                rhs ? lines (rhs) : {});
}

relop::relop (AST_node _sym)
	: type (AST_type::relop),
	  sym (_sym)
{
}

virtual vector <string> relop::contents ()
{
	return collect (sym ? lines (sym) : {});
}

addExpr::addExpr (AST_node _lhs,
                  AST_node _op,
                  AST_node _rhs)
	: type (AST_type::addExpr),
	  lhs (_lhs)
{
}

virtual vector <string> addExpr::contents ()
{
	return collect (lhs ? lines (lhs) : {},
	                op ? lines (op) : {},
	                rhs ? lines (rhs) : {});
}

addop::addop (AST_node _sym)
	: type (AST_type::addop),
	  sym (_sym)
{
}

virtual vector <string> addop::contents ()
{
	return collect (sym ? lines (sym) : {});
}

term::term (AST_node _lhs,
            AST_node _op,
            AST_node _rhs)
	: type (AST_type::term),
	  lhs (_lhs)
{
}

virtual vector <string> term::contents ()
{
	return collect (lhs ? lines (lhs) : {},
	                op ? lines (op) : {},
	                rhs ? lines (rhs) : {});
}

mulop::mulop (AST_node _sym)
	: type (AST_type::mulop),
	  sym (_sym)
{
}

virtual vector <string> mulop::contents ()
{
	return collect (sym ? lines (sym) : {});
}

factor::factor (AST_node _rvalue)
	: type (AST_type::factor),
	  rvalue (_rvalue)
{
}

virtual vector <string> factor::contents ()
{
	return collect (rvalue ? lines (rvalue) : {});
}

funcCallExpr::funcCallExpr (AST_node _name,
                            AST_node _arg_list)
	: type (AST_type::funcCallExpr),
	  name (_name)
{
}

virtual vector <string> funcCallExpr::contents ()
{
	return collect (name ? lines (name) : {},
	                arg_list ? lines (arg_list) : {});
}

argList::argList (std::vector <AST_node> _args)
	: type (AST_type::argList),
	  args (_args)
{
}

virtual vector <string> argList::contents ()
{
	return collect (args ? lines (args) : {});
}

