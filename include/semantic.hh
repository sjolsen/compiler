#ifndef SEMANTIC_HH
#define SEMANTIC_HH

#include <ast.hh>



void semantic_check (const program& p);

void semantic_check (const declList& node,
                     const symbol_table& global_table);

void semantic_check (const decl& node,
                     const symbol_table& global_table);

void semantic_check (const funDecl& node,
                     const symbol_table& global_table);

void semantic_check (const funBody& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table);

void semantic_check (const statementList& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table);

void semantic_check (const statement& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table);

void semantic_check (const compoundStmt& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table);

void semantic_check (const assignStmt& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table);

void semantic_check (const condStmt& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table);

void semantic_check (const loopStmt& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table);

void semantic_check (const returnStmt& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table);

mc_type semantic_check (const var& node,
                        const symbol_table& local_table,
                        const symbol_table& global_table);

void semantic_check (const expression& node);
void semantic_check (const relop& node);
void semantic_check (const addExpr& node);
void semantic_check (const addop& node);
void semantic_check (const term& node);
void semantic_check (const mulop& node);
void semantic_check (const factor& node);
void semantic_check (const funcCallExpr& node);
void semantic_check (const argList& node);



#endif
