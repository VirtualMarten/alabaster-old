#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <stdint.h>
#include <string.h>
#include "math.h"

typedef struct arr_header {
    size_t size, length;
    uint8_t item_size;
    uint8_t expansion;
    void *_rtn;
    char data[];
} arr_header_t;

typedef void* arr_t;
typedef int (*arr_sort_func_t)(const void*, const void*);

#define arr(item1, ...) ({\
    __typeof__(item1) items[] = { item1, __VA_ARGS__ };\
    const uint8_t is = sizeof(items[0]);\
    const size_t l = sizeof(items) / is;\
    arr_t a = arr_new(is, l, 8);\
    memcpy(a, items, sizeof(items));\
    arr_header(a)->length = l;\
    a;\
})

arr_t arr_new(uint8_t item_size, size_t array_size, uint8_t expansion);

// Free array and all it's contents.
#define arr_delete(a) _arr_delete((arr_t*)&a)
// Reallocate array to new size.
#define arr_resize(a, new_size) _arr_resize((arr_t*)&a, new_size)
// Set the value of item at index.
#define arr_set(a, i, ...) ({ __typeof__(__VA_ARGS__) _item = __VA_ARGS__; _arr_set((arr_t*)&a, i, &_item); })
// Append a new item to array, resizing by 'expansion' if needed.
#define arr_add(a, ...) ({ __typeof__(__VA_ARGS__) _item = __VA_ARGS__; _arr_add((arr_t*)&a, &_item); })
// Get pointer header data of array.
#define arr_header(a) ((arr_header_t*)((int8_t*)a - sizeof(arr_header_t)))
// Get item at index as type.
#define arr_get_as(a, i, type) (*(type*)arr_get(a, i))
// Remove item from array at index, returning a temporary pointer to the item removed.
#define arr_remove(a, i) _arr_remove((arr_t*)&a, i)
// Remove item from array at index, returning the item as the type specified.
#define arr_remove_as(a, i, type) (*(type*)_arr_remove((arr_t*)&a, i))
// I don't know what this function does. Who even made this!?
#define arr_find(a, item, offset) _arr_find(a, &item, offset)

// Get the length of array (the item count).
static inline size_t arr_len(arr_t arr) {
    return arr ? arr_header(arr)->length : 0;
}

#define arr_length(s) arr_len(s)

// Get the amount of items allocated in the array.
static inline size_t arr_size(arr_t arr) {
    return arr ? arr_header(arr)->size : 0;
}

void _arr_delete(arr_t *arr);
arr_t _arr_resize(arr_t *arr, size_t new_size);
void _arr_set(arr_t *arr, int i, void *item);
int _arr_add(arr_t *arr, void *item);
void* _arr_remove(arr_t *arr, int i);
int _arr_find(arr_t arr, void *item, uint32_t offset);
arr_t arr_sort(arr_t arr, arr_sort_func_t compare);

static inline uint32_t _arr_index(int i, size_t l) {
    return ((i < 0) ? i + l : (size_t)i) % l;
}

// Get a pointer to the item at index.
// If 'i' is negative, it is equivalent to 'length - i'
static inline void* arr_get(arr_t arr, int i) {
    return arr ? (int8_t*)arr + _arr_index(i, arr_len(arr)) * arr_header(arr)->item_size : NULL;
}

static inline arr_t arr_swap(arr_t arr, size_t a, size_t b) {
    const uint8_t s = arr_header(arr)->item_size;
    memcpy(arr_header(arr)->_rtn, (int8_t*)arr + a * s, s);
    memcpy((int8_t*)arr + a * s, (int8_t*)arr + b * s, s);
    memcpy((int8_t*)arr + b * s, arr_header(arr)->_rtn, s);
    return arr;
}

static inline arr_t arr_shuffle(arr_t arr) {
    for (size_t i = 0; i < arr_len(arr); i++)
        arr_swap(arr, i, rrange(0, arr_len(arr) - 1 - i));
    return arr;
}

#endif