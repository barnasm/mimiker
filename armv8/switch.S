//#include <mips/asm.h>
//#include <mips/m32c0.h>
#include <arm/ctx.h>
//#include <mips/pcpu.h>
//#include <mips/regdef.h>

//#include "assym.h"
//#include "test/mock.h"
	
#define tmp0  x9
#define tmp1 x10
	
#define save0 x19	
#define save1 x20

;; .extern intr_disable;
;; .extern intr_enable
;; .extern _spin_lock
;; .extern _spin_unlock
;; .extern vm_map_activate
	
//        .set noreorder

.global ctx_switch
.local  ctx_resume
.local  ctx_save

#
# void ctx_switch(thread_t *from, thread_t *to)
#

//_TEXT_SECTION_NAMED(name);
.pushsection .text, "ax", @progbits;
.globl	ctx_switch;
.balign	4;
//.ent	ctx_switch;
ctx_switch:

	// mask interrupts
	msr	DAIFclr, #3	
	isb	//Instruction Synchronization Barrier
	
	// # don't save context of @from thread if user did not provide one
	cbnz	X0, ctx_save
	b	ctx_resume
	
ctx_save:
	//add	tmp1, x0, TD_KCTX         //load context pointer

	//save context of @from thread
	mrs 	tmp0, spsr_el3
	//str	tmp0, [tmp1, CTX_SR]
	
	SAVE_CTX(tmp0, tmp1, sp)
	
	mov	save0, x0                  // save @from pointer	

        //  disable interrupts so interlock on td_spin can be done safely
        mov	save1, x1                  // save @to thread pointer
        //bl	intr_disable

        //  release @from thread spin lock
        //add	x0, save0, TD_SPIN         // 1st arg - @from spin lock
	//bl	spin_unlock
        

ctx_resume:
	mov	save0, x1                  //save @to thread pointer
	// update curthread pointer to reference @to thread

	//ldr	tmp0, _pcpu_data
	//str	save1, [tmp0, PCPU_CURTHREAD]

	// check for TDF_NEEDLOCK flag
        //ldr	tmp0, [save1, TD_FLAGS]
        //and	tmp0, tmp0, #TDF_NEEDLOCK
        //cbz	tmp0, 1f

	// acquire @to thread spin lock 
        //ldr	x1, ctx_resume           // 2nd arg - waiting point
        //add    	x0, save1, TD_SPIN        // 1st arg - @to spin lock
        //bl	_spin_lock

	// Enable interrupts finishing safe interlock on td_spin.
        //bl	intr_enable

	// switch user space if necessary
1:      //ldr	x0, [save1, TD_PROC]
        cbz	x0, 2f                  // switching to kernel thread ?

	//ldr	x0, [x0, P_USPACE]
2:      //bl	vm_map_activate

	// restore @to thread context
        //add	tmp1, save1, TD_KCTX
        LOAD_CTX(tmp0, tmp1)

	// restore status register with updated interrupt mask
        //LOAD_REG(tmp0, SR, tmp1)
	//ldr	tmp0, [tmp1, CTX_SR] 
//         mfc0    t1, C0_SR
//         ext     t1, SR_IMASK_SHIFT, SR_IMASK_BITS
//         ins     t0, t1, SR_IMASK_SHIFT, SR_IMASK_BITS
//         mtc0    t0, C0_SR

	isb
        ret

.size	ctx_switch,.-ctx_switch;
.end	ctx_switch;
.popsection


	
# vim: sw=8 ts=8 et
