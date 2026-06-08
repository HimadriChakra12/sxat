#include "history.h"
#include "image.h"
#include "rendering.h"
#include <stdlib.h>

static SDL_Texture* snapshots[HISTORY_MAX];
static int head  = -1; /* index of current state */
static int count = 0;  /* how many valid snapshots exist */

static SDL_Texture* alloc_snapshot(void) {
    return SDL_CreateTexture(
        rend,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        img_orig_surface->w,
        img_orig_surface->h
    );
}

static void copy_edit_to(SDL_Texture* dst) {
    SDL_SetRenderTarget(rend, dst);
    SDL_SetTextureBlendMode(dst, SDL_BLENDMODE_NONE);
    SDL_RenderCopy(rend, img_edit_texture, NULL, NULL);
    SDL_SetRenderTarget(rend, NULL);
}

static void copy_from_to_edit(SDL_Texture* src) {
    SDL_SetRenderTarget(rend, img_edit_texture);
    SDL_SetTextureBlendMode(img_edit_texture, SDL_BLENDMODE_NONE);
    SDL_RenderCopy(rend, src, NULL, NULL);
    SDL_SetRenderTarget(rend, NULL);
}

void history_init(void) {
    for (int i = 0; i < HISTORY_MAX; i++) snapshots[i] = NULL;
    head  = -1;
    count = 0;
}

void history_deinit(void) {
    for (int i = 0; i < HISTORY_MAX; i++) {
        if (snapshots[i]) { SDL_DestroyTexture(snapshots[i]); snapshots[i] = NULL; }
    }
}

void history_push(void) {
    /* discard any redo states above head */
    for (int i = head + 1; i < count; i++) {
        if (snapshots[i]) { SDL_DestroyTexture(snapshots[i]); snapshots[i] = NULL; }
    }
    count = head + 1;

    /* if full, drop the oldest entry by shifting */
    if (count == HISTORY_MAX) {
        if (snapshots[0]) SDL_DestroyTexture(snapshots[0]);
        for (int i = 0; i < HISTORY_MAX - 1; i++) snapshots[i] = snapshots[i + 1];
        snapshots[HISTORY_MAX - 1] = NULL;
        count--;
        head--;
    }

    head++;
    if (snapshots[head] == NULL) snapshots[head] = alloc_snapshot();
    copy_edit_to(snapshots[head]);
    count = head + 1;
}

uint8_t history_undo(void) {
    if (head <= 0) return 0;
    head--;
    copy_from_to_edit(snapshots[head]);
    return 1;
}

uint8_t history_redo(void) {
    if (head >= count - 1) return 0;
    head++;
    copy_from_to_edit(snapshots[head]);
    return 1;
}
