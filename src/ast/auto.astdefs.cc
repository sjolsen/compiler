program::program (Node <declList> _decl_list): decl_list(_decl_list) {type = AST_type::program;}
program::program () {type = AST_type::program;}
vector <string> program::contents () const {return collect (valid (decl_list) ? lines (decl_list) : vector <string> {} ); }
declList::declList (std::vector <Node <decl>> _decls): decls(_decls) {type = AST_type::declList;}
declList::declList () {type = AST_type::declList;}
vector <string> declList::contents () const {return collect (valid (decls) ? lines (decls) : vector <string> {} ); }
decl::decl (AST_node _sub_decl): sub_decl(_sub_decl) {type = AST_type::decl;}
decl::decl () {type = AST_type::decl;}
vector <string> decl::contents () const {return collect (valid (sub_decl) ? lines (sub_decl) : vector <string> {} ); }
varDecl::varDecl (Node <typeSpecifier> _type_spec, Node <terminal> _name, Node <terminal> _size): type_spec(_type_spec) , name(_name) , size(_size) {type = AST_type::varDecl;}
varDecl::varDecl () {type = AST_type::varDecl;}
vector <string> varDecl::contents () const {return collect (valid (type_spec) ? lines (type_spec) : vector <string> {} , valid (name) ? lines (name) : vector <string> {} , valid (size) ? lines (size) : vector <string> {} ); }
typeSpecifier::typeSpecifier (Node <terminal> _kwd_node): kwd_node(_kwd_node) {type = AST_type::typeSpecifier;}
typeSpecifier::typeSpecifier () {type = AST_type::typeSpecifier;}
vector <string> typeSpecifier::contents () const {return collect (valid (kwd_node) ? lines (kwd_node) : vector <string> {} ); }
funDecl::funDecl (Node <typeSpecifier> _type_spec, Node <terminal> _name, Node <formalDeclList> _decl_list, Node <funBody> _body): type_spec(_type_spec) , name(_name) , decl_list(_decl_list) , body(_body) {type = AST_type::funDecl;}
funDecl::funDecl () {type = AST_type::funDecl;}
vector <string> funDecl::contents () const {return collect (valid (type_spec) ? lines (type_spec) : vector <string> {} , valid (name) ? lines (name) : vector <string> {} , valid (decl_list) ? lines (decl_list) : vector <string> {} , valid (body) ? lines (body) : vector <string> {} ); }
formalDeclList::formalDeclList (std::vector <Node <formalDecl>> _decls): decls(_decls) {type = AST_type::formalDeclList;}
formalDeclList::formalDeclList () {type = AST_type::formalDeclList;}
vector <string> formalDeclList::contents () const {return collect (valid (decls) ? lines (decls) : vector <string> {} ); }
formalDecl::formalDecl (Node <typeSpecifier> _type_spec, Node <terminal> _name, bool _is_array): type_spec(_type_spec) , name(_name) , is_array(_is_array) {type = AST_type::formalDecl;}
formalDecl::formalDecl () {type = AST_type::formalDecl;}
vector <string> formalDecl::contents () const {return collect (valid (type_spec) ? lines (type_spec) : vector <string> {} , valid (name) ? lines (name) : vector <string> {} , valid (is_array) ? lines (is_array) : vector <string> {} ); }
funBody::funBody (Node <localDeclList> _decl_list, Node <statementList> _stmt_list): decl_list(_decl_list) , stmt_list(_stmt_list) {type = AST_type::funBody;}
funBody::funBody () {type = AST_type::funBody;}
vector <string> funBody::contents () const {return collect (valid (decl_list) ? lines (decl_list) : vector <string> {} , valid (stmt_list) ? lines (stmt_list) : vector <string> {} ); }
localDeclList::localDeclList (std::vector <Node <varDecl>> _decls): decls(_decls) {type = AST_type::localDeclList;}
localDeclList::localDeclList () {type = AST_type::localDeclList;}
vector <string> localDeclList::contents () const {return collect (valid (decls) ? lines (decls) : vector <string> {} ); }
statementList::statementList (std::vector <Node <statement>> _stmts): stmts(_stmts) {type = AST_type::statementList;}
statementList::statementList () {type = AST_type::statementList;}
vector <string> statementList::contents () const {return collect (valid (stmts) ? lines (stmts) : vector <string> {} ); }
statement::statement (AST_node _sub_stmt): sub_stmt(_sub_stmt) {type = AST_type::statement;}
statement::statement () {type = AST_type::statement;}
vector <string> statement::contents () const {return collect (valid (sub_stmt) ? lines (sub_stmt) : vector <string> {} ); }
compoundStmt::compoundStmt (Node <statementList> _stmt_list): stmt_list(_stmt_list) {type = AST_type::compoundStmt;}
compoundStmt::compoundStmt () {type = AST_type::compoundStmt;}
vector <string> compoundStmt::contents () const {return collect (valid (stmt_list) ? lines (stmt_list) : vector <string> {} ); }
assignStmt::assignStmt (Node <var> _lvalue, Node <expression> _rvalue): lvalue(_lvalue) , rvalue(_rvalue) {type = AST_type::assignStmt;}
assignStmt::assignStmt () {type = AST_type::assignStmt;}
vector <string> assignStmt::contents () const {return collect (valid (lvalue) ? lines (lvalue) : vector <string> {} , valid (rvalue) ? lines (rvalue) : vector <string> {} ); }
condStmt::condStmt (Node <expression> _cond_expr, Node <statement> _then_stmt, Node <statement> _else_stmt): cond_expr(_cond_expr) , then_stmt(_then_stmt) , else_stmt(_else_stmt) {type = AST_type::condStmt;}
condStmt::condStmt () {type = AST_type::condStmt;}
vector <string> condStmt::contents () const {return collect (valid (cond_expr) ? lines (cond_expr) : vector <string> {} , valid (then_stmt) ? lines (then_stmt) : vector <string> {} , valid (else_stmt) ? lines (else_stmt) : vector <string> {} ); }
loopStmt::loopStmt (Node <expression> _cond_expr, Node <statement> _then_stmt): cond_expr(_cond_expr) , then_stmt(_then_stmt) {type = AST_type::loopStmt;}
loopStmt::loopStmt () {type = AST_type::loopStmt;}
vector <string> loopStmt::contents () const {return collect (valid (cond_expr) ? lines (cond_expr) : vector <string> {} , valid (then_stmt) ? lines (then_stmt) : vector <string> {} ); }
returnStmt::returnStmt (Node <expression> _rtrn_expr): rtrn_expr(_rtrn_expr) {type = AST_type::returnStmt;}
returnStmt::returnStmt () {type = AST_type::returnStmt;}
vector <string> returnStmt::contents () const {return collect (valid (rtrn_expr) ? lines (rtrn_expr) : vector <string> {} ); }
var::var (Node <terminal> _name, Node <addExpr> _size): name(_name) , size(_size) {type = AST_type::var;}
var::var () {type = AST_type::var;}
vector <string> var::contents () const {return collect (valid (name) ? lines (name) : vector <string> {} , valid (size) ? lines (size) : vector <string> {} ); }
expression::expression (Node <expression> _lhs, Node <relop> _op, Node <addExpr> _rhs): lhs(_lhs) , op(_op) , rhs(_rhs) {type = AST_type::expression;}
expression::expression () {type = AST_type::expression;}
vector <string> expression::contents () const {return collect (valid (lhs) ? lines (lhs) : vector <string> {} , valid (op) ? lines (op) : vector <string> {} , valid (rhs) ? lines (rhs) : vector <string> {} ); }
relop::relop (Node <terminal> _sym): sym(_sym) {type = AST_type::relop;}
relop::relop () {type = AST_type::relop;}
vector <string> relop::contents () const {return collect (valid (sym) ? lines (sym) : vector <string> {} ); }
addExpr::addExpr (Node <addExpr> _lhs, Node <addop> _op, Node <term> _rhs): lhs(_lhs) , op(_op) , rhs(_rhs) {type = AST_type::addExpr;}
addExpr::addExpr () {type = AST_type::addExpr;}
vector <string> addExpr::contents () const {return collect (valid (lhs) ? lines (lhs) : vector <string> {} , valid (op) ? lines (op) : vector <string> {} , valid (rhs) ? lines (rhs) : vector <string> {} ); }
addop::addop (Node <terminal> _sym): sym(_sym) {type = AST_type::addop;}
addop::addop () {type = AST_type::addop;}
vector <string> addop::contents () const {return collect (valid (sym) ? lines (sym) : vector <string> {} ); }
term::term (Node <term> _lhs, Node <mulop> _op, Node <factor> _rhs): lhs(_lhs) , op(_op) , rhs(_rhs) {type = AST_type::term;}
term::term () {type = AST_type::term;}
vector <string> term::contents () const {return collect (valid (lhs) ? lines (lhs) : vector <string> {} , valid (op) ? lines (op) : vector <string> {} , valid (rhs) ? lines (rhs) : vector <string> {} ); }
mulop::mulop (Node <terminal> _sym): sym(_sym) {type = AST_type::mulop;}
mulop::mulop () {type = AST_type::mulop;}
vector <string> mulop::contents () const {return collect (valid (sym) ? lines (sym) : vector <string> {} ); }
factor::factor (AST_node _rvalue): rvalue(_rvalue) {type = AST_type::factor;}
factor::factor () {type = AST_type::factor;}
vector <string> factor::contents () const {return collect (valid (rvalue) ? lines (rvalue) : vector <string> {} ); }
funcCallExpr::funcCallExpr (Node <terminal> _name, Node <argList> _arg_list): name(_name) , arg_list(_arg_list) {type = AST_type::funcCallExpr;}
funcCallExpr::funcCallExpr () {type = AST_type::funcCallExpr;}
vector <string> funcCallExpr::contents () const {return collect (valid (name) ? lines (name) : vector <string> {} , valid (arg_list) ? lines (arg_list) : vector <string> {} ); }
argList::argList (std::vector <Node <expression>> _args): args(_args) {type = AST_type::argList;}
argList::argList () {type = AST_type::argList;}
vector <string> argList::contents () const {return collect (valid (args) ? lines (args) : vector <string> {} ); }
