#include <code_gen/asm_predicates.hh>

using namespace std;



namespace
{
	mips_byte mc_size (basic_mc_type t)
	{
		switch (t)
		{
		case basic_mc_type::mc_int:
			return 1;
		case basic_mc_type::mc_char:
			return 4;
		default:
			return 0;
		}
	}

	inline mips_byte mc_align (basic_mc_type t)
	{ return mc_size (move (t)); }

	mips_byte mc_size (mc_type t)
	{
		if (t.size != 0) // Array)
			return t.size;
		return mc_align (t.type);
	}
}



local_var_layout::local_var_layout ()
	: size (0)
{
}

local_var_layout::local_var_layout (typename vector <mc_type>::const_iterator first,
                                    typename vector <mc_type>::const_iterator last)
	: size (last - first)
{
	typename offsets::value_type current_offset = 0;

	for (auto i = first; i != last; ++i)
	{
		mips_byte align = mc_align (i->type);
		if (align != 0)
		{
			current_offset += current_offset % align;
			offsets.push_back (current_offset);
			current_offset += mc_size (*i);
		}
	}
}
