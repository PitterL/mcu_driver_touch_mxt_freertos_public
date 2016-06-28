/*
 * vector.h
 *
 * Created: 3/22/2016 3:32:13 PM
 *  Author: pitter.liao
 */ 


#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdbool.h>

typedef void *vector_element;
typedef vector_element vector_iterator;

enum{V_HEAD, V_TAIL, V_FIRST, V_LAST, V_BEGIN, V_END, NUM_VEC_ITER};

struct vector{
	int element_size;
	vector_iterator iter[NUM_VEC_ITER];

	vector_iterator (*push_back)(struct vector *, const vector_element *);
	vector_iterator (*push_front)(struct vector *, const vector_element *);
	vector_iterator (*get)(struct vector *, int);
	vector_iterator (*dec)(struct vector *, vector_iterator);
	vector_iterator (*inc)(struct vector *, vector_iterator);
	vector_iterator (*begin)(struct vector *);
	vector_iterator (*end)(struct vector *);
	int (*count)(struct vector *);
	bool (*empty)(struct vector *);
	void (*reset)(struct vector *);
	void *(*destory)(struct vector *);
};

static inline vector_iterator vector_push_front(struct vector *vec, const vector_element *val)
{	
	if (vec->iter[V_BEGIN] != vec->iter[V_HEAD]) {
		vec->iter[V_BEGIN] = vec->iter[V_BEGIN] - vec->element_size;
		memcpy(vec->iter[V_BEGIN], val, vec->element_size);
		
		return vec->iter[V_BEGIN];
	}
	
	return NULL;
}

static inline vector_iterator vector_push_back(struct vector *vec, const vector_element *val)
{
	if (vec->iter[V_END] != vec->iter[V_TAIL]) {
		vec->iter[V_END] += vec->element_size;
		memcpy(vec->iter[V_END], val, vec->element_size);
		return vec->iter[V_END];
	}
	
	return NULL;
}

static inline vector_iterator vector_get(struct vector *vec, int index)
{
	return vec->begin(vec) + vec->element_size * index;
}

static inline vector_iterator vector_inc(struct vector *vec, vector_iterator iter)
{
	return iter + vec->element_size;
}

static inline vector_iterator vector_dec(struct vector *vec, vector_iterator iter)
{
	return iter - vec->element_size;
}

static inline vector_iterator vector_begin(struct vector *vec)
{
	return vec->iter[V_BEGIN];
}

static inline vector_iterator vector_end(struct vector *vec)
{
	return vec->iter[V_END];
}

static inline int vector_count(struct vector *vec)
{
	return (vec->end(vec) - vec->begin(vec)) / vec->element_size + 1;
}

static inline bool vector_empty(struct vector *vec)
{
	return vec->end(vec) < vec->begin(vec);
}

static inline void vector_reset(struct vector *vec)
{
	vec->iter[V_BEGIN] = vec->iter[V_FIRST];
	vec->iter[V_END] = vec->iter[V_LAST];
}

static inline void *vector_destory(struct vector *vec)
{
	if (vec)
		kfree(vec);

	return NULL;
}

/* -----------memory layour----------------
	 |->m_start								m_end->|
	 V_HEAD			  V_BEGIN			V_END			V_TAIL
	 |->  rsv begin ->|			
					  |->valid element
										|->empty space
*/
static inline struct vector * create_vector(int element_size, int max_depth, int rsv_begin)
{
	int size;

	struct vector *vec;
	vector_iterator iter;

	size = sizeof(*vec);
	size += element_size * max_depth;
	vec = kzalloc(size, GFP_KERNEL);
	if (vec) {
		iter = (vector_iterator)(vec + 1);
		vec->iter[V_HEAD] = iter;
		vec->iter[V_TAIL] = iter + (max_depth - 1 )* element_size;	//last valid space
		vec->iter[V_FIRST] = vec->iter[V_BEGIN] = iter + rsv_begin * element_size;
		vec->iter[V_LAST] = vec->iter[V_END] = vec->iter[V_BEGIN] - element_size;	//last used space

		vec->element_size = element_size;
		vec->push_front = vector_push_front;
		vec->push_back = vector_push_back;
		vec->get = vector_get;
		vec->inc = vector_inc;
		vec->dec = vector_dec;
		vec->begin = vector_begin;
		vec->end = vector_end;
		vec->reset = vector_reset;
		vec->count = vector_count;
		vec->empty = vector_empty;
		vec->destory = vector_destory;
	}

	return vec;
};

typedef struct vector * vector_t;

#endif /* VECTOR_H_ */