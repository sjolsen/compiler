#ifndef AST_PREDICATES_HH
#define AST_PREDICATES_HH

#include <ast/astdef.hh>
#include <tokenizer.hh>



typedef std::shared_ptr <AST> AST_node;

AST program_p (const std::deque <token>& tokens);

AST_node declList_p    (token_range& tokens);
AST_node decl_p        (token_range& tokens);
AST_node varDeclStmt_p (token_range& tokens);
AST_node varDecl_p     (token_range& tokens);



#endif
