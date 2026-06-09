#ifndef _10h0ch_status_bar_h
#define _10h0ch_status_bar_h

#include "fonts.h"
#include "../config.h"
#include <SDL2/SDL_render.h>

#define STATUS_BAR_HEIGHT CFG_BAR_HEIGHT
/**
 * initialize the status bar module
 */
void status_bar_init(void);

/**
 * deinitialize the status bar module
 */
void status_bar_deinit(void);

/**
 */
void status_bar_render_bar(void);


#endif
