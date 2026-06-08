#ifndef _10h0ch_image_h
#define _10h0ch_image_h

#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_render.h>

extern SDL_Surface* img_orig_surface;
extern SDL_Texture* img_orig_texture;
extern SDL_Texture* img_edit_texture;

/**
 * initialize the image module
 */
void image_init(void);

/**
 * deinitialize the image module
 */
void image_deinit(void);

/**
 * render the image and its "edit layer" to the center of our window
 */
void image_render_img(void);

/**
 * Load an image from a file path (if path != NULL) or from stdin.
 * Initializes the img_orig_... and img_edit_... module vars.
 */
SDL_Surface* image_load(const char* path);

/**
 */
void image_write_img_to_stdout(void);


/**
 * returns an SDL_Rect which defines the visible image region relative to the window.
 * x/ y define the offset and width/height should match img_orig_surface's clip_rect
 * dimensions.
 */
SDL_Rect image_get_offset_rect(void);

#endif
