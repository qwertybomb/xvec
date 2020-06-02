#include "xvec.h"
#include <stdio.h>
int main() {
    xvec(int) *v = xvec_new(int);
    xvec_push(v, 1);
    xvec_push(v, 1);
    xvec_push(v, 1, 7, 8, 9, 10, 11);
    xvec_at(v, 2) = 2;
    xvec_insert(v, 0, 12, 3, 5, 6, 7, 8, 9, 10);
    xvec_insert(v, 0, 1, 2, 3, 4);
    xvec_insert(v, 0, 1, 2, 3, 4);
    xvec_insert(v, 0, 1, 2, 3, 4);
    xvec_insert(v, 0, 1, 2, 3, 4);
    xvec_pop(v);
    xvec_insert(v, v->len, 1, 2, 3, 4);
    xvec_remove_range(v,v->len-2,v->len-1);
    xvec_remove(v,v->len-2);
    xvec_reserve(v,70);
    printf("%zu\n", v->cap);
    for (int i = 0; i < v->len; ++i) {
        printf("%d\n", xvec_at(v, i));
    }

    xvec_free(v);
}