#include "mock.h"

extern void ctx_switch(thread_t *from, thread_t *to);

int main(int argc, char** argv)
{
  thread_t a, b;
  thread_t *ap = &a, *bp = &b;

  while (1)
    ctx_switch(ap, bp);

  return 0;
}
