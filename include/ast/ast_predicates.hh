#ifndef AST_PREDICATES_HH
#define AST_PREDICATES_HH

#include <ast/astdef.hh>
#include <tokenizer.hh>



program program_p (const std::vector <token>& tokens);

std::shared_ptr <declList>       declList_p       (token_range& tokens);
std::shared_ptr <decl>           decl_p           (token_range& tokens);
std::shared_ptr <varDecl>        varDecl_p        (token_range& tokens);
std::shared_ptr <typeSpecifier>  typeSpecifier_p  (token_range& tokens);
std::shared_ptr <funDecl>        funDecl_p        (token_range& tokens);
std::shared_ptr <formalDeclList> formalDeclList_p (token_range& tokens);
std::shared_ptr <formalDecl>     formalDecl_p     (token_range& tokens);
std::shared_ptr <funBody>        funBody_p        (token_range& tokens);
std::shared_ptr <localDeclList>  localDeclList_p  (token_range& tokens);
std::shared_ptr <statementList>  statementList_p  (token_range& tokens);
std::shared_ptr <statement>      statement_p      (token_range& tokens);
std::shared_ptr <compoundStmt>   compoundStmt_p   (token_range& tokens);
std::shared_ptr <assignStmt>     assignStmt_p     (token_range& tokens);
std::shared_ptr <condStmt>       condStmt_p       (token_range& tokens);
std::shared_ptr <loopStmt>       loopStmt_p       (token_range& tokens);
std::shared_ptr <returnStmt>     returnStmt_p     (token_range& tokens);
std::shared_ptr <var>            var_p            (token_range& tokens);
std::shared_ptr <expression>     expression_p     (token_range& tokens);
std::shared_ptr <relop>          relop_p          (token_range& tokens);
std::shared_ptr <addExpr>        addExpr_p        (token_range& tokens);
std::shared_ptr <addop>          addop_p          (token_range& tokens);
std::shared_ptr <term>           term_p           (token_range& tokens);
std::shared_ptr <mulop>          mulop_p          (token_range& tokens);
std::shared_ptr <factor>         factor_p         (token_range& tokens);
std::shared_ptr <funcCallExpr>   funcCallExpr_p   (token_range& tokens);
std::shared_ptr <argList>        argList_p        (token_range& tokens);



#endif
