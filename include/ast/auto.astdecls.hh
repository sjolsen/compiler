struct declList
	: AST
{
	std::vector <Node <decl>> decls;
	declList ();
	declList (std::vector <Node <decl>>&& _decls);
	virtual std::vector <std::string> contents () const;
};

struct decl
	: AST
{
	Node <basic_decl> sub_decl;
	decl ();
	decl (Node <basic_decl>&& _sub_decl);
	virtual std::vector <std::string> contents () const;
};

struct typeSpecifier
	: AST
{
	Node <terminal> kwd_node;
	typeSpecifier ();
	typeSpecifier (Node <terminal>&& _kwd_node);
	virtual std::vector <std::string> contents () const;
};

struct formalDeclList
	: AST
{
	std::vector <Node <formalDecl>> decls;
	formalDeclList ();
	formalDeclList (std::vector <Node <formalDecl>>&& _decls);
	virtual std::vector <std::string> contents () const;
};

struct funBody
	: AST
{
	Node <localDeclList> decl_list;
	Node <statementList> stmt_list;
	funBody ();
	funBody (Node <localDeclList>&& _decl_list, Node <statementList>&& _stmt_list);
	virtual std::vector <std::string> contents () const;
};

struct localDeclList
	: AST
{
	std::vector <Node <varDecl>> decls;
	localDeclList ();
	localDeclList (std::vector <Node <varDecl>>&& _decls);
	virtual std::vector <std::string> contents () const;
};

struct statementList
	: AST
{
	std::vector <Node <statement>> stmts;
	statementList ();
	statementList (std::vector <Node <statement>>&& _stmts);
	virtual std::vector <std::string> contents () const;
};

struct statement
	: AST
{
	AST_node sub_stmt;
	statement ();
	statement (AST_node&& _sub_stmt);
	virtual std::vector <std::string> contents () const;
};

struct compoundStmt
	: AST
{
	Node <statementList> stmt_list;
	compoundStmt ();
	compoundStmt (Node <statementList>&& _stmt_list);
	virtual std::vector <std::string> contents () const;
};

struct assignStmt
	: AST
{
	Node <var> lvalue;
	Node <expression> rvalue;
	assignStmt ();
	assignStmt (Node <var>&& _lvalue, Node <expression>&& _rvalue);
	virtual std::vector <std::string> contents () const;
};

struct condStmt
	: AST
{
	Node <expression> cond_expr;
	Node <statement> then_stmt;
	Node <statement> else_stmt;
	condStmt ();
	condStmt (Node <expression>&& _cond_expr, Node <statement>&& _then_stmt, Node <statement>&& _else_stmt);
	virtual std::vector <std::string> contents () const;
};

struct loopStmt
	: AST
{
	Node <expression> cond_expr;
	Node <statement> then_stmt;
	loopStmt ();
	loopStmt (Node <expression>&& _cond_expr, Node <statement>&& _then_stmt);
	virtual std::vector <std::string> contents () const;
};

struct returnStmt
	: AST
{
	Node <terminal> rtrn_kwd;
	Node <expression> rtrn_expr;
	returnStmt ();
	returnStmt (Node <terminal>&& _rtrn_kwd, Node <expression>&& _rtrn_expr);
	virtual std::vector <std::string> contents () const;
	virtual file_position pos () const;
};

struct var
	: AST
{
	Node <terminal> name;
	Node <addExpr> size;
	var ();
	var (Node <terminal>&& _name, Node <addExpr>&& _size);
	virtual std::vector <std::string> contents () const;
	virtual file_position pos () const;
};

struct expression
	: AST
{
	Node <expression> lhs;
	Node <relop> op;
	Node <addExpr> rhs;
	expression ();
	expression (Node <expression>&& _lhs, Node <relop>&& _op, Node <addExpr>&& _rhs);
	virtual std::vector <std::string> contents () const;
	virtual file_position pos () const;
};

struct relop
	: AST
{
	Node <terminal> sym;
	relop ();
	relop (Node <terminal>&& _sym);
	virtual std::vector <std::string> contents () const;
	virtual file_position pos () const;
};

struct addExpr
	: AST
{
	Node <addExpr> lhs;
	Node <addop> op;
	Node <term> rhs;
	addExpr ();
	addExpr (Node <addExpr>&& _lhs, Node <addop>&& _op, Node <term>&& _rhs);
	virtual std::vector <std::string> contents () const;
	virtual file_position pos () const;
};

struct addop
	: AST
{
	Node <terminal> sym;
	addop ();
	addop (Node <terminal>&& _sym);
	virtual std::vector <std::string> contents () const;
	virtual file_position pos () const;
};

struct term
	: AST
{
	Node <term> lhs;
	Node <mulop> op;
	Node <factor> rhs;
	term ();
	term (Node <term>&& _lhs, Node <mulop>&& _op, Node <factor>&& _rhs);
	virtual std::vector <std::string> contents () const;
	virtual file_position pos () const;
};

struct mulop
	: AST
{
	Node <terminal> sym;
	mulop ();
	mulop (Node <terminal>&& _sym);
	virtual std::vector <std::string> contents () const;
	virtual file_position pos () const;
};

struct factor
	: AST
{
	AST_node rvalue;
	factor ();
	factor (AST_node&& _rvalue);
	virtual std::vector <std::string> contents () const;
	virtual file_position pos () const;
};

struct funcCallExpr
	: AST
{
	Node <terminal> name;
	Node <argList> arg_list;
	funcCallExpr ();
	funcCallExpr (Node <terminal>&& _name, Node <argList>&& _arg_list);
	virtual std::vector <std::string> contents () const;
	virtual file_position pos () const;
};

struct argList
	: AST
{
	std::vector <Node <expression>> args;
	argList ();
	argList (std::vector <Node <expression>>&& _args);
	virtual std::vector <std::string> contents () const;
};
