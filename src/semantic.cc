#include <semantic.hh>

#include <stdexcept>

using namespace std;



void semantic_check (const program& p)
{
	semantic_check (*p.decl_list, *p.table);
}



void semantic_check (const declList& node,
                     const symbol_table& global_table)
{
	for (const Node <decl>& declaration : node.decls)
		semantic_check (*declaration, global_table);
}



void semantic_check (const decl& node,
                     const symbol_table& global_table)
{
	if (node.sub_decl->type == AST_type::funDecl)
		semantic_check (reinterpret_cast <const funDecl&> (*node.sub_decl),
		                global_table);
}



void semantic_check (const funDecl& node,
                     const symbol_table& global_table)
{
	semantic_check (*node.body, *node.table, global_table);
}



void semantic_check (const funBody& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table)
{

	semantic_check (*node.stmt_list, local_table, global_table);
}



void semantic_check (const statementList& node,
                     const symbol_table& local_table,
                     const symbol_table& global_table)
{

}



void semantic_check (const statement& node)
{
}



void semantic_check (const compoundStmt& node)
{
}



void semantic_check (const assignStmt& node)
{
}



void semantic_check (const condStmt& node)
{
}



void semantic_check (const loopStmt& node)
{
}



void semantic_check (const returnStmt& node)
{
}



void semantic_check (const var& node)
{
}



void semantic_check (const expression& node)
{
}



void semantic_check (const relop& node)
{
}



void semantic_check (const addExpr& node)
{
}



void semantic_check (const addop& node)
{
}



void semantic_check (const term& node)
{
}



void semantic_check (const mulop& node)
{
}



void semantic_check (const factor& node)
{
}



void semantic_check (const funcCallExpr& node)
{
}



void semantic_check (const argList& node)
{
}
