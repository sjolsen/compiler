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

void populate_table (symbol_table& table,
                     const program& tree)
{
	for (const Node <decl>& decl_node : tree.decl_list->decls)
	{
		if (table.count (decl_node->sub_decl->get_name ()) > 0)
			throw syntax_error ("Redefined identifier (first definition here)",
			                    table [decl_node->sub_decl->get_name ()].decl_node->pos ());

		table [decl_node->sub_decl->get_name ()] = symbol_entry {decl_node->sub_decl->get_name (),
		                                                         decl_node->sub_decl->get_type (),
		                                                         decl_node->sub_decl};
	}
}

void populate_table (symbol_table& table,
                     const funDecl& tree);
