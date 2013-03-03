program::program (AST_node _decl_list): decl_list(_decl_list) {type = AST_type::program;}
vector <string> program::contents () const {return collect (valid (decl_list) ? lines (decl_list) : vector <string> {} ); }
declList::declList (std::vector <AST_node> _decls): decls(_decls) {type = AST_type::declList;}
vector <string> declList::contents () const {return collect (valid (decls) ? lines (decls) : vector <string> {} ); }
decl::decl (AST_node _sub_decl): sub_decl(_sub_decl) {type = AST_type::decl;}
vector <string> decl::contents () const {return collect (valid (sub_decl) ? lines (sub_decl) : vector <string> {} ); }
varDecl::varDecl (AST_node _type_spec, AST_node _name, AST_node _size): type_spec(_type_spec) , name(_name) , size(_size) {type = AST_type::varDecl;}
vector <string> varDecl::contents () const {return collect (valid (type_spec) ? lines (type_spec) : vector <string> {} , valid (name) ? lines (name) : vector <string> {} , valid (size) ? lines (size) : vector <string> {} ); }
typeSpecifier::typeSpecifier (AST_node _kwd_node): kwd_node(_kwd_node) {type = AST_type::typeSpecifier;}
vector <string> typeSpecifier::contents () const {return collect (valid (kwd_node) ? lines (kwd_node) : vector <string> {} ); }
funDecl::funDecl (AST_node _type_spec, AST_node _name, AST_node _decl_list, AST_node _body): type_spec(_type_spec) , name(_name) , decl_list(_decl_list) , body(_body) {type = AST_type::funDecl;}
vector <string> funDecl::contents () const {return collect (valid (type_spec) ? lines (type_spec) : vector <string> {} , valid (name) ? lines (name) : vector <string> {} , valid (decl_list) ? lines (decl_list) : vector <string> {} , valid (body) ? lines (body) : vector <string> {} ); }
formalDeclList::formalDeclList (std::vector <AST_node> _decls): decls(_decls) {type = AST_type::formalDeclList;}
vector <string> formalDeclList::contents () const {return collect (valid (decls) ? lines (decls) : vector <string> {} ); }
formalDecl::formalDecl (AST_node _type_spec, AST_node _name, bool _is_array): type_spec(_type_spec) , name(_name) , is_array(_is_array) {type = AST_type::formalDecl;}
vector <string> formalDecl::contents () const {return collect (valid (type_spec) ? lines (type_spec) : vector <string> {} , valid (name) ? lines (name) : vector <string> {} , valid (is_array) ? lines (is_array) : vector <string> {} ); }
funBody::funBody (AST_node _decl_list, AST_node _stmt_list): decl_list(_decl_list) , stmt_list(_stmt_list) {type = AST_type::funBody;}
vector <string> funBody::contents () const {return collect (valid (decl_list) ? lines (decl_list) : vector <string> {} , valid (stmt_list) ? lines (stmt_list) : vector <string> {} ); }
localDeclList::localDeclList (std::vector <AST_node> _decls): decls(_decls) {type = AST_type::localDeclList;}
vector <string> localDeclList::contents () const {return collect (valid (decls) ? lines (decls) : vector <string> {} ); }
statementList::statementList (std::vector <AST_node> _stmts): stmts(_stmts) {type = AST_type::statementList;}
vector <string> statementList::contents () const {return collect (valid (stmts) ? lines (stmts) : vector <string> {} ); }
statement::statement (AST_node _sub_stmt): sub_stmt(_sub_stmt) {type = AST_type::statement;}
vector <string> statement::contents () const {return collect (valid (sub_stmt) ? lines (sub_stmt) : vector <string> {} ); }
compoundStmt::compoundStmt (AST_node _stmt_list): stmt_list(_stmt_list) {type = AST_type::compoundStmt;}
vector <string> compoundStmt::contents () const {return collect (valid (stmt_list) ? lines (stmt_list) : vector <string> {} ); }
assignStmt::assignStmt (AST_node _lvalue, AST_node _rvalue): lvalue(_lvalue) , rvalue(_rvalue) {type = AST_type::assignStmt;}
vector <string> assignStmt::contents () const {return collect (valid (lvalue) ? lines (lvalue) : vector <string> {} , valid (rvalue) ? lines (rvalue) : vector <string> {} ); }
condStmt::condStmt (AST_node _cond_expr, AST_node _then_stmt, AST_node _else_stmt): cond_expr(_cond_expr) , then_stmt(_then_stmt) , else_stmt(_else_stmt) {type = AST_type::condStmt;}
vector <string> condStmt::contents () const {return collect (valid (cond_expr) ? lines (cond_expr) : vector <string> {} , valid (then_stmt) ? lines (then_stmt) : vector <string> {} , valid (else_stmt) ? lines (else_stmt) : vector <string> {} ); }
loopStmt::loopStmt (AST_node _cond_expr, AST_node _then_stmt): cond_expr(_cond_expr) , then_stmt(_then_stmt) {type = AST_type::loopStmt;}
vector <string> loopStmt::contents () const {return collect (valid (cond_expr) ? lines (cond_expr) : vector <string> {} , valid (then_stmt) ? lines (then_stmt) : vector <string> {} ); }
returnStmt::returnStmt (AST_node _rtrn_expr): rtrn_expr(_rtrn_expr) {type = AST_type::returnStmt;}
vector <string> returnStmt::contents () const {return collect (valid (rtrn_expr) ? lines (rtrn_expr) : vector <string> {} ); }
var::var (AST_node _name, AST_node _size): name(_name) , size(_size) {type = AST_type::var;}
vector <string> var::contents () const {return collect (valid (name) ? lines (name) : vector <string> {} , valid (size) ? lines (size) : vector <string> {} ); }
expression::expression (AST_node _lhs, AST_node _op, AST_node _rhs): lhs(_lhs) , op(_op) , rhs(_rhs) {type = AST_type::expression;}
vector <string> expression::contents () const {return collect (valid (lhs) ? lines (lhs) : vector <string> {} , valid (op) ? lines (op) : vector <string> {} , valid (rhs) ? lines (rhs) : vector <string> {} ); }
relop::relop (AST_node _sym): sym(_sym) {type = AST_type::relop;}
vector <string> relop::contents () const {return collect (valid (sym) ? lines (sym) : vector <string> {} ); }
addExpr::addExpr (AST_node _lhs, AST_node _op, AST_node _rhs): lhs(_lhs) , op(_op) , rhs(_rhs) {type = AST_type::addExpr;}
vector <string> addExpr::contents () const {return collect (valid (lhs) ? lines (lhs) : vector <string> {} , valid (op) ? lines (op) : vector <string> {} , valid (rhs) ? lines (rhs) : vector <string> {} ); }
addop::addop (AST_node _sym): sym(_sym) {type = AST_type::addop;}
vector <string> addop::contents () const {return collect (valid (sym) ? lines (sym) : vector <string> {} ); }
term::term (AST_node _lhs, AST_node _op, AST_node _rhs): lhs(_lhs) , op(_op) , rhs(_rhs) {type = AST_type::term;}
vector <string> term::contents () const {return collect (valid (lhs) ? lines (lhs) : vector <string> {} , valid (op) ? lines (op) : vector <string> {} , valid (rhs) ? lines (rhs) : vector <string> {} ); }
mulop::mulop (AST_node _sym): sym(_sym) {type = AST_type::mulop;}
vector <string> mulop::contents () const {return collect (valid (sym) ? lines (sym) : vector <string> {} ); }
factor::factor (AST_node _rvalue): rvalue(_rvalue) {type = AST_type::factor;}
vector <string> factor::contents () const {return collect (valid (rvalue) ? lines (rvalue) : vector <string> {} ); }
funcCallExpr::funcCallExpr (AST_node _name, AST_node _arg_list): name(_name) , arg_list(_arg_list) {type = AST_type::funcCallExpr;}
vector <string> funcCallExpr::contents () const {return collect (valid (name) ? lines (name) : vector <string> {} , valid (arg_list) ? lines (arg_list) : vector <string> {} ); }
argList::argList (std::vector <AST_node> _args): args(_args) {type = AST_type::argList;}
vector <string> argList::contents () const {return collect (valid (args) ? lines (args) : vector <string> {} ); }
