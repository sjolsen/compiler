#ifndef ASMDEFS_HH
#define ASMDEFS_HH

#include <cstdint>
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>



typedef std::uint8_t mips_byte;
typedef std::uint32_t mips_word;
typedef std::uint64_t mips_dword;



enum class opname
{
	add,
	b,
	beq,
	div,
	jal,
	jr,
	lb,
	li,
	lw,
	mfhi,
	mflo,
	move,
	mult,
	nop,
	nor,
	bit_or,
	sb,
	sll,
	slt,
	sub,
	sw,
	xori
};



typedef int virt_reg; // Virtual register specifier

enum class real_reg // Physical register
	: mips_byte
{
	zero = 0,
	at = 1,
	v0 = 2,
	v1 = 3,
	a0 = 4,
	a1 = 5,
	a2 = 6,
	a3 = 7,
	t0 = 8,
	t1 = 9,
	t2 = 10,
	t3 = 11,
	t4 = 12,
	t5 = 13,
	t6 = 14,
	t7 = 15,
	s0 = 16,
	s1 = 17,
	s2 = 18,
	s3 = 19,
	s4 = 20,
	s5 = 21,
	s6 = 22,
	s7 = 23,
	t8 = 24,
	t9 = 25,
	k0 = 26,
	k1 = 27,
	gp = 28,
	sp = 29,
	fp = 30,
	ra = 31
};

class register_pool
{
private:
	std::deque <virt_reg> reg_queue;
	virt_reg next_register;
	std::unordered_map <std::string, virt_reg> var_map;

public:
	register_pool ();

	virt_reg get ();
	virt_reg get (const std::string& name);
	void release (virt_reg v);
	int max_live () const;
};

union mips_register
{
	real_reg real;
	virt_reg virt;
	int literal;

	mips_register (real_reg r);
	mips_register (virt_reg r);
	// mips_register (int i);
};



struct instruction
{
	opname op;

	mips_register _1;
	mips_register _2;
	mips_register _3;

	std::string label;
};



struct lvalue_reference
{
	virt_reg data_reg;
	virt_reg address_reg;
	std::vector <instruction> load_code;
	std::vector <instruction> store_code;
};



enum class asm_type
{
	mips_word,
	mips_byte
};



struct asm_data
{
	asm_type type;
	std::string name;
};



struct asm_function
{
	std::string name;
	std::vector <instruction> body;
};



std::string to_string (opname name);
std::string to_string (real_reg r);
std::string to_string (const instruction& i);
std::vector <std::string> lines (asm_type t);
std::vector <std::string> lines (const asm_data& datum);
std::vector <std::string> lines (const asm_function& func);



#endif
