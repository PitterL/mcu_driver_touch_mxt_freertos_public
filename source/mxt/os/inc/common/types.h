/*
 * types.h
 *
 * Created: 2/27/2016 9:11:13 PM
 *  Author: pitter.liao
 */ 


#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>
#include <stdbool.h>

#define __bitwise
#define __force
#define __attribute_const__

typedef uint8_t  u8;
typedef int8_t  s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
#ifdef CONFIG_64BIT
typedef uint64_t u64;
typedef int64_t s64;
#endif

typedef int8_t   __s8;
typedef uint8_t  __u8;
typedef int16_t  __s16;
typedef uint16_t __u16;
typedef int32_t  __s32;
typedef uint32_t __u32;
#ifdef CONFIG_64BIT
typedef int64_t  __s64;
typedef uint64_t __u64;
#endif

typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
#ifdef CONFIG_64BIT
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;
#endif

typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;

typedef struct {
	int counter;
} atomic_t;

#ifdef CONFIG_64BIT
typedef struct {
	long counter;
} atomic64_t;
#endif

struct list_head {
	struct list_head *next, *prev;
};

struct hlist_head {
	struct hlist_node *first;
};

struct hlist_node {
	struct hlist_node *next, **pprev;
};

#endif /* TYPES_H_ */