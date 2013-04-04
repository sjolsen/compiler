#ifndef ASTPRINT_HH
#define ASTPRINT_HH

#include <ast/astdef.hh>
#include <ast/symbol_table.hh>



std::vector <std::string> collect (std::vector <std::string>&& v);

template <typename... Rest>
std::vector <std::string> collect (std::vector <std::string>&& first,
                                   Rest&&... rest)
{
	auto&& rest_lines = collect (std::forward <Rest> (rest)...);
	first.reserve (first.size () + rest_lines.size ());
	for (std::string& line : rest_lines)
		first.emplace_back (move (line));
	return first;
}

std::vector <std::string> lines (const symbol_table& table);
std::vector <std::string> lines (const AST& tree);

template <typename AstType>
std::vector <std::string> lines (const Node <AstType>& node)
{
	return lines (*node);
}

template <typename NodeType>
std::vector <std::string> lines (const std::vector <NodeType>& nodes)
{
	std::vector <std::string> output;
	for (const NodeType& node : nodes)
		output = collect (move (output), lines (node));
	return output;
}

std::vector <std::string> lines (bool b);

template <typename NodeType>
bool valid (const std::vector <NodeType>& nodes)
{ return !nodes.empty (); }

template <typename AstType>
bool valid (const Node <AstType>& node)
{ return static_cast <bool> (node); }

bool valid (bool b);




#endif
