#ifdef __cplusplus
typedef unsigned long	pthread_t;
extern "C" {
#endif

#if !defined(__x86_64__) || __SIZEOF_POINTER__ != 8
# error "x86_64 Architecture required"
#endif

#if !defined(__clang__)
# error "Compile with clang >:("
#endif

#if !defined(__GNUC__)
# error "GNUC Required"
#endif

#ifndef _SSIZE_T
# define _SSIZE_T
typedef __typeof__(long signed int) ssize_t;
#endif

#ifndef _SIZE_T
# define _SIZE_T
typedef __typeof__(sizeof(int)) size_t;
#endif

#ifndef _UINTPTR_T
# define _UINTPTR_T
typedef __typeof__(sizeof(int)) uintptr_t;
#endif

#if !defined(__FEATURES)
# define __FEATURES
#define weak __attribute__((__weak__))
#define HIDDEN __attribute__((__visibility__("hidden")))
#define __weak_alias(old, new) \
	extern __typeof__(old) new __attribute__((__weak__, __alias__(#old)))
#if !defined(__always_inline)
# define __always_inline __attribute__((always_inline))
#endif
#endif

static HIDDEN __always_inline
uintptr_t	__get_tp(void)
{
	register uintptr_t	tp;
	__asm__ ("mov %%fs:0,%0" : "=r" (tp) );
	return tp;
}

struct __ptcb
{
	void (*__f)(void *);
	void *__x;
	struct __ptcb *__next;
};

struct __locale_map;

struct __locale_struct
{
	const struct __locale_map *_c[6];
};

typedef struct __locale_struct * locale_t;

struct __pthread
{
	struct __pthread *self;
#ifndef TLS_ABOVE_TP
	uintptr_t *dtv;
#endif
	struct __pthread *prev, *next;
	uintptr_t sysinfo;
#ifndef TLS_ABOVE_TP
#ifdef CANARY_PAD
	uintptr_t canary_pad;
#endif
	uintptr_t canary;
#endif
	int tid;
	int errno_val;
	volatile int detach_state;
	volatile int cancel;
	volatile unsigned char canceldisable, cancelasync;
	unsigned char tsd_used:1;
	unsigned char dlerror_flag:1;
	unsigned char *map_base;
	size_t map_size;
	void *stack;
	size_t stack_size;
	size_t guard_size;
	void *result;
	struct __ptcb *cancelbuf;
	void **tsd;
	struct
	{
		volatile void *volatile head;
		long off;
		volatile void *volatile pending;
	} robust_list;
	int h_errno_val;
	volatile int timer_id;
	locale_t locale;
	volatile int killlock[1];
	char *dlerror_buf;
	void *stdio_locks;
#ifdef TLS_ABOVE_TP
	uintptr_t canary;
	uintptr_t *dtv;
#endif
};

#if !defined(__EXIT_SUCCESS)
# define	__EXIT_SUCCESS	0
#endif

#if !defined(__EXIT_FAILURE)
# define	__EXIT_FAILURE	1
#endif

#ifndef __cplusplus
typedef struct __pthread * pthread_t;
#endif

#ifdef TLS_ABOVE_TP
#define __pthread_self() ((pthread_t)(__get_tp() - sizeof(struct __pthread) - TP_OFFSET))
#else
#define __pthread_self() ((pthread_t)__get_tp())
#endif

#if !defined(errno) || !defined(_ERRNO)
HIDDEN __attribute__((const)) static
__always_inline int	*__errno_location(void)
{ return &__pthread_self()->errno_val; }
#define errno (*__errno_location())
#define _ERRNO
#endif

static HIDDEN __always_inline
long	__syscall_r(unsigned long __r)
{
	if (__r > -4096UL)
	{
		errno = -__r;
		return (-1);
	}
	return (__r);
}

static HIDDEN __always_inline long	__syscall3(
	long __n, long __a1, long __a2, long __a3)
{
	register unsigned long	r;
	__asm__ __volatile__ (
		"syscall" : "=a"(r) : "a"(__n), "D"(__a1),
		"S"(__a2), "d"(__a3) : "rcx", "r11", "memory");
	return (__syscall_r(r));
}

#if !defined(__NR_write)
# define __NR_write	1
#endif

#ifndef CHAR_BIT
# define CHAR_BIT	8
#endif

HIDDEN __attribute__((const)) static size_t	__bufsize(const char *__restrict__ s)
{
#if !defined (__BUFFER_ALIAS_TYPES)
#define	__BUFFER_ALIAS_TYPES
	typedef unsigned long int __attribute__ ((__may_alias__))	t_bytemask;
	typedef unsigned long int __attribute__ ((__may_alias__))	t_word;
#endif
	register const uintptr_t	s0 = (uintptr_t)s;
	register const t_word		*w = (const t_word *)(((uintptr_t)s) & \
									-((uintptr_t)(sizeof(t_word))));
	register const t_word		m = ((t_word)-1 / 0xff) * 0x7f;
	register t_bytemask			mask;
	register t_word				wi;

	if (!s || !*s)
		return (0);
	wi = *w;
	mask = ~(((wi & m) + m) | wi | m) >> (
			CHAR_BIT * (s0 % sizeof(t_word)));
	if (mask)
		return (__builtin_ctzl(mask) / CHAR_BIT);
	wi = *++w;
	while (!((wi - (((t_word)-1 / 0xff) * 0x01))
			& ~wi & (((t_word)-1 / 0xff) * 0x80)))
		wi = *++w;
	wi = (wi - ((t_word)-1 / 0xff) * 0x01) & ~wi & ((t_word)-1 / 0xff) * 0x80;
	wi = (__builtin_ctzl(wi) / CHAR_BIT);
	return (((const char *)w) + wi - s);
}

__weak_alias(__bufsize, __BUFSIZE);

static HIDDEN ssize_t	__write_impl(int __fd, const void *__restrict__ __buf, size_t __size)
{
	return ((ssize_t)__syscall3(__NR_write, __fd, (long)__buf, (long)__size));
}

__weak_alias(__write_impl, write);

#if !defined(__STDOUT_FILENO)
# define __STDOUT_FILENO	1
#endif

HIDDEN static void	*__memcpy_impl(
	void *__restrict__ dst,
	const void *__restrict__ src,
	size_t n)
{
	register unsigned char			*d = dst;
	register const unsigned char	*s = src;

	for (; (uintptr_t)s % 4 && n; --n) *d++ = *s++;

	typedef unsigned int __attribute__((__may_alias__)) ui32;

	if ((uintptr_t)d % 4 == 0)
	{
		if (n & 8)
		{
			*(ui32 *)(d + 0) = *(ui32 *)(s + 0);
			*(ui32 *)(d + 4) = *(ui32 *)(s + 4);
			d += 8; s += 8;
		}
		if (n & 4)
		{
			*(ui32 *)(d + 0) = *(ui32 *)(s + 0);
			d += 4; s += 4;
		}
		if (n & 2)
		{
			*d++ = *s++;
			*d++ = *s++;
		}
		if (n & 1)
			*d = *s;
		return dst;
	}
	for (; n; --n)
		*d++ = *s++;
	return dst;
}

__weak_alias(__memcpy_impl, memcpy);

int	main(const int argc, char **argv, char *envp[])
{
	(void)argc; (void)argv; (void)envp;

	__attribute__((aligned(16)))
	const char _RAW[14] = {
		72, 101, 108, 108, 111, 44, 32,
		119, 111, 114, 108, 100, 33, 10
	};

	register const size_t _bytes = __BUFSIZE(_RAW);

	const char *_buf = (const char *)__builtin_alloca(_bytes);
	memcpy((void *)_buf, (const void *)_RAW, _bytes);

	register const ssize_t _written = write(
		__STDOUT_FILENO, _buf, _bytes);

	if (_written != (ssize_t)_bytes)
		return (__EXIT_FAILURE);
	
	return (__EXIT_SUCCESS);
}

#ifdef __cplusplus
}
#endif
