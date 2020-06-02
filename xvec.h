//
// Created by qwerty on 6/1/20.
//

#ifndef XVEC_C_XVEC_H
#define XVEC_C_XVEC_H
#include <stdlib.h>
#include <string.h>

#define X_NAME(prefix) JOIN(prefix,__LINE__)
#define JOIN(prefix, postfix) _JOIN(prefix,postfix)
#define _JOIN(prefix, postfix) prefix##postfix

static inline size_t upper_power_of_two(size_t v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    //only needed for 64 bit mode
    if (sizeof(size_t) == 8) v |= v >> 32;
    v++;
    return v;
}

#define xvec_base struct { size_t cap, len; }
#define XVEC_ALEN(a) (sizeof(a) / sizeof(*a))

#define xvec(T)               \
    struct X_NAME(xvec_##T) { \
        xvec_base;            \
        T data[];             \
    }
/* how to use
 * T = type
 * ... = items to initialize the array with
 */
#define xvec_new(T, ...)                                                             \
({                                                                                   \
    const T __args[] = {__VA_ARGS__};                                                \
    const  size_t initial_size = 16+XVEC_ALEN(__args);                               \
    struct X_NAME(xvec_##T) *v = malloc(sizeof(xvec(T)) + sizeof(T) * initial_size); \
    v->cap = initial_size;                                                           \
    v->len = XVEC_ALEN(__args);                                                      \
    for(size_t i =0; i < v->len; ++i)                                                \
        v->data[i] = __args[i];                                                      \
    v;                                                                               \
})                                                                                   \
/* how to use
 * v = container
 * size = the size to resize to
 */
#define xvec_resize(v,size)                                         \
({                                                                  \
    const size_t s = size;                                          \
    /*clear memory before resizing*/                                \
    if(s > v->len) {                                                \
        memset(v->data+v->len,0, (v->cap-v->len)*sizeof(*v->data)); \
    }                                                               \
    if(s > v->cap) {                                                \
        v->cap = s;                                                 \
        v = realloc(v,sizeof(xvec_base) + sizeof(*v->data)*v->cap); \
    }                                                               \
    v->len = s;                                                     \
    v;                                                              \
})

#define xvec_reserve(v, size)                                       \
({                                                                  \
    const size_t s = size;                                          \
    if(s > v->cap) {                                                \
        v->cap = s;                                                 \
        v = realloc(v,sizeof(xvec_base) + sizeof(*v->data)*v->cap); \
    }                                                               \
    v;                                                              \
})

#define xvec_free(v) free(v)

#define xvec_pop(v) ({v->data[--v->len];})

#define xvec_ref(v, i) (&v->data[i])
/* how to use
 * v = container
 * i = where
 */
#define xvec_at(v, i) (*(xvec_ref(v, i)))
/*how to use
 * v = container
 * ... = items to insert
 */
#define xvec_push(v, ...)                                          \
({                                                                 \
    const __typeof__(*v->data) __args[] = {__VA_ARGS__};           \
    if(v->len + XVEC_ALEN(__args) >= v->cap) {                     \
       v->cap = upper_power_of_two(v->len+XVEC_ALEN(__args));      \
       v = realloc(v,sizeof(xvec_base) + sizeof(*v->data)*v->cap); \
    }                                                              \
    for(size_t i = 0; i < XVEC_ALEN(__args); ++i)  {               \
        v->data[v->len++] = __args[i];                             \
    }                                                              \
    v;                                                             \
})

/*how to use
 * v = container
 * index = where to insert
 * ... = items to insert
 */
#define xvec_insert(v, index, ...)                                 \
({                                                                 \
    const size_t n = index;                                        \
    const __typeof__(*v->data) __args[] = {__VA_ARGS__};           \
    if(v->len + XVEC_ALEN(__args) >= v->cap) {                     \
       v->cap = upper_power_of_two(v->len+XVEC_ALEN(__args));      \
       v = realloc(v,sizeof(xvec_base) + sizeof(*v->data)*v->cap); \
    }                                                              \
    memcpy(v->data+n+XVEC_ALEN(__args),v->data+n,                  \
    sizeof(*v->data)*(v->len-n));                                  \
    memcpy(v->data+n,&__args[0],                                   \
    XVEC_ALEN(__args)*sizeof(*v->data));                           \
    v->len+=XVEC_ALEN(__args);                                     \
    v;                                                             \
})
/* how to use
 * v = container
 * index = where to remove
 */
#define xvec_remove(v,index)                                                  \
({                                                                            \
    const size_t _index = index;                                              \
    memcpy(v->data+_index,v->data+_index+1,sizeof(*v->data)*(v->len-_index)); \
    v->len--;                                                                 \
    v;                                                                        \
})
/* how to use
 * v = container
 * first = the first item to be removed
 * last = the last item to be removed
 */
#define xvec_remove_range(v,first,last)                                       \
({                                                                            \
    const size_t _first = first;                                              \
    const size_t _last = last;                                                \
    const size_t n = _last-_first+1;                                          \
    memcpy(v->data+_first,v->data+_last+1,sizeof(*v->data)*(v->len-_last));   \
    v->len-=n;                                                                \
    v;                                                                        \
})
#endif //XVEC_C_XVEC_H
