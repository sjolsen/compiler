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



vector <string> code_gen (const program& node);}



vector <string> code_gen (const declList& node,
                          const symbol_table& global_table)




asm_function code_gen (const funDecl& node,
                       const symbol_table& global_table);



vector <instruction> code_gen (const funBody& node,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table);



vector <instruction> code_gen (const statementList& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table);



vector <instruction> code_gen (const statement& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table);



vector <instruction> code_gen (const compoundStmt& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table);



vector <instruction> code_gen (const assignStmt& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table);



vector <instruction> code_gen (const condStmt& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table);



vector <instruction> code_gen (const loopStmt& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table);



vecotr <instruction> code_gen (const returnStmt& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table);



lvalue_reference code_gen (const var& node,
                           register_pool& vregs,
                           const symbol_table& local_table,
                           const symbol_table& param_table,
                           const symbol_table& global_table);



vector <instruction> code_gen (const expression& node,
                               mips_register r,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table);



vector <instruction> code_gen (const relop& op,
                               mips_register l,
                               mips_register r);



vector <instruction> code_gen (const addExpr& node,
                               mips_register r,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table);



vector <instruction> code_gen (const term& node,
                               mips_register r,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table);



vector <instruction> code_gen (const factor& node,
                               mips_register r,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table);



vector <instruction> code_gen (const funcCallExpr& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table);



vector <instruction> code_gen (const argList& node,
                               register_pool& vregs,
                               const symbol_table& local_table,
                               const symbol_table& param_table,
                               const symbol_table& global_table);



vector <instruction> schedule_code (vector <instruction> code,
                                    register_pool& vregs,
                                    const symbol_table& local_table,
                                    const symbol_table& param_table,
                                    const symbol_table& global_table);



#endif
