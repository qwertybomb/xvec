#ifndef XVEC_C_XVEC_H
#define XVEC_C_XVEC_H

#include <stdlib.h>
#include <string.h>

/*needed for increasing capacity*/
static inline size_t upper_power_of_two(size_t v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    //only needed for 64 bit mode
    if (sizeof (size_t) == 8) v |= v >> 32;

    v++;
    return v;
}

typedef struct xvec_base { size_t cap, len; } xvec_base;
#define XVEC_ALEN(a) (sizeof(a) / sizeof(*a))

static inline size_t *xvec_len_ptr(void *v) {
    return &((xvec_base *) v - 1)->len;
}
static inline size_t *xvec_cap_ptr(void *v) {
    return &((xvec_base *) v - 1)->cap;
}
static inline size_t xvec_len(void* v) {
    return ((xvec_base *) v - 1)->len;
}
static inline size_t xvec_cap(void* v) {
    return ((xvec_base *) v - 1)->cap;
}


/* how to use
 * T = type
 * ... = items to initialize the array with
*/
#define xvec_new(T, ...)                                                              \
({                                                                                    \
    const T __args[] = {__VA_ARGS__};                                                 \
    const  size_t initial_size = 16+XVEC_ALEN(__args);                                \
    xvec_base *_v = (xvec_base*)malloc(sizeof(xvec_base) + sizeof(T) * initial_size); \
    _v->cap = initial_size;                                                           \
    _v->len = XVEC_ALEN(__args);                                                      \
    T* v = (T*)(_v+1);                                                                \
    memcpy(v,__args,sizeof(__args));                                                  \
    v;                                                                                \
})                                                                                    \
/* how to use
 * v = container
 * size = the size to resize to
*/
#define xvec_resize(v, size)                         \
({                                                   \
    const size_t _size = size;                       \
    size_t* _len = xvec_len_ptr(v);                  \
    size_t* _cap = xvec_cap_ptr(v);                  \
    if(_size > *_len) {                              \
        xvec_reserve(v,_size);                       \
        /*clear memory after resizing*/              \
        memset(v+*_len,0, (*_cap-*_len)*sizeof(*v)); \
        _len = xvec_len_ptr(v);                      \
    }                                                \
    *_len = _size;                                   \
    v;                                               \
})

#define xvec_reserve(v, size)                                                                                          \
({                                                                                                                     \
    const size_t s = size;                                                                                             \
    size_t* _cap = xvec_cap_ptr(v);                                                                                    \
    if(s > *_cap) {                                                                                                    \
        *_cap = s;                                                                                                     \
        v = (__typeof__(v))((char*)realloc((xvec_base*)v-1,sizeof(xvec_base) + sizeof(*v)*(*_cap))+sizeof(xvec_base)); \
    }                                                                                                                  \
    v;                                                                                                                 \
})

#define xvec_free(v) free((xvec_base*)v-1)

#define xvec_pop(v) ({v[(*xvec_len_ptr(v))--];})

/*how to use
 * v = container
 * ... = items to insert
*/
#define xvec_push(v, ...)                                           \
({                                                                  \
    const __typeof__(*v) __args[] = {__VA_ARGS__};                  \
    size_t* _len = xvec_len_ptr(v);                                 \
    size_t* _cap = xvec_cap_ptr(v);                                 \
    if(*_len + XVEC_ALEN(__args) >= *_cap) {                        \
       xvec_reserve(v,upper_power_of_two(*_len+XVEC_ALEN(__args))); \
       _len = xvec_len_ptr(v);                                      \
    }                                                               \
    for(size_t i = 0; i < XVEC_ALEN(__args); ++i)  {                \
        v[(*_len)++] = __args[i];                                   \
    }                                                               \
    v;                                                              \
})

/*how to use
 * v = container
 * index = where to insert
 * ... = items to insert
*/
#define xvec_insert(v, index, ...)                                  \
({                                                                  \
    const size_t _n = index;                                        \
    const __typeof__(*v) __args[] = {__VA_ARGS__};                  \
    size_t* _len = xvec_len_ptr(v);                                 \
    size_t* _cap = xvec_cap_ptr(v);                                 \
    if(*_len + XVEC_ALEN(__args) >= *_cap) {                        \
       xvec_reserve(v,upper_power_of_two(*_len+XVEC_ALEN(__args))); \
       _len = xvec_len_ptr(v);                                      \
    }                                                               \
    memcpy(v+_n+XVEC_ALEN(__args),v+_n,                             \
    sizeof(*v)*(*_len-_n));                                         \
    memcpy(v+_n,&__args[0],                                         \
    XVEC_ALEN(__args)*sizeof(*v));                                  \
    *_len += XVEC_ALEN(__args);                                     \
    v;                                                              \
})
/* how to use
 * v = container
 * index = where to remove
*/
#define xvec_remove(v, index)                              \
({                                                         \
    const size_t _index = index;                           \
    size_t *_len = xvec_len_ptr(v);                        \
    memcpy(v+_index,v+_index+1,sizeof(*v)*(*_len-_index)); \
    (*_len)--;                                             \
    v;                                                     \
})
/* how to use
 * v = container
 * first = the first item to be removed
 * last = the last item to be removed
*/
#define xvec_remove_range(v, first, last)                \
({                                                       \
    const size_t _first = first;                         \
    const size_t _last = last;                           \
    const size_t _n = _last-_first+1;                    \
    size_t *_len  = xvec_len_ptr(v);                     \
    memcpy(v+_first,v+_last+1,sizeof(*v)*(*_len-_last)); \
    *_len-=_n;                                           \
    v;                                                   \
})

/* how to use
 * v = container
*/
#define xvec_copy(v)                              \
({                                                \
    __typeof__(v) new_xvec = xvec_new(typeof(v)); \
    xvec_resize(new_xvec,xvec_len(v));            \
    memcpy(new_xvec,v,xvec_len(v)*sizeof(*v));    \
    new_xvec;                                     \
})

#endif //XVEC_C_XVEC_H
