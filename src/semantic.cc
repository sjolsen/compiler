#include <semantic.hh>



namespace
{
	void check_undeclared (const funBody& body)
	{
	}
	void check_funcalls (const funBody& body)
	{
	}
	void check_array_accesses (const funBody& body)
	{
	}
	void check_assignments (const funBody& body)
	{
	}
}



void semantic_check (const program& p)
{
	for (const Node <decl>& top_level_decl : p.decl_list->decls)
		if (top_level_decl->sub_decl->type == AST_type::funDecl)
		{
			const funBody& body = *(dynamic_cast <const funDecl&> (*top_level_decl->sub_decl)).body;

			check_undeclared (body);
			check_funcalls (body);
			check_array_accesses (body);
			check_assignments (body);
		}
}
