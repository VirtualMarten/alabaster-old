#include "ae/array.h"
#include <stdlib.h>

arr_t arr_new(uint8_t item_size, size_t array_size, uint8_t expansion) {
    if (!(item_size && array_size)) return NULL;
    arr_header_t *h = malloc(sizeof(arr_header_t) + array_size * item_size);
    *h = (arr_header_t){
        .size = array_size,
        .length = 0,
        .item_size = item_size,
        .expansion = expansion,
        ._rtn = malloc(item_size)
    };
    return ((char*)h) + sizeof(arr_header_t);
}

void _arr_delete(arr_t *arr) {
    arr_header_t *h = (arr_header_t*)((uint8_t*)*arr - sizeof(arr_header_t));
    free(h->_rtn);
    free(h);
    *arr = NULL;
}

arr_t _arr_resize(arr_t *arr, size_t new_size) {
    if (!(arr && *arr)) return NULL;
    if (!new_size) _arr_delete(arr);
    arr_header_t *h = (arr_header_t*)((uint8_t*)*arr - sizeof(arr_header_t));
    size_t old_size = h->size;
    h->size = new_size;
    h = realloc(h, sizeof(arr_header_t) + new_size * h->item_size);
    *arr = ((char*)h) + sizeof(arr_header_t);
    if (old_size < new_size)
        memset((*arr) + old_size * h->item_size, 0, (new_size - old_size) * h->item_size);
    return *arr;
}

void _arr_set(arr_t *arr, int i, void *item) {
    if (!(arr && *arr)) return;
    if (i < 0) i = arr_header(*arr)->length + i;
    if ((size_t)i >= arr_header(*arr)->size)
        _arr_resize(arr, arr_header(*arr)->size + arr_header(*arr)->expansion);
    memcpy((uint8_t*)*arr + i * arr_header(*arr)->item_size, item, arr_header(*arr)->item_size);
    if (arr_header(*arr)->length <= (size_t)i) arr_header(*arr)->length = i + 1;
}

int _arr_add(arr_t *arr, void *item) {
    if (!(arr && *arr)) return -1;
    _arr_set(arr, arr_header(*arr)->length, item);
    return arr_header(*arr)->length - 1;
}

void* _arr_remove(arr_t *arr, int i) {
    if (!(arr && *arr)) return NULL;
    size_t l = arr_header(*arr)->length;
    uint8_t s = arr_header(*arr)->item_size;
    i = _arr_index(i, l);
    int is = i * s;
    memcpy(arr_header(*arr)->_rtn, (uint8_t*)*arr + is, s);
    memmove((uint8_t*)*arr + is, (uint8_t*)*arr + is + s, (l - i) * s);
    if (l > 1 && l - 1 <= arr_header(*arr)->size - arr_header(*arr)->expansion)
        _arr_resize(arr, arr_header(*arr)->size - arr_header(*arr)->expansion);
    arr_header(*arr)->length -= 1;
    return arr_header(*arr)->_rtn;
}

int _arr_find(arr_t arr, void *item, uint32_t offset) {
    if (!arr) return -1;
    uint32_t i = 0;
    const size_t l = arr_header(arr)->length;
    const uint8_t s = arr_header(arr)->item_size;
    for (; i < l; i++) {
        if ((uint8_t*)arr + i * s == item) {
            if (offset == 0) return i;
            else --offset;
        }
    }
    return -1;
}

arr_t arr_sort(arr_t arr, arr_sort_func_t compare) {
    if (!arr) return NULL;
    qsort(arr, arr_header(arr)->length, arr_header(arr)->item_size, compare);
    return arr;
}