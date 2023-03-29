#ifndef __ASSET_H__
#define __ASSET_H__

#include "ae/str.h"
#include <stdbool.h>

typedef struct asset_type_s asset_type_t;

typedef struct {
    str_t path, name;
    asset_type_t *type;
    bool initialized;
    void **ptr;
} asset_t;

typedef int (*asset_init_t)(asset_t*);
typedef int (*asset_load_t)(asset_t*);
typedef void (*asset_free_t)(asset_t*);

struct asset_type_s {
    str_t name;
    asset_init_t init_callback;
    asset_load_t load_callback;
    asset_free_t free_callback;
};

static inline asset_type_t new_asset_type(const char *name, asset_init_t init, asset_load_t load, asset_free_t free) {
    return (asset_type_t){
        .name = str_new(name),
        .init_callback = init,
        .load_callback = load,
        .free_callback = free
    };
}

static inline void delete_asset_type(asset_type_t *at) {
    if (at) str_delete(at->name);
}

#define _GET_LOAD_ASSET(_1, _2, _3, _4, NAME, ...) NAME
#define load_asset(...) _GET_LOAD_ASSET(__VA_ARGS__, _LOAD_ASSET4, _LOAD_ASSET2, _LOAD_ASSET2, _LOAD_ASSET1)(__VA_ARGS__)
#define _LOAD_ASSET1(name) _load_asset(name, NULL, NULL, NULL)
#define _LOAD_ASSET2(name, path) _load_asset(name, path, NULL, NULL)
#define _LOAD_ASSET4(name, path, ptr, type) _load_asset(name, path, (void**)&ptr, &type)
//#define load_asset(name, path, ptr, type) _load_asset(name, path, (void**)&ptr, &type)
int _load_asset(const char *name, const char *path, void **ptr, asset_type_t *type);
int free_asset(const char *name);

#endif