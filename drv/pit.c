/* Programable Interval Timer (PIT) driver for Intel 8253 */
#include <common.h>
#include <dev/i8253reg.h>
#include <dev/isareg.h>
#include <pci.h>
#include <interrupt.h>
#include <klog.h>
#include <timer.h>
#include <spinlock.h>
#include <sysinit.h>
#include <devclass.h>

typedef struct pit_state {
  resource_t *regs;
  spin_t lock;
  intr_handler_t intr_handler;
  timer_t timer;
  uint64_t period_frac;        /* period as a fraction of a second */
  uint16_t period_cntr;        /* period as PIT counter value */
  volatile uint16_t last_cntr; /* last seen counter value */
  volatile bool overflow;      /* counter overflow detected? */
  bintime_t time;              /* last time measured by the timer */
} pit_state_t;

#define inb(addr) bus_read_1(pit->regs, (addr))
#define outb(addr, val) bus_write_1(pit->regs, (addr), (val))

static void pit_set_frequency(pit_state_t *pit, uint16_t period) {
  assert(spin_owned(&pit->lock));

  outb(TIMER_MODE, TIMER_SEL0 | TIMER_16BIT | TIMER_RATEGEN);
  outb(TIMER_CNTR0, period & 0xff);
  outb(TIMER_CNTR0, period >> 8);
  pit->period_cntr = period;
}

static uint16_t pit_get_counter(pit_state_t *pit) {
  assert(spin_owned(&pit->lock));

  outb(TIMER_MODE, TIMER_SEL0 | TIMER_LATCH);
  uint16_t value = 0;
  value |= inb(TIMER_CNTR0);
  value |= inb(TIMER_CNTR0) << 8;
  return pit->period_cntr - value;
}

static intr_filter_t pit_intr(void *data) {
  pit_state_t *pit = data;

  WITH_SPIN_LOCK (&pit->lock) {
    bintime_add_frac(&pit->time, pit->period_frac);
    pit->overflow = false;
    pit->last_cntr = pit_get_counter(pit);
  }
  tm_trigger(&pit->timer);
  return IF_FILTERED;
}

static device_t *device_of(timer_t *tm) {
  return tm->tm_priv;
}

static int timer_pit_start(timer_t *tm, unsigned flags, const bintime_t start,
                           const bintime_t period) {
  assert(flags & TMF_PERIODIC);
  assert(!(flags & TMF_ONESHOT));

  device_t *dev = device_of(tm);
  pit_state_t *pit = dev->state;

  pit->time = getbintime();
  pit->period_frac = period.frac;
  uint16_t counter = bintime_mul(period, TIMER_FREQ).sec;
  WITH_SPIN_LOCK (&pit->lock) {
    pit_set_frequency(pit, counter);
    pit->last_cntr = pit_get_counter(pit);
  }
  bus_intr_setup(dev, 0, &pit->intr_handler);
  return 0;
}

static int timer_pit_stop(timer_t *tm) {
  device_t *dev = device_of(tm);
  pit_state_t *pit = dev->state;

  bus_intr_teardown(dev, &pit->intr_handler);
  return 0;
}

static bintime_t timer_pit_gettime(timer_t *tm) {
  device_t *dev = device_of(tm);
  pit_state_t *pit = dev->state;
  uint16_t cntr = 0;
  bintime_t bt;
  WITH_SPIN_LOCK (&pit->lock) {
    uint16_t last_cntr = pit->last_cntr;
    bool overflow = pit->overflow;
    bt = pit->time;
    cntr = pit_get_counter(pit);
    pit->last_cntr = cntr;
    if (cntr < last_cntr || overflow) {
      cntr += pit->period_cntr;
      pit->overflow = true;
    }
  }
  bintime_t offset = bintime_mul(tm->tm_min_period, cntr);
  bintime_add_frac(&bt, offset.frac);
  return bt;
}

static int pit_attach(device_t *dev) {
  assert(dev->parent->bus == DEV_BUS_PCI);

  pit_state_t *pit = dev->state;

  pit->regs =
    bus_alloc_resource(dev, RT_ISA, 0, IO_TIMER1, IO_TIMER1 + IO_TMRSIZE - 1,
                       IO_TMRSIZE, RF_ACTIVE);
  assert(pit->regs != NULL);

  pit->lock = SPIN_INITIALIZER(0);
  pit->intr_handler = INTR_HANDLER_INIT(pit_intr, NULL, pit, "i8254 timer", 0);

  pit->timer = (timer_t){
    .tm_name = "i8254",
    .tm_flags = TMF_PERIODIC,
    .tm_frequency = TIMER_FREQ,
    .tm_min_period = HZ2BT(TIMER_FREQ),
    .tm_max_period = bintime_mul(HZ2BT(TIMER_FREQ), 65536),
    .tm_start = timer_pit_start,
    .tm_stop = timer_pit_stop,
    .tm_gettime = timer_pit_gettime,
    .tm_priv = dev,
  };

  tm_register(&pit->timer);

  return 0;
}

static driver_t pit_driver = {
  .desc = "i8254 PIT driver",
  .size = sizeof(pit_state_t),
  .attach = pit_attach,
};

extern device_t *gt_pci;

static void pit_init(void) {
  (void)make_device(gt_pci, &pit_driver);
}

SYSINIT_ADD(pit, pit_init, DEPS("rootdev"));
DEVCLASS_ENTRY(root, pit_driver);
