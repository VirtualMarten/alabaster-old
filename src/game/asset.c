#include "game/asset.h"
#include "game/base.h"

#include "ae/array.h"

#include <string.h>
#include <stdio.h>

static asset_t *assets = NULL;

static inline asset_t* get_asset(const char *name, unsigned int *index) {
    for (unsigned int i = 0; i < arr_len(assets); i++) {
        if (!strncmp(assets[i].name, name, str_len(assets[i].name))) {
            if (index) *index = i;
            return &assets[i];
        }
    }
    return NULL;
}

void _free_asset(asset_t *asset, unsigned int i) {
    str_del(asset->name);
    str_del(asset->path);
    arr_remove(assets, i);
}

int _load_asset(const char *name, const char *path, void **ptr, asset_type_t *type) {
    unsigned int i;
    asset_t *asset;
    if ((asset = get_asset(name, &i)) != NULL) {
        if (type || ptr) {
            _free_asset(asset, i);
            goto new_asset;
        }
        else if (path != NULL)
            str_set(asset->path, path);
    }
    else {
        new_asset:
        if (!assets)
            assets = arr_new(sizeof(asset_t), 1, 1);
        arr_add(assets, (asset_t){
            .path = str_new(path),
            .name = str_new(name),
            .type = type,
            .initialized = false,
            .ptr = ptr
        });
        i = arr_len(assets) - 1;
        asset = &assets[i];
        if (!asset->type->init_callback(asset)) {
            fprintf(stderr, "Failed to initialize %s asset \"%s\"!\n", type->name, asset->name);
            _free_asset(asset, i);
            return 0;
        }
        else asset->initialized = true;
    }
    if (!asset->type->load_callback(asset)) {
        fprintf(stderr, "Failed to load %s asset \"%s\"!\n", asset->type->name, asset->name);
        return 0;
    }
    else printf("Loaded %s asset \"%s\".\n", asset->type->name, asset->name);
    return 1;
}

int free_asset(const char *name) {
    unsigned int i;
    asset_t *asset = get_asset(name, &i);
    if (asset != NULL) {
        asset->type->free_callback(asset);
        _free_asset(asset, i);
    }
    return 1;
}