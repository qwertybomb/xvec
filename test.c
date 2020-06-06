#include "xvec.h"
#include <stdio.h>

int main() {
    int *v = xvec_new(int,9765);
    xvec_push(v, 1);
    xvec_push(v, 1);
    xvec_push(v, 1, 7, 8, 9, 10, 11);
    v[2] = 2;
    xvec_insert(v, 0, 12, 3, 5, 6, 7, 8, 9, 10);
    xvec_insert(v, 0, 1, 2, 3, 4);
    xvec_insert(v, 0, 1, 2, 3, 4);
    xvec_insert(v, 0, 1, 2, 3, 4);
    xvec_insert(v, 0, 1, 2, 3, 4);
    xvec_pop(v);
    xvec_insert(v, xvec_len(v), 1, 2, 3, 4);
    xvec_remove_range(v,xvec_len(v)-2,xvec_len(v)-1);
    xvec_remove(v,xvec_len(v)-2);
    xvec_reserve(v,70);
    xvec_resize(v,3453);
    xvec_resize(v,32);
    int* t= xvec_copy(v);
    t[31] = -1;
    printf("%zu\n", xvec_cap(v));
    for (int i = 0; i < xvec_len(v); ++i) {
        printf("%d\n", v[i]);
    }
    printf("%d\n",t[31]);
    xvec_free(v);
}