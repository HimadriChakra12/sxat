#ifndef _10h0ch_history_h
#define _10h0ch_history_h

#include <SDL2/SDL_render.h>
#include <stdint.h>

#define HISTORY_MAX 32

void history_init(void);
void history_deinit(void);

/* snapshot current img_edit_texture onto the undo stack */
void history_push(void);

/* restore previous snapshot; returns 1 on success, 0 if nothing to undo */
uint8_t history_undo(void);

/* re-apply undone snapshot; returns 1 on success, 0 if nothing to redo */
uint8_t history_redo(void);

#endif
