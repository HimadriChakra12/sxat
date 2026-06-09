#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <errno.h>

#include "rendering.h"
#include "fonts.h"
#include "image.h"
#include "status_bar.h"
#include "tools.h"
#include "history.h"
#include "../config.h"

#include <SDL2/SDL_clipboard.h>

/* expand leading ~ to $HOME */
static void expand_path(const char* in, char* out, size_t outsz) {
    if (in[0] == '~') {
        const char* home = getenv("HOME");
        if (!home) home = "";
        snprintf(out, outsz, "%s%s", home, in + 1);
    } else {
        snprintf(out, outsz, "%s", in);
    }
}

static void mkdirp(const char* path) {
    char tmp[1024];
    snprintf(tmp, sizeof(tmp), "%s", path);
    for (char* p = tmp + 1; *p; p++) {
        if (*p == '/') { *p = '\0'; mkdir(tmp, 0755); *p = '/'; }
    }
    mkdir(tmp, 0755);
}

static void build_save_path(const char* dir, char* out, size_t outsz) {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    char base[256];
    strftime(base, sizeof(base), "liau_%Y%m%d_%H%M%S", tm);
    snprintf(out, outsz, "%s/%s.png", dir, base);
    if (access(out, F_OK) != 0) return;
    for (int n = 1; n < 10000; n++) {
        snprintf(out, outsz, "%s/%s_%d.png", dir, base, n);
        if (access(out, F_OK) != 0) return;
    }
}

static char last_saved_path[1024] = "";

static void do_save(void) {
    char dir[1024];
    expand_path(CFG_SAVE_DIR, dir, sizeof(dir));
    mkdirp(dir);
    build_save_path(dir, last_saved_path, sizeof(last_saved_path));
    if (image_write_img_to_file(last_saved_path) == 0)
        fprintf(stderr, "saved: %s\n", last_saved_path);
    else
        fprintf(stderr, "save failed: %s\n", last_saved_path);
}

static void do_copy_image(void) {
    /* write to a temp file then pipe to xclip */
    char tmp[] = "/tmp/liau_copy_XXXXXX.png";
    /* mkstemp doesn't support suffix so just use a fixed tmp path */
    snprintf(tmp, sizeof(tmp), "/tmp/liau_copy_%d.png", (int)getpid());
    if (image_write_img_to_file(tmp) != 0) {
        fprintf(stderr, "copy: failed to write temp file\n");
        return;
    }
    char cmd[1200];
    snprintf(cmd, sizeof(cmd),
        "xclip -selection clipboard -t image/png < \"%s\" && rm -f \"%s\"", tmp, tmp);
    if (system(cmd) == 0)
        fprintf(stderr, "image copied to clipboard\n");
    else
        fprintf(stderr, "copy failed — is xclip installed?\n");
}

/* detect if stdin has data without blocking (for screenshot tool pipes) */
static int stdin_has_data(void) {
    if (isatty(STDIN_FILENO)) return 0;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    struct timeval tv = { 0, 0 };
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

int main(int argc, char* argv[]) {
    rendering_init();
    image_init();
    fonts_init();
    tools_init();
    status_bar_init();
    history_init();

    /* support:
     *   liau image.png          (argv[1] = path)
     *   liau -f image.png       (swappy-compatible, argv[1]="-f" argv[2]=path)
     *   cat image.png | liau    (stdin pipe)
     */
    const char* path = NULL;
    if (argc >= 3 && strcmp(argv[1], "-f") == 0) {
        path = argv[2];
    } else if (argc >= 2) {
        path = argv[1];
    }

    /* accept stdin pipe only if data is actually available */
    if (path == NULL && !stdin_has_data()) {
        /* block-wait a moment for slow pipes (screenshot tools may take ms) */
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        struct timeval tv = { 1, 0 }; /* 1 second timeout */
        int ready = !isatty(STDIN_FILENO)
                    ? select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv)
                    : 0;
        if (ready <= 0) {
            fprintf(stderr, "usage: liau <image>\n       screenshot_tool | liau\n");
            return 1;
        }
    }

    image_load(path);
    history_push();
    rendering_handle_window_resized();

    SDL_Event evt;
    while (SDL_WaitEvent(&evt)) {
        if (evt.type == SDL_QUIT) {
            break;
        } else if (evt.type == SDL_KEYDOWN) {
            uint8_t ctrl = (evt.key.keysym.mod & KMOD_CTRL) != 0;
            if (ctrl && evt.key.keysym.sym == CFG_KEY_SAVE) {
                do_save();
            } else if (ctrl && evt.key.keysym.sym == CFG_KEY_COPY) {
                do_copy_image();
            } else {
                if (tools_handle_keydown(&evt.key) == 0)
                    if (evt.key.keysym.sym == CFG_KEY_QUIT) break;
            }
        } else if (evt.type == SDL_MOUSEBUTTONDOWN || evt.type == SDL_MOUSEBUTTONUP) {
            tools_handle_mouse_click(&evt.button);
        } else if (evt.type == SDL_MOUSEMOTION) {
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
