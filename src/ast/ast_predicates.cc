#include <ast/ast_predicates.hh>

using namespace std;



namespace
{
	typedef vector <shared_ptr <AST>> childlist;
}



shared_ptr <AST> program_p (deque <token>& working_set)
{
	auto decl = decl_p (working_set);
	if (decl == nullptr)
		throw syntax_error ("Expected a declaration",
		                    working_set.front ().pos);

	auto declList = declList_p (working_set);
	if (declList == nullptr)
		throw logic_error ("declList_p returned a null pointer");

	if (!working_set.empty ())
		throw syntax_error ("Expected a declaration",
		                    working_set.front ().pos);

	return make_shared <AST> (AST_type::program, childlist {decl, declList});
}
