#ifndef _ASM_GENERIC_BITOPS_ATOMIC_FLAG_H_
#define _ASM_GENERIC_BITOPS_ATOMIC_FLAG_H_

#ifdef CONFIG_SMP
#  define _atomic_spin_lock_irqsave(l,f)	(f = f)
#  define _atomic_spin_unlock_irqrestore(l,f)	(f = f)
#else
#  define _atomic_spin_lock_irqsave(l,f) do { local_irq_save(f); } while (0)
#  define _atomic_spin_unlock_irqrestore(l,f) do { local_irq_restore(f); } while (0)
#endif

/*
 * NMI events can occur at any time, including when interrupts have been
 * disabled by *_irqsave().  So you can get NMI events occurring while a
 * *_bit function is holding a spin lock.  If the NMI handler also wants
 * to do bit manipulation (and they do) then you can get a deadlock
 * between the original caller of *_bit() and the NMI handler.
 *
 * by Keith Owens
 */

#define set_flag(__mask, __addr) do {				\
	unsigned long __flags;	\
	_atomic_spin_lock_irqsave((__addr), __flags);	\
	*(__addr) |= (__mask); \
	_atomic_spin_unlock_irqrestore((__addr), __flags);	\
} while(0)

#define test_flag(__mask, __addr) ({				\
((*(__addr)) & (__mask)) != 0; })

#define clear_flag(__mask, __addr) do {				\
	unsigned long __flags;	\
	_atomic_spin_lock_irqsave((__addr), __flags);	\
	*(__addr) &= ~(__mask);	\
	_atomic_spin_unlock_irqrestore((__addr), __flags);	\
} while(0)

#define test_flag_8bit(__mask, __addr) test_flag(__mask, __addr)

static inline void set_and_clr_flag(unsigned long mask_s, unsigned long mask_c, volatile unsigned long *addr)
{
	unsigned long *p = ((unsigned long *)addr);
	unsigned long flags;

	_atomic_spin_lock_irqsave(p, flags);
	*p  &= ~mask_c;
	*p  |= mask_s;
	_atomic_spin_unlock_irqrestore(p, flags);
}

/**
 * change_bit - Toggle a bit in memory
 * @nr: Bit to change
 * @addr: Address to start counting from
 *
 * change_bit() is atomic and may not be reordered. It may be
 * reordered on other architectures than x86.
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static inline void change_flag(unsigned long mask, volatile unsigned long *addr)
{
	unsigned long *p = ((unsigned long *)addr);
	unsigned long flags;

	_atomic_spin_lock_irqsave(p, flags);
	*p ^= mask;
	_atomic_spin_unlock_irqrestore(p, flags);
}

/**
 * test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It may be reordered on other architectures than x86.
 * It also implies a memory barrier.
 */
static inline unsigned long test_and_set_flag(unsigned long mask, volatile unsigned long *addr)
{
	unsigned long *p = ((unsigned long *)addr);
	unsigned long old;
	unsigned long flags;

	_atomic_spin_lock_irqsave(p, flags);
	old = *p;
	*p = old | mask;
	_atomic_spin_unlock_irqrestore(p, flags);

	return (old & mask) != 0;
}

/**
 * test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It can be reorderdered on other architectures other than x86.
 * It also implies a memory barrier.
 */
static inline unsigned long test_and_clear_flag(unsigned long mask, volatile unsigned long *addr)
{
	unsigned long *p = ((unsigned long *)addr);
	unsigned long old;
	unsigned long flags;

	_atomic_spin_lock_irqsave(p, flags);
	old = *p;
	*p = old & ~mask;
	_atomic_spin_unlock_irqrestore(p, flags);

	return (old & mask) != 0;
}

static inline unsigned long test_flag_set_and_clear(unsigned long mask_s, unsigned long mask, volatile unsigned long *addr)
{
	unsigned long *p = ((unsigned long *)addr);
	unsigned long old;
	unsigned long flags;

	_atomic_spin_lock_irqsave(p, flags);
	old = *p;
	*p = (old & ~mask) | mask_s;
	_atomic_spin_unlock_irqrestore(p, flags);

	return (old & mask) != 0;
}


/**
 * test_and_change_bit - Change a bit and return its old value
 * @nr: Bit to change
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
static inline unsigned long test_and_change_flag(unsigned long mask, volatile unsigned long *addr)
{
	unsigned long *p = ((unsigned long *)addr);
	unsigned long old;
	unsigned long flags;

	_atomic_spin_lock_irqsave(p, flags);
	old = *p;
	*p = old ^ mask;
	_atomic_spin_unlock_irqrestore(p, flags);

	return (old & mask) != 0;
}

#endif /* _ASM_GENERIC_BITOPS_ATOMIC_FLAG_H */
