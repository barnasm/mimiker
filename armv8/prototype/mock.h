#ifndef _MOCK_H_
#define _MOCK_H_

#include "thread.h"
#include "spinlock.h"
#include "interrupt.h"
#include "pcpu.h"
#include "pmap.h"
#include "sched.h"
#include "vm_map.h"
#include "common.h"

thread_t *thread_self(void) {
  return PCPU_GET(curthread);
}

#define spin_recurse_p(s) ((s)->s_type & LK_RECURSE)
bool spin_owned(spin_t *s) {
  return (s->s_owner == thread_self());
}

struct vm_map {
  TAILQ_HEAD(vm_map_list, vm_segment) entries;
  size_t nentries;
  pmap_t *pmap;
  mtx_t mtx; /* Mutex guarding vm_map structure and all its entries. */
};


void mips_intr_disable(void) {
  asm volatile("msr DAIFclr, #3; isb");
}

void mips_intr_enable(void) {
  asm volatile("msr DAIFset, #3; isb");
}

void intr_disable(void) {
  mips_intr_disable();
  thread_self()->td_idnest++;  
}

void spin_unlock(spin_t *s) {
  //assert(spin_owned(s));

  if (s->s_count > 0) {
    //assert(spin_recurse_p(s));
    s->s_count--;
  } else {
    s->s_owner = NULL;
    s->s_lockpt = NULL;
  }
   intr_enable();
}

 __wired_data pcpu_t _pcpu_data[1] = {{}};

void _spin_lock(spin_t *s, const void *waitpt){
  intr_disable();

  if (spin_owned(s)) {
    //if (!spin_recurse_p(s))
      //panic("Spin lock %p is not recursive!", s);
    s->s_count++;
    return;
  }
  //assert(s->s_owner == NULL);
  s->s_owner = thread_self();
  s->s_lockpt = waitpt;
}

void intr_enable(void) {
  //assert(intr_disabled());
  thread_t *td = thread_self();
  td->td_idnest--;
  if (td->td_idnest == 0)
    mips_intr_enable();
}


void vm_map_activate(vm_map_t *map) {
  //SCOPED_NO_PREEMPTION();
  PCPU_SET(uspace, map);
  //pmap_activate(map ? map->pmap : NULL);
}
#endif 

