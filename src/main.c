#include <stdio.h>
#include <unistd.h>

#include "rendering.h"
#include "fonts.h"
#include "image.h"
#include "status_bar.h"
#include "tools.h"
#include "history.h"

int main(int argc, char* argv[]) {
    rendering_init();
    image_init();
    fonts_init();
    tools_init();
    status_bar_init();
    history_init();

    image_load(argc > 1 ? argv[1] : NULL);
    history_push(); /* initial snapshot so undo can return to pristine state */
    rendering_handle_window_resized(); // make sure we "refresh" wnd_rect before starting our main loop

    // main loop...
    SDL_Event evt;
    uint8_t event_handler_result = 0;
    while(SDL_WaitEvent(&evt)) {
        if (evt.type == SDL_QUIT) {
            break;
        } else if (evt.type == SDL_KEYDOWN) {
            event_handler_result = tools_handle_keydown(&evt.key);
            if (event_handler_result == 0) {
                if (evt.key.keysym.sym == SDLK_q) break;
            }
        } else if (evt.type == SDL_MOUSEBUTTONDOWN || evt.type == SDL_MOUSEBUTTONUP) {
            tools_handle_mouse_click(&evt.button);
        } else if (evt.type ==  SDL_MOUSEMOTION) {
            tools_handle_mouse_motion(&evt.motion);
        } else if (evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_RESIZED) {
            rendering_handle_window_resized();
        }

        rendering_wipe_screen();

        image_render_img();
        tools_render_tool_ghost();
        status_bar_render_bar();

        rendering_swap_screen();
    }

    if (!isatty(STDOUT_FILENO))
        image_write_img_to_stdout();

    status_bar_deinit();
    tools_deinit();
    fonts_deinit();
    image_deinit();
    history_deinit();
    rendering_deinit();

    return 0;
}
