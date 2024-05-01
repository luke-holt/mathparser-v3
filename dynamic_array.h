#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdlib.h>

#define da_create(da)         _da_create((void *)&da, sizeof(*da), 1)
#define da_destroy(da)        _da_destroy((void *)&da)
#define da_append(da, item)   _da_append((void *)&da, item)
#define da_pop(da)            _da_pop(da)
#define da_len(da)            _da_len(da)
#define da_last(da)           _da_last(da);

void _da_create(void **da, size_t itemsize, size_t capacity);
void _da_destroy(void **da);
void _da_append(void **da, void *item);
size_t _da_len(void *da);
void *_da_last(void *da);

#endif // DYNAMIC_ARRAY_H

