struct program : AST {AST_node decl_list;program(AST_node _decl_list);virtual std::vector <std::string> contents () override; };
struct declList : AST {std::vector <AST_node> decls;declList(std::vector <AST_node> _decls);virtual std::vector <std::string> contents () override; };
struct decl : AST {AST_node sub_decl;decl(AST_node _sub_decl);virtual std::vector <std::string> contents () override; };
struct varDecl : AST {AST_node type_spec;AST_node name;AST_node size;varDecl(AST_node _type_spec, AST_node _name, AST_node _size);virtual std::vector <std::string> contents () override; };
struct typeSpecifier : AST {AST_node kwd_node;typeSpecifier(AST_node _kwd_node);virtual std::vector <std::string> contents () override; };
struct funDecl : AST {AST_node type_spec;AST_node name;AST_node decl_list;AST_node body;funDecl(AST_node _type_spec, AST_node _name, AST_node _decl_list, AST_node _body);virtual std::vector <std::string> contents () override; };
struct formalDeclList : AST {std::vector <AST_node> decls;formalDeclList(std::vector <AST_node> _decls);virtual std::vector <std::string> contents () override; };
struct formalDecl : AST {AST_node type_spec;AST_node name;bool is_array;formalDecl(AST_node _type_spec, AST_node _name, bool _is_array);virtual std::vector <std::string> contents () override; };
struct funBody : AST {AST_node decl_list;AST_node stmt_list;funBody(AST_node _decl_list, AST_node _stmt_list);virtual std::vector <std::string> contents () override; };
struct localDeclList : AST {std::vector <AST_node> decls;localDeclList(std::vector <AST_node> _decls);virtual std::vector <std::string> contents () override; };
struct statementList : AST {std::vector <AST_node> stmts;statementList(std::vector <AST_node> _stmts);virtual std::vector <std::string> contents () override; };
struct statement : AST {AST_node sub_stmt;statement(AST_node _sub_stmt);virtual std::vector <std::string> contents () override; };
struct compoundStmt : AST {AST_node stmt_list;compoundStmt(AST_node _stmt_list);virtual std::vector <std::string> contents () override; };
struct assignStmt : AST {AST_node lvalue;AST_node rvalue;assignStmt(AST_node _lvalue, AST_node _rvalue);virtual std::vector <std::string> contents () override; };
struct condStmt : AST {AST_node cond_expr;AST_node then_stmt;AST_node else_stmt;condStmt(AST_node _cond_expr, AST_node _then_stmt, AST_node _else_stmt);virtual std::vector <std::string> contents () override; };
struct loopStmt : AST {AST_node cond_expr;AST_node then_stmt;loopStmt(AST_node _cond_expr, AST_node _then_stmt);virtual std::vector <std::string> contents () override; };
struct returnStmt : AST {AST_node rtrn_expr;returnStmt(AST_node _rtrn_expr);virtual std::vector <std::string> contents () override; };
struct var : AST {AST_node name;AST_node size;var(AST_node _name, AST_node _size);virtual std::vector <std::string> contents () override; };
struct expression : AST {AST_node lhs;AST_node op;AST_node rhs;expression(AST_node _lhs, AST_node _op, AST_node _rhs);virtual std::vector <std::string> contents () override; };
struct relop : AST {AST_node sym;relop(AST_node _sym);virtual std::vector <std::string> contents () override; };
struct addExpr : AST {AST_node lhs;AST_node op;AST_node rhs;addExpr(AST_node _lhs, AST_node _op, AST_node _rhs);virtual std::vector <std::string> contents () override; };
struct addop : AST {AST_node sym;addop(AST_node _sym);virtual std::vector <std::string> contents () override; };
struct term : AST {AST_node lhs;AST_node op;AST_node rhs;term(AST_node _lhs, AST_node _op, AST_node _rhs);virtual std::vector <std::string> contents () override; };
struct mulop : AST {AST_node sym;mulop(AST_node _sym);virtual std::vector <std::string> contents () override; };
struct factor : AST {AST_node rvalue;factor(AST_node _rvalue);virtual std::vector <std::string> contents () override; };
struct funcCallExpr : AST {AST_node name;AST_node arg_list;funcCallExpr(AST_node _name, AST_node _arg_list);virtual std::vector <std::string> contents () override; };
struct argList : AST {std::vector <AST_node> args;argList(std::vector <AST_node> _args);virtual std::vector <std::string> contents () override; };
