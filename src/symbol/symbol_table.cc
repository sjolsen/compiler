#include <symbol/symbol_table.hh>

using namespace std;



// namespace
// {
// 	void s_table_ctor (symbol_table* table,
// 	                   const AST& node)
// 	{
// 		if (node.type == AST_type::varDecl &&
// 		    node.children [1]->tokenp->type == token_type::identifier)
// 		{
// 			if (count (node.tokenp->str) > 0)
// 				throw syntax_error ("Redefined variable (first definition here)",
// 				                    (*this) [node.tokenp->str].deftoken.pos);
// 			(*this) [node.tokenp->str] = {
// 		}
// 		else
// 			for (const AST_node& child : node.children)
// 				s_table_ctor (table, *child);
// 	}
// }



symbol_table::symbol_table (const AST& tree)
{
	// s_table_ctor (this, &tree);
}
