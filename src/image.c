#include "image.h"
#include "rendering.h"
#include "../config.h"

#include <png.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>

SDL_Surface* img_orig_surface = NULL;
SDL_Texture* img_orig_texture = NULL;
SDL_Texture* img_edit_texture = NULL;

/* scale factor: image coords * img_scale = window coords */
static float img_scale = 1.0f;

void image_init(void) {
    uint32_t img_flags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    if ((IMG_Init(img_flags) & img_flags) != img_flags) {
        fprintf(stderr, "could not initialize SDL_image: %s\n", IMG_GetError());
        SDL_DestroyWindow(wnd);
        SDL_Quit();
        exit(-1);
    }
}

void image_deinit(void) {
    SDL_DestroyTexture(img_edit_texture);
    SDL_DestroyTexture(img_orig_texture);
    SDL_FreeSurface(img_orig_surface);
    IMG_Quit();
}

SDL_Surface* image_load(const char* path) {
    SDL_Surface* img_surface = NULL;

    if (path != NULL) {
        img_surface = IMG_Load(path);
        if (img_surface == NULL) {
            fprintf(stderr, "SDL_image could not load '%s': %s\n", path, IMG_GetError());
            exit(-1);
        }
    } else {
        void* tmp_buff = NULL;
        int img_buff_offset = 0;
        size_t img_buff_size = 0x1000;
        uint8_t* img_buff = (uint8_t*)malloc(img_buff_size * sizeof(uint8_t));

        int c;
        while ((c = getchar()) != EOF) {
            if ((size_t)img_buff_offset == img_buff_size) {
                img_buff_size += 0x1000;
                if ((tmp_buff = realloc(img_buff, img_buff_size * sizeof(uint8_t))) != NULL) {
                    img_buff = (uint8_t*)tmp_buff;
                } else {
                    fprintf(stderr, "could not resize input image buffer\n");
                    free(img_buff);
                    exit(-1);
                }
            }
            img_buff[img_buff_offset++] = c;
        }

        if (img_buff_offset == 0) {
            fprintf(stderr, "usage: liau <image>\n       cat image.png | liau\n");
            exit(-1);
        }

        if (img_buff_offset < (int)img_buff_size)
            img_buff = realloc(img_buff, img_buff_offset);

        SDL_RWops* sdl_img_buff = SDL_RWFromMem(img_buff, img_buff_offset);
        if (!sdl_img_buff) {
            fprintf(stderr, "SDL_RWFromMem: %s\n", SDL_GetError());
            free(img_buff);
            exit(-1);
        }

        img_surface = IMG_Load_RW(sdl_img_buff, 0);
        if (!img_surface) {
            fprintf(stderr, "SDL_image load: %s\n", IMG_GetError());
            SDL_RWclose(sdl_img_buff);
            free(img_buff);
            exit(-1);
        }

        SDL_RWclose(sdl_img_buff);
        free(img_buff);
    }

    img_orig_surface = img_surface;
    img_orig_texture = SDL_CreateTextureFromSurface(rend, img_orig_surface);
    SDL_SetTextureBlendMode(img_orig_texture, SDL_BLENDMODE_NONE);

    img_edit_texture = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA32,
                           SDL_TEXTUREACCESS_TARGET,
                           img_orig_surface->w, img_orig_surface->h);
    SDL_SetTextureBlendMode(img_edit_texture, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(rend, img_edit_texture);
    SDL_RenderCopy(rend, img_orig_texture, NULL, NULL);
    SDL_SetRenderTarget(rend, NULL);

    return img_surface;
}

/* -------------------------------------------------------------------------
 * Scale / offset helpers
 * The image is scaled to fit the window (minus status bar) while keeping
 * aspect ratio.  All tool coords are in *image space*, not window space.
 * ------------------------------------------------------------------------- */

static void image_compute_scale(void) {
    int avail_w = wnd_rect.w;
    int avail_h = wnd_rect.h - CFG_BAR_HEIGHT;
    float sx = (float)avail_w / img_orig_surface->w;
    float sy = (float)avail_h / img_orig_surface->h;
    img_scale = sx < sy ? sx : sy;
    if (img_scale > 1.0f) img_scale = 1.0f; /* never upscale */
}

SDL_Rect image_get_offset_rect(void) {
    image_compute_scale();
    int sw = (int)(img_orig_surface->w * img_scale);
    int sh = (int)(img_orig_surface->h * img_scale);
    return (SDL_Rect){
        .x = (wnd_rect.w - sw) / 2,
        .y = ((wnd_rect.h - CFG_BAR_HEIGHT) - sh) / 2,
        .w = sw,
        .h = sh,
    };
}

void image_window_to_image(int wx, int wy, int* ix, int* iy) {
    image_compute_scale();
    SDL_Rect r = image_get_offset_rect();
    *ix = (int)((wx - r.x) / img_scale);
    *iy = (int)((wy - r.y) / img_scale);
}

void image_render_img(void) {
    SDL_Rect dst = image_get_offset_rect();
    SDL_SetRenderTarget(rend, NULL);
    SDL_RenderCopy(rend, img_orig_texture, NULL, &dst);
    SDL_RenderCopy(rend, img_edit_texture, NULL, &dst);
}

/* -------------------------------------------------------------------------
 * Save: render edit texture to a full-res offscreen surface, write PNG.
 * We never read from the screen — that would be capped to window size.
 * ------------------------------------------------------------------------- */

static int image_write_png_to_fp(FILE* fp) {
    int w = img_orig_surface->w;
    int h = img_orig_surface->h;

    /* render orig + edits into a full-res offscreen texture */
    SDL_Texture* out = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA32,
                           SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(out, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(rend, out);
    SDL_RenderCopy(rend, img_orig_texture, NULL, NULL);
    SDL_SetTextureBlendMode(img_edit_texture, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(rend, img_edit_texture, NULL, NULL);
    SDL_SetTextureBlendMode(img_edit_texture, SDL_BLENDMODE_NONE);

    uint8_t* pixels = (uint8_t*)malloc(w * h * 4);
    uint32_t stride = w * 4;
    SDL_Rect full = { 0, 0, w, h };
    SDL_RenderReadPixels(rend, &full, SDL_PIXELFORMAT_RGBA32, pixels, stride);
    SDL_SetRenderTarget(rend, NULL);
    SDL_DestroyTexture(out);

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) { free(pixels); return -1; }
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) { png_destroy_write_struct(&png_ptr, NULL); free(pixels); return -1; }
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        free(pixels);
        return -1;
    }

    uint8_t** rows = (uint8_t**)malloc(h * sizeof(uint8_t*));
    for (int i = 0; i < h; i++) rows[i] = pixels + stride * i;

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, w, h, 8, PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_set_rows(png_ptr, info_ptr, rows);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    free(rows);
    free(pixels);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
}

void image_write_img_to_stdout(void) {
    image_write_png_to_fp(stdout);
}

int image_write_img_to_file(const char* path) {
    FILE* fp = fopen(path, "wb");
    if (!fp) { fprintf(stderr, "could not open '%s' for writing\n", path); return -1; }
    int ret = image_write_png_to_fp(fp);
    fclose(fp);
    return ret;
}
