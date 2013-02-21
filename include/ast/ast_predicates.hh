#ifndef AST_PREDICATES_HH
#define AST_PREDICATES_HH

#include <ast/astdef.hh>
#include <tokenizer.hh>



typedef std::shared_ptr <AST> AST_node;

AST program_p (const std::vector <token>& tokens);

AST_node declList_p       (token_range& tokens);
AST_node decl_p           (token_range& tokens);
AST_node varDeclStmt_p    (token_range& tokens);
AST_node varDecl_p        (token_range& tokens);
AST_node stringDecl_p     (token_range& tokens);
AST_node intTypeSpec_p    (token_range& tokens);
AST_node typeSpecifier_p  (token_range& tokens);
AST_node initBraceList_p  (token_range& tokens);
AST_node exprList_p       (token_range& tokens);
AST_node funDecl_p        (token_range& tokens);
AST_node formalDeclList_p (token_range& tokens);
AST_node formalDecl_p     (token_range& tokens);
AST_node statementList_p  (token_range& tokens);
AST_node statement_p      (token_range& tokens);
AST_node compoundStmt_p   (token_range& tokens);
AST_node condStmt_p       (token_range& tokens);
AST_node loopStmt_p       (token_range& tokens);
AST_node returnStmt_p     (token_range& tokens);
AST_node lvalue_p         (token_range& tokens);
AST_node rvalue_p         (token_range& tokens);
AST_node expression_p     (token_range& tokens);
AST_node equivExpr_p      (token_range& tokens);
AST_node equivop_p        (token_range& tokens);
AST_node relExpr_p        (token_range& tokens);
AST_node relop_p          (token_range& tokens);
AST_node addExpr_p        (token_range& tokens);
AST_node addop_p          (token_range& tokens);
AST_node term_p           (token_range& tokens);
AST_node mulop_p          (token_range& tokens);
AST_node funcCallExpr_p   (token_range& tokens);
AST_node argList_p        (token_range& tokens);



#endif
