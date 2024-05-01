#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "dynamic_array.h"

#include "util.h"

#define DA_FROM_DATA_PTR(p) ((DA *)((char *)p - sizeof(DA)))

typedef struct {
    unsigned int iterindex;
    unsigned int itemsize;
    unsigned int capacity;
    unsigned int count;
    char data[];
} DA;

void
_da_create(void **da, size_t itemsize, size_t capacity)
{
    assert(da);
    assert(itemsize);
    assert(capacity);
    DA *a;
    a = malloc(sizeof(DA) + (itemsize * capacity));
    if (a) {
        a->itemsize = itemsize;
        a->capacity = capacity;
        a->iterindex = a->count = 0;
    }
    *da = a->data;
}

void
_da_destroy(void **da)
{
    DA *a;
    a = DA_FROM_DATA_PTR(*da);
    free(a);
    *da = NULL;
}

void
_da_append(void **da, void *item)
{
    DA *a;
    a = DA_FROM_DATA_PTR(*da);
    if (a->count == a->capacity) {
        a->capacity *= 2;
        a = erealloc(a, sizeof(DA) + a->capacity * a->itemsize);
        *da = a->data;
    }
    memcpy((a->data + a->count * a->itemsize), item, a->itemsize);
    a->count++;
}

size_t
_da_len(void *da)
{
    DA *a;
    a = DA_FROM_DATA_PTR(da);
    return a->count;
}

void *
_da_last(void *da)
{
    DA *a;
    a = DA_FROM_DATA_PTR(da);
    return (a->data + (a->count - 1) * a->itemsize);
}

