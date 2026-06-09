#include "rendering.h"
#include "status_bar.h"
#include "tools.h"
#include "../config.h"
#include <SDL2/SDL_ttf.h>

static TTF_Font* status_bar_font = NULL;
static SDL_Color status_bar_color = {
    .r = _CFG_R(CFG_BAR_FG),
    .g = _CFG_G(CFG_BAR_FG),
    .b = _CFG_B(CFG_BAR_FG),
    .a = _CFG_A(CFG_BAR_FG),
};

void status_bar_init(void) {
    status_bar_font = fonts_load_font(CFG_BAR_FONT, CFG_BAR_FONTSIZE);
}

void status_bar_deinit(void) {
    TTF_CloseFont(status_bar_font);
    status_bar_font = NULL;
}

void status_bar_render_bar(void) {
    SDL_Rect status_bar_rect = {
        .x = 0,
        .y = wnd_rect.h - CFG_BAR_HEIGHT,
        .w = wnd_rect.w,
        .h = CFG_BAR_HEIGHT,
    };
    SDL_SetRenderDrawColor(rend,
        _CFG_R(CFG_BAR_BG), _CFG_G(CFG_BAR_BG),
        _CFG_B(CFG_BAR_BG), _CFG_A(CFG_BAR_BG));
    SDL_RenderFillRect(rend, &status_bar_rect);

    char* status_bar_tool_text = tools_get_status_bar_text();
    if (status_bar_tool_text != NULL) {
        SDL_Surface* s = fonts_render_text(status_bar_tool_text, status_bar_font, status_bar_color);
        SDL_Texture* t = SDL_CreateTextureFromSurface(rend, s);
        SDL_Rect tr = {
            .x = status_bar_rect.x + 5,
            .y = status_bar_rect.y + (status_bar_rect.h>>1) - (s->clip_rect.h>>1),
            .w = s->clip_rect.w,
            .h = s->clip_rect.h,
        };
        SDL_RenderCopy(rend, t, &(s->clip_rect), &tr);
        SDL_DestroyTexture(t);
        SDL_FreeSurface(s);
    }
}
