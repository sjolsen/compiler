#include <ast/astprint.hh>

using namespace std;



vector <string> collect (vector <string>&& v)
{ return v; }

vector <string> lines (const symbol_table& table)
{
	vector <string> lines = {"(symbol_table"};
	for (const auto& kv_pair : table)
		lines.emplace_back ("  (" + kv_pair.first + ' ' + to_string (kv_pair.second.type) + ')');
	lines.back () += ')';

	return lines;
}

vector <string> lines (const AST& tree)
{
	vector <string> lines = {'(' + to_string (tree.type)};
	for (string& line : tree.contents ())
		lines.emplace_back ("  " + move (line));
	lines.back () += ')';

	return lines;
}

vector <string> lines (const AST_node& node)
{
	return lines (*node);
}

vector <string> lines (bool b)
{ return b ? vector <string> {"[]"} : vector <string> {}; }

bool valid (const AST_node& node)
{ return node != nullptr; }

bool valid (bool b)
{ return b; }
