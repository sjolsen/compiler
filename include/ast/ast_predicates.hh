#ifndef AST_PREDICATES_HH
#define AST_PREDICATES_HH

#include <ast/astdef.hh>
#include <tokenizer.hh>



program program_p (const std::vector <token>& tokens);

Node <declList>       declList_p       (token_range& tokens);
Node <decl>           decl_p           (token_range& tokens);
Node <varDecl>        varDecl_p        (token_range& tokens);
Node <typeSpecifier>  typeSpecifier_p  (token_range& tokens);
Node <funDecl>        funDecl_p        (token_range& tokens);
Node <formalDeclList> formalDeclList_p (token_range& tokens);
Node <formalDecl>     formalDecl_p     (token_range& tokens);
Node <funBody>        funBody_p        (token_range& tokens);
Node <localDeclList>  localDeclList_p  (token_range& tokens);
Node <statementList>  statementList_p  (token_range& tokens);
Node <statement>      statement_p      (token_range& tokens);
Node <compoundStmt>   compoundStmt_p   (token_range& tokens);
Node <assignStmt>     assignStmt_p     (token_range& tokens);
Node <condStmt>       condStmt_p       (token_range& tokens);
Node <loopStmt>       loopStmt_p       (token_range& tokens);
Node <returnStmt>     returnStmt_p     (token_range& tokens);
Node <var>            var_p            (token_range& tokens);
Node <expression>     expression_p     (token_range& tokens);
Node <relop>          relop_p          (token_range& tokens);
Node <addExpr>        addExpr_p        (token_range& tokens);
Node <addop>          addop_p          (token_range& tokens);
Node <term>           term_p           (token_range& tokens);
Node <mulop>          mulop_p          (token_range& tokens);
Node <factor>         factor_p         (token_range& tokens);
Node <funcCallExpr>   funcCallExpr_p   (token_range& tokens);
Node <argList>        argList_p        (token_range& tokens);



#endif
