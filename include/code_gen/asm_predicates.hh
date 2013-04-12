#ifndef ASM_PREDICATES_HH
#define ASM_PREDICATES_HH

#include <code_gen/asmdefs.hh>
#include <ast.hh>



struct local_var_layout
{
	std::vector <mips_byte> offsets;
	mips_byte size;

	local_var_layout ();
	local_var_layout (typename std::vector <mc_type>::const_iterator first,
	                  typename std::vector <mc_type>::const_iterator last);
};



#endif
