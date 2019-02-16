#ifndef __ARMV8_CTX_H__
#define __ARMV8_CTX_H__

//#include <mips/mips.h>

#ifdef __ASSEMBLER__

//#include <mips/asm.h>
#define CTX_X19 0x0
#define CTX_X20 0x8
#define CTX_X21 0x10
#define CTX_X22 0x18
#define CTX_X23 0x28
#define CTX_X24 0x30
#define CTX_X25 0x38
#define CTX_X26 0x40
#define CTX_X27 0x48
#define CTX_X28 0x50
#define CTX_fp 0x58
#define CTX_lr 0x60
#define CTX_sp 0x68
#define CTX_pc 0x70




#define SAVE_REG(reg, offset, base) str reg, [base, #CTX_##offset]
#define LOAD_REG(reg, offset, base) ldr reg, [base, #CTX_##offset]

#define SAVE_CTX(_sr, _sp, reg)\
  SAVE_REG(x19, X19, reg);\
  SAVE_REG(x20, X20, reg);\
  SAVE_REG(x21, X21, reg);\
  SAVE_REG(x22, X22, reg);\
  SAVE_REG(x23, X23, reg);\
  SAVE_REG(x24, X24, reg);\
  SAVE_REG(x25, X25, reg);\
  SAVE_REG(x26, X26, reg);\
  SAVE_REG(x27, X27, reg);\
  SAVE_REG(x28, X28, reg)

#define LOAD_CTX(_sr, reg)\
  LOAD_REG(x19, X19, reg);\
  LOAD_REG(x20, X20, reg);\
  LOAD_REG(x21, X21, reg);\
  LOAD_REG(x22, X22, reg);\
  LOAD_REG(x23, X23, reg);\
  LOAD_REG(x24, X24, reg);\
  LOAD_REG(x25, X25, reg);\
  LOAD_REG(x26, X26, reg);\
  LOAD_REG(x27, X27, reg);\
  LOAD_REG(x28, X28, reg)

#else /* !__ASSEMBLER__ */

#define unsigned long long reg_t

//arm
#define lr x30	// link register 
#define fp x29	// frame pointer
#defune sp sp
#define pc pc
//#define sr spsr	//status register

typedef struct ctx {
  reg_t x19, x20, x21, x22, x23, x24, x25, x27, x28; //callee saved regs
  //reg_t fp, lr, sp;
  //reg_t pc
} ctx_t;


#endif

#endif
