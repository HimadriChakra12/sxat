#include "tool.h"
#include "../../config.h"
#include "rendering.h"
#include "image.h"
#include "history.h"
#include "hex.h"
#include "input_util.h"
#include <string.h>
#include <SDL2/SDL_clipboard.h>

typedef enum {
    RECT_MODE_DRAW,
    RECT_MODE_INPUT_COLOR,
    RECT_MODE_INPUT_ALPHA
} rect_mode_t;

static rect_mode_t  rect_mode     = RECT_MODE_DRAW;
static input_util_t rect_input_util;
static char         status_bar_buff[192] = "\0";

static uint32_t rect_color    = CFG_RECT_DEFAULT_COLOR;
static uint8_t  rect_filled   = 0;
static uint8_t  rect_thickness = CFG_RECT_DEFAULT_THICKNESS;
static int32_t  drag_start_x  = -1;
static int32_t  drag_start_y  = -1;
static int32_t  drag_cur_x    = -1;
static int32_t  drag_cur_y    = -1;
static uint8_t  dragging      = 0;

/* paste clipboard text into an input_util buffer, filtering chars */
static void paste_clipboard(input_util_t* iu) {
    if (!SDL_HasClipboardText()) return;
    char* text = SDL_GetClipboardText();
    if (!text) return;
    input_util_wipe(iu);
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        /* lowercase hex */
        if (c >= 'A' && c <= 'F') c = c - 'A' + 'a';
        if (iu->key_filter(c)) INPUT_UTIL_ADD(iu, c);
    }
    SDL_free(text);
}

static uint8_t rect_handle_activate(void) {
    SDL_ShowCursor(SDL_ENABLE);
    rect_mode = RECT_MODE_DRAW;
    dragging  = 0;
    return 1;
}

static uint8_t rect_handle_deactivate(void) {
    dragging  = 0;
    rect_mode = RECT_MODE_DRAW;
    return 1;
}

static uint8_t rect_handle_keydown(SDL_KeyboardEvent* evt) {
    if (evt->keysym.sym == CFG_KEY_QUIT) return 1;

    uint8_t ctrl = (evt->keysym.mod & KMOD_CTRL) != 0;

    /* color / alpha input mode */
    if (rect_mode == RECT_MODE_INPUT_COLOR || rect_mode == RECT_MODE_INPUT_ALPHA) {
        if (evt->keysym.sym == SDLK_ESCAPE) {
            rect_mode = RECT_MODE_DRAW;
        } else if (ctrl && evt->keysym.sym == CFG_KEY_PASTE) {
            paste_clipboard(&rect_input_util);
        } else if (evt->keysym.sym == SDLK_RETURN || evt->keysym.sym == SDLK_RETURN2) {
            if (rect_mode == RECT_MODE_INPUT_COLOR)
                rect_color = (hex_parse_hex_str(rect_input_util.buff) << 8) | (rect_color & 0xff);
            else
                rect_color = (rect_color & ~0xff) | (hex_parse_hex_str(rect_input_util.buff) & 0xff);
            rect_mode = RECT_MODE_DRAW;
        } else {
            input_util_handle_keydown(&rect_input_util, evt);
        }
        return 1;
    }

    switch (evt->keysym.sym) {
        case CFG_KEY_RECT_TOGGLE_FILL:
            rect_filled = !rect_filled;
            return 1;

        case CFG_KEY_SIZE_DEC:
            if (rect_thickness > 1) rect_thickness--;
            return 1;

        case CFG_KEY_SIZE_INC:
            if (rect_thickness < 64) rect_thickness++;
            return 1;

        case CFG_KEY_COLOR_INPUT:
            rect_mode = RECT_MODE_INPUT_COLOR;
            input_util_reset(&rect_input_util, 6, input_util_hex_key_filter);
            return 1;

        case CFG_KEY_ALPHA_INPUT:
            rect_mode = RECT_MODE_INPUT_ALPHA;
            input_util_reset(&rect_input_util, 2, input_util_hex_key_filter);
            return 1;
    }

    return 0;
}

static uint8_t rect_handle_mouse_motion(SDL_MouseMotionEvent* evt) {
    if (!dragging) return 0;
    drag_cur_x = evt->x;
    drag_cur_y = evt->y;
    return 1;
}

/* draw a filled rect using SDL, respecting thickness for outline mode */
static void draw_rect_on_texture(SDL_Texture* tex,
                                  int32_t x0, int32_t y0,
                                  int32_t x1, int32_t y1) {
    uint8_t cr = (rect_color >> 24) & 0xff;
    uint8_t cg = (rect_color >> 16) & 0xff;
    uint8_t cb = (rect_color >>  8) & 0xff;
    uint8_t ca =  rect_color        & 0xff;

    SDL_SetRenderTarget(rend, tex);
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rend, cr, cg, cb, ca);

    if (rect_filled) {
        SDL_Rect r = { .x = x0, .y = y0, .w = x1 - x0, .h = y1 - y0 };
        SDL_RenderFillRect(rend, &r);
    } else {
        /* draw multiple nested rects to achieve thickness */
        for (uint8_t i = 0; i < rect_thickness; i++) {
            SDL_Rect r = { .x = x0 + i, .y = y0 + i,
                           .w = x1 - x0 - 2*i, .h = y1 - y0 - 2*i };
            if (r.w <= 0 || r.h <= 0) break;
            SDL_RenderDrawRect(rend, &r);
        }
    }

    SDL_SetRenderTarget(rend, NULL);
}

static uint8_t rect_handle_mouse_click(SDL_MouseButtonEvent* evt) {
    if (evt->button != SDL_BUTTON_LEFT) return 0;

    if (evt->state == SDL_PRESSED) {
        drag_start_x = evt->x;
        drag_start_y = evt->y;
        drag_cur_x   = evt->x;
        drag_cur_y   = evt->y;
        dragging     = 1;
        return 1;
    }

    if (evt->state == SDL_RELEASED && dragging) {
        dragging = 0;

        int32_t x0, y0, x1, y1;
        image_window_to_image(drag_start_x, drag_start_y, &x0, &y0);
        image_window_to_image(evt->x, evt->y, &x1, &y1);

        if (x0 > x1) { int32_t t = x0; x0 = x1; x1 = t; }
        if (y0 > y1) { int32_t t = y0; y0 = y1; y1 = t; }

        if (x0 < 0) x0 = 0;
        if (y0 < 0) y0 = 0;
        if (x1 > img_orig_surface->w) x1 = img_orig_surface->w;
        if (y1 > img_orig_surface->h) y1 = img_orig_surface->h;

        if (x1 <= x0 || y1 <= y0) return 1;

        history_push();
        draw_rect_on_texture(img_edit_texture, x0, y0, x1, y1);
        return 1;
    }

    return 0;
}

static char* rect_provide_status_bar_text(void) {
    if (rect_mode == RECT_MODE_DRAW) {
        sprintf(status_bar_buff,
            "[rect] | color=#%06x alpha=#%02x | %s | thickness=%d | c=color a=alpha f=fill [/]=size",
            rect_color >> 8, rect_color & 0xff,
            rect_filled ? "filled" : "outline",
            rect_thickness);
    } else if (rect_mode == RECT_MODE_INPUT_COLOR) {
        sprintf(status_bar_buff, "[rect] | set color: #%s_ (ctrl+v to paste)", rect_input_util.buff);
    } else if (rect_mode == RECT_MODE_INPUT_ALPHA) {
        sprintf(status_bar_buff, "[rect] | set alpha: #%s_ (ctrl+v to paste)", rect_input_util.buff);
    }
    return status_bar_buff;
}

static void rect_render_ghost(void) {
    if (!dragging) return;

    SDL_Rect img_rect = image_get_offset_rect();

    int32_t x0 = drag_start_x < drag_cur_x ? drag_start_x : drag_cur_x;
    int32_t y0 = drag_start_y < drag_cur_y ? drag_start_y : drag_cur_y;
    int32_t x1 = drag_start_x > drag_cur_x ? drag_start_x : drag_cur_x;
    int32_t y1 = drag_start_y > drag_cur_y ? drag_start_y : drag_cur_y;

    if (x0 < img_rect.x) x0 = img_rect.x;
    if (y0 < img_rect.y) y0 = img_rect.y;
    if (x1 > img_rect.x + img_rect.w) x1 = img_rect.x + img_rect.w;
    if (y1 > img_rect.y + img_rect.h) y1 = img_rect.y + img_rect.h;

    uint8_t cr = (rect_color >> 24) & 0xff;
    uint8_t cg = (rect_color >> 16) & 0xff;
    uint8_t cb = (rect_color >>  8) & 0xff;

    SDL_SetRenderTarget(rend, NULL);
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rend, cr, cg, cb, 0xa0);

    if (rect_filled) {
        SDL_Rect r = { .x = x0, .y = y0, .w = x1 - x0, .h = y1 - y0 };
        SDL_RenderFillRect(rend, &r);
    } else {
        for (uint8_t i = 0; i < rect_thickness; i++) {
            SDL_Rect r = { .x = x0 + i, .y = y0 + i,
                           .w = x1 - x0 - 2*i, .h = y1 - y0 - 2*i };
            if (r.w <= 0 || r.h <= 0) break;
            SDL_RenderDrawRect(rend, &r);
        }
    }
}

tool_t tools_tool_rect = {
    .activate_handler         = rect_handle_activate,
    .deactivate_handler       = rect_handle_deactivate,
    .keydown_handler          = rect_handle_keydown,
    .mouse_motion_handler     = rect_handle_mouse_motion,
    .mouse_click_handler      = rect_handle_mouse_click,
    .status_bar_text_provider = rect_provide_status_bar_text,
    .ghost_renderer           = rect_render_ghost,
};
